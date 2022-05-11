#ifndef __ENGINE_RENDER_MEMORY_H
#define __ENGINE_RENDER_MEMORY_H

#include "geyser.h"
#include "render_state.h"

typedef struct FreeList {
  struct FreeList *next;
  u64 offset;
  u64 size;
} FreeList;

typedef struct MemoryPool {
  struct MemoryPool *next;
  FreeList *free;
  VkBuffer buffer;
  VkDeviceMemory memory;
  VkDeviceSize size;
} MemoryPool;

typedef struct MemoryManager {
  MemoryPool *pools;
} MemoryManager;

/**
 * @brief Creates a memory manager.
 * 
 * @param state The render state.
 * @param m The memory manager struct to write to.
 */
void memory_create_manager(RenderState *state, MemoryManager *m);
void memory_allocate_pool(RenderState *state, MemoryPool *m);
void memory_extend_pool(RenderState *state, MemoryPool *pool);
FreeList *memory_find_free_block(const MemoryPool *m, const u64 size);

#endif
