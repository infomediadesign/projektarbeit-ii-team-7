#ifndef __ENGINE_RENDER_MEMORY_H
#define __ENGINE_RENDER_MEMORY_H

/* Size of memory pool blocks, in mebibytes (MUST be a multiple of 64) */
#define MEMORY_POOL_SIZE 64LU

#include "render_state.h"

typedef struct MemoryComponent {
  u64 crc;
  u64 offset;
  u64 size;
  u32 pool_id;
} MemoryComponent;

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

typedef struct ImageMemoryPool {
  struct ImageMemoryPool *next;
  FreeList *free;
  VkDeviceMemory memory;
  VkDeviceSize size;
} ImageMemoryPool;

typedef struct MemoryManager {
  MemoryPool *pools;
  ImageMemoryPool *image_pools;
  MemoryComponent *components;
} MemoryManager;

typedef struct FreeMemoryBlock {
  MemoryPool *pool;
  FreeList *free;
} FreeMemoryBlock;

typedef struct FreeImageMemoryBlock {
  ImageMemoryPool *pool;
  FreeList *free;
} FreeImageMemoryBlock;

/**
 * @brief Creates a memory manager.
 *
 * @param state The render state.
 * @param m The memory manager struct to write to.
 */
void memory_create_manager(RenderState *state, MemoryManager *m);
void memory_allocate_pool(RenderState *state, MemoryPool *m);
void memory_extend_pool(RenderState *state, MemoryPool *pool);
void memory_allocate_image_pool(RenderState *state, ImageMemoryPool *m);
void memory_extend_image_pool(RenderState *state, ImageMemoryPool *pool);
FreeList *memory_pool_find_free_block(const MemoryPool *m, const u64 size);
FreeList *memory_pool_find_free_block_aligned(const ImageMemoryPool *m, const u64 alignment, const u64 size);
void memory_find_free_block(RenderState *state, MemoryManager *m, const u64 size, FreeMemoryBlock *block);
void memory_find_free_image_block(
  RenderState *state, MemoryManager *m, const u64 alignment, const u64 size, FreeImageMemoryBlock *block
);
void memory_free_block(MemoryPool *pool, const u64 offset, const u64 size);
void memory_free_image_block(ImageMemoryPool *pool, const u64 offset, const u64 size);

#endif
