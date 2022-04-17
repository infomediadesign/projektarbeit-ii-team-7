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
#include "state.h"

/**
 * @brief Initializes Vulkan resources.
 * 
 * Populates most Vulkan-related fields in the
 * RenderState struct. It also creates the window surface
 * for rendering.
 * 
 * ```
 * geyser_init_vk(render_state);
 * ...
 * // do some rendering here
 * ...
 * geyser_destroy_vk(render_state);
 * ```
 * 
 * @warning Will crash the app if Vulkan fails to initialize.
 * @param state Rendering state.
 */
void geyser_init_vk(RenderState *state);

/**
 * @brief Frees Vulkan resources.
 * 
 * ```
 * // some rendering here
 * geyser_destroy_vk(render_state);
 * // you can't do vulkan rendering past this point
 * ```
 * 
 * @param state Rendering state
 */
void geyser_destroy_vk(RenderState *state);

#endif
