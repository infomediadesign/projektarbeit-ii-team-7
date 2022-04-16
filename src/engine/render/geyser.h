#ifndef __ENGINE_RENDER_GEYSER_H
#define __ENGINE_RENDER_GEYSER_H

/**
 * Geyser is a minimalistic Vulkan middleware library.
 * 
 * It is domain-specific and isn't designed to be used
 * anywhere outside of this project, however it could be
 * used with some adaptation.
 * 
 */
#include <glad/vulkan.h>
#include "state.h"

void geyser_init_vk(RenderState *state);
void geyser_destroy_vk(RenderState *state);

#endif
