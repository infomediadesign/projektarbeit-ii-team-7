#include "memory.h"

#include "../crc64.h"
#include "../limits.h"
#include "../util.h"
#include "geyser.h"

MemoryComponent *_find_existing_memory_block(MemoryManager *manager, const u64 crc) {
  if (crc == 0)
    return NULL;

  for (u32 i = 0; i < MAX_MEMORY_COMPONENTS; i++)
    if (manager->components[i].crc != 0 && manager->components[i].crc == crc)
      return &manager->components[i];

  return NULL;
}

void _add_memory_component(MemoryManager *manager, const u64 crc, MemoryPool *mp, ImageMemoryPool *imp, FreeList *fl) {
  if (crc == 0)
    return;

  for (u32 i = 0; i < MAX_MEMORY_COMPONENTS; i++) {
    if (manager->components[i].crc == 0) {
      manager->components[i].crc        = crc;
      manager->components[i].pool       = mp;
      manager->components[i].image_pool = imp;
      manager->components[i].free_list  = fl;

      return;
    }
  }
}

void memory_create_manager(RenderState *state, MemoryManager *m) {
  m->pools       = (MemoryPool *)calloc(1, sizeof(MemoryPool));
  m->image_pools = (ImageMemoryPool *)calloc(1, sizeof(ImageMemoryPool));
  m->components  = (MemoryComponent *)calloc(MAX_MEMORY_COMPONENTS, sizeof(MemoryComponent));

  memory_allocate_pool(state, m->pools);
  memory_allocate_image_pool(state, m->image_pools);
}

void memory_allocate_pool(RenderState *state, MemoryPool *mp) {
  mp->free         = (FreeList *)calloc(1, sizeof(FreeList));
  mp->free->next   = NULL;
  mp->free->offset = 0;
  mp->free->size   = util_mebibytes(MEMORY_POOL_SIZE);

  VkMemoryRequirements memory_requirements;

  const VkBufferCreateInfo buffer_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = NULL,
    .size  = util_mebibytes(MEMORY_POOL_SIZE),
    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
    .flags = 0,
  };

  geyser_success_or_message(
    vkCreateBuffer(state->device, &buffer_info, NULL, &mp->buffer), "Failed to create a memory pool buffer!"
  );

  vkGetBufferMemoryRequirements(state->device, mp->buffer, &memory_requirements);

  const VkMemoryAllocateInfo memory_alloc_info = {
    GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
    .allocationSize  = memory_requirements.size,
    .memoryTypeIndex = geyser_get_memory_type_index(state, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
  };

  geyser_success_or_message(
    vkAllocateMemory(state->device, &memory_alloc_info, NULL, &mp->memory), "Failed to allocate memory pool memory!"
  );

  mp->size = memory_alloc_info.allocationSize;

  vkBindBufferMemory(state->device, mp->buffer, mp->memory, 0);
}

void memory_allocate_image_pool(RenderState *state, ImageMemoryPool *mp) {
  mp->free         = (FreeList *)calloc(1, sizeof(FreeList));
  mp->free->next   = NULL;
  mp->free->offset = 0;
  mp->free->size   = util_mebibytes(MEMORY_POOL_SIZE);

  const VkMemoryAllocateInfo memory_alloc_info = {
    GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
    .allocationSize  = util_mebibytes(MEMORY_POOL_SIZE),
    .memoryTypeIndex = geyser_get_memory_type_index(state, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)
  };

  geyser_success_or_message(
    vkAllocateMemory(state->device, &memory_alloc_info, NULL, &mp->memory), "Failed to allocate memory pool memory!"
  );

  mp->size = memory_alloc_info.allocationSize;
}

void memory_extend_pool(RenderState *state, MemoryPool *pool) {
  if (pool->next != NULL) {
    memory_extend_pool(state, pool->next);
    return;
  }

  pool->next = (MemoryPool *)calloc(1, sizeof(MemoryPool));
  memory_allocate_pool(state, pool->next);
}

void memory_extend_image_pool(RenderState *state, ImageMemoryPool *pool) {
  if (pool->next != NULL) {
    memory_extend_image_pool(state, pool->next);
    return;
  }

  pool->next = (ImageMemoryPool *)calloc(1, sizeof(ImageMemoryPool));
  memory_allocate_image_pool(state, pool->next);
}

