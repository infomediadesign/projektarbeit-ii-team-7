#include "memory.h"

#include "../util.h"
#include "geyser.h"

void memory_create_manager(RenderState *state, MemoryManager *m) {
  m->pools = (MemoryPool *)calloc(1, sizeof(MemoryPool));

  memory_allocate_pool(state, m->pools);
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

void memory_extend_pool(RenderState *state, MemoryPool *pool) {
  if (pool->next != NULL) {
    memory_extend_pool(state, pool->next);
    return;
  }

  pool->next = (MemoryPool *)calloc(1, sizeof(MemoryPool));
  memory_allocate_pool(state, pool->next);
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

FreeList *memory_pool_find_free_block_aligned(const MemoryPool *m, const u64 alignment, const u64 size) {
  FreeList *l = m->free;

  while (l != NULL) {
    const u64 diff = alignment - l->offset % alignment;

    if (l->size - diff >= size) {
      /* This has bugs. TODO: fix this */
      l->offset += diff;
      l->size -= diff;

      break;
    }

    l = l->next;
  }

  return l;
}

void memory_find_free_block(
  RenderState *state, MemoryManager *m, const u64 alignment, const u64 size, FreeMemoryBlock *block
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

  FreeList *l      = NULL;
  MemoryPool *pool = m->pools;

  while (pool != NULL) {
    if (alignment == 0)
      l = memory_pool_find_free_block(pool, size);
    else
      l = memory_pool_find_free_block_aligned(pool, alignment, size);

    if (l != NULL)
      break;

    pool = pool->next;
  }

  if (l == NULL) {
    memory_extend_pool(state, m->pools);
    memory_find_free_block(state, m, alignment, size, block);

    return;
  }

  block->pool = pool;
  block->free = l;
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
