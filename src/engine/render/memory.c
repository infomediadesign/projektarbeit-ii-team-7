#include "memory.h"

#include "../util.h"

void memory_create_manager(RenderState *state, MemoryManager *m) {
  m->pools = (MemoryPool *)calloc(1, sizeof(MemoryPool));

  memory_allocate_pool(state, m->pools);
}

void memory_allocate_pool(RenderState *state, MemoryPool *mp) {
  mp->free         = (FreeList *)calloc(1, sizeof(FreeList));
  mp->free->next   = NULL;
  mp->free->offset = 0;
  mp->free->size   = util_mebibytes(64);

  VkMemoryRequirements memory_requirements;

  const VkBufferCreateInfo buffer_info = {
    .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
    .pNext = NULL,
    .size  = util_mebibytes(64),
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

FreeList *memory_find_free_block(const MemoryPool *m, const u64 size) {
  FreeList *l = m->free;

  while (l != NULL) {
    if (l->size >= size)
      break;

    l = l->next;
  }

  return l;
}