FreeList *memory_pool_find_free_block(const MemoryPool *m, const u64 size) {
  FreeList *l = m->free;

  while (l != NULL) {
    if (l->size >= size)
      break;

    l = l->next;
  }

  return l;
}

FreeList *memory_pool_find_free_block_aligned(const ImageMemoryPool *m, const u64 alignment, const u64 size) {
  FreeList *l = m->free;

  while (l != NULL) {
    if (l->offset % alignment == 0) {
      if (l->size >= size)
        break;
    } else {
      const u64 diff = alignment - l->offset % alignment;

      if (l->size - diff >= size) {
        /* This has bugs. TODO: fix this */
        l->offset += diff;
        l->size -= diff;

        break;
      }
    }

    l = l->next;
  }

  return l;
}

void memory_find_free_block(
  RenderState *state, MemoryManager *m, const u64 crc, const u64 size, FreeMemoryBlock *block
) {
  if (size > util_mebibytes(MEMORY_POOL_SIZE)) {
    printf(
      "[Geyser Error] Cannot assign more than %lu MiB of GPU memory! (%lu bytes requested)\n", MEMORY_POOL_SIZE, size
    );
    abort();
  }

  if (m == NULL || m->pools == NULL) {
    printf("[Geyser Error] Memory manager is not initialized!\n");
    abort();
  }

  MemoryComponent *mc = _find_existing_memory_block(m, crc);

  if (mc != NULL) {
    block->pool     = mc->pool;
    block->free     = mc->free_list;
    block->newblock = 0;

    return;
  }

  FreeList *l      = NULL;
  MemoryPool *pool = m->pools;

  while (pool != NULL) {
    l = memory_pool_find_free_block(pool, size);

    if (l != NULL)
      break;

    pool = pool->next;
  }

  if (l == NULL) {
    memory_extend_pool(state, m->pools);
    memory_find_free_block(state, m, crc, size, block);

    return;
  }

  _add_memory_component(m, crc, pool, NULL, l);

  block->pool     = pool;
  block->free     = l;
  block->newblock = 1;
}

void memory_find_free_image_block(
  RenderState *state, MemoryManager *m, const u64 alignment, const u64 crc, const u64 size, FreeImageMemoryBlock *block
) {
  if (size > util_mebibytes(MEMORY_POOL_SIZE)) {
    printf(
      "[Geyser Error] Cannot assign more than %lu MiB of GPU memory! (%lu bytes requested)\n", MEMORY_POOL_SIZE, size
    );
    abort();
  }

  if (m == NULL || m->image_pools == NULL) {
    printf("[Geyser Error] Memory manager is not initialized!\n");
    abort();
  }

  MemoryComponent *mc = _find_existing_memory_block(m, crc);

  if (mc != NULL) {
    block->pool     = mc->image_pool;
    block->free     = mc->free_list;
    block->newblock = 0;

    return;
  }

  FreeList *l           = NULL;
  ImageMemoryPool *pool = m->image_pools;

  while (pool != NULL) {
    l = memory_pool_find_free_block_aligned(pool, alignment, size);

    if (l != NULL)
      break;

    pool = pool->next;
  }

  if (l == NULL) {
    memory_extend_image_pool(state, m->image_pools);
    memory_find_free_image_block(state, m, alignment, crc, size, block);

    return;
  }

  _add_memory_component(m, crc, NULL, pool, l);

  block->pool     = pool;
  block->free     = l;
  block->newblock = 1;
}

void memory_free_block(MemoryPool *pool, const u64 offset, const u64 size) {
  FreeList *l = pool->free;

  FreeList *new_freelist = (FreeList *)calloc(1, sizeof(FreeList));
  new_freelist->next     = l;
  new_freelist->offset   = offset;
  new_freelist->size     = size;

  if (offset + size == l->offset) {
    new_freelist->size += l->size;
    new_freelist->next = l->next;

    free(l);
  }

  pool->free = new_freelist;
}

void memory_free_image_block(ImageMemoryPool *pool, const u64 offset, const u64 size) {
  FreeList *l = pool->free;

  FreeList *new_freelist = (FreeList *)calloc(1, sizeof(FreeList));
  new_freelist->next     = l;
  new_freelist->offset   = offset;
  new_freelist->size     = size;

  if (offset + size == l->offset) {
    new_freelist->size += l->size;
    new_freelist->next = l->next;

    free(l);
  }

  pool->free = new_freelist;
}
