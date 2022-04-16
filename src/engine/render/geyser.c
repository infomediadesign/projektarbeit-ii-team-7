#include "geyser.h"
#include <string.h>

static GLADapiproc glad_vulkan_load_func(void* user, const char* name) {
  return glfwGetInstanceProcAddress((VkInstance)user, name);
}

void geyser_allocate(RenderState *state) {
  state->instance = (VkInstance *)malloc(sizeof(VkInstance));
  state->surface = (VkSurfaceKHR *)malloc(sizeof(VkSurfaceKHR));
}

const char *get_window_surface_extension() {
#ifdef _WIN32
  return "VK_KHR_win32_surface";
#else
  if (strcmp(getenv("XDG_SESSION_TYPE"), "wayland") == 0)
    return "VK_KHR_wayland_surface";
  else
    return "VK_KHR_xlib_surface";
#endif
}

void geyser_init_vk(RenderState *state) {
  /* Load functions necessary for instance creation */
  gladLoadVulkanUserPtr(NULL, glad_vulkan_load_func, NULL);
  geyser_allocate(state);

  const char *ext_names[] = { "VK_KHR_surface", get_window_surface_extension() };

  VkInstanceCreateInfo instance_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    NULL,
    0,
    NULL,
    0,
    NULL,
    2,
    ext_names
  };

  vkCreateInstance(&instance_info, NULL, state->instance);

  /* Now that we have an instance, load all other functions */
  gladLoadVulkanUserPtr(NULL, glad_vulkan_load_func, state->instance);

  if (glfwCreateWindowSurface(*state->instance, state->window, NULL, state->surface)) {
    printf("[Geyser Error] Window surface initialization failed!\n");
  }
}

void geyser_destroy_vk(RenderState *state) {
  vkDestroySurfaceKHR(*state->instance, *state->surface, NULL);
}
