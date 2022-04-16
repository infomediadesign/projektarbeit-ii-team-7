#include "geyser.h"
#include <string.h>

static GLADapiproc glad_vulkan_load_func(void *user, const char* name) {
  return glfwGetInstanceProcAddress((VkInstance)user, name);
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

  const char *ext_names[] = { "VK_KHR_surface", get_window_surface_extension() };
  const char *validation_layer_names[] = { "VK_LAYER_KHRONOS_validation" };

  u32 layer_count = 0;
  vkEnumerateInstanceLayerProperties(&layer_count, NULL);

  if (layer_count > 0) {
    VkLayerProperties layer_properties[layer_count];
    vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

    for (u32 i = 0; i < layer_count; i++) {
      printf("%s\n", layer_properties[i].layerName);
    }
  } else {
    printf("Validation layers unavailable\n");
  }

  VkApplicationInfo app_info = {
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pNext = NULL,
    .pApplicationName = "Geyser",
    .applicationVersion = 1,
    .pEngineName = "Miniflow",
    .engineVersion = 1,
    .apiVersion = VK_API_VERSION_1_1 /* 1.1.0 */
  };

  VkInstanceCreateInfo instance_info = {
    VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    NULL,
    0,
    &app_info,
    1,
    validation_layer_names,
    2,
    ext_names
  };

  VkResult res = vkCreateInstance(&instance_info, NULL, &state->instance);

  if (res != VK_SUCCESS) {
    printf("[Geyser Error] Failed to create vulkan instance ");

    switch (res) {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      printf("(host out of memory)\n");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      printf("(device out of memory)\n");
      break;
    case VK_ERROR_INITIALIZATION_FAILED:
      printf("(initialization failed)\n");
      break;
    case VK_ERROR_LAYER_NOT_PRESENT:
      printf("(layer not present)\n");
      break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      printf("(extension not present)\n");
      break;
    case VK_ERROR_INCOMPATIBLE_DRIVER:
      printf("(driver is incompatible)\n");
      break;
    default:
      printf("\n");
    }

    return;
  }

  /* Now that we have an instance, load all other functions */
  gladLoadVulkanUserPtr(NULL, glad_vulkan_load_func, state->instance);

  u32 device_count = 0;
  vkEnumeratePhysicalDevices(state->instance, &device_count, NULL);

  if (device_count <= 0) {
    printf("[Geyser Error] No physical graphics devices detected!\n");
    return;
  }

  VkPhysicalDevice physical_devices[device_count];
  vkEnumeratePhysicalDevices(state->instance, &device_count, physical_devices);

  /* Simply pick the first discrete GPU as our "ideal" GPU. */
  for (u32 i = 0; i < device_count; i++) {
    VkPhysicalDeviceProperties device_properties;
    vkGetPhysicalDeviceProperties(physical_devices[i], &device_properties);

    state->physical_device = physical_devices[i];

    if (device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      printf("[Geyser] Using a discrete GPU\n");
      printf(
        "API Version: %u\nDriver Version: %u\nVendor ID: %u\nDevice: %s (ID: %u)\n",
        device_properties.apiVersion,
        device_properties.driverVersion,
        device_properties.vendorID,
        device_properties.deviceName,
        device_properties.deviceID
      );
      break;
    }
  }

  u32 queue_properties_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(state->physical_device, &queue_properties_count, NULL);

  if (queue_properties_count == 0) {
    printf("[Geyser Error] Cannot fetch physical device queue properties\n");
    return;
  }

  VkQueueFamilyProperties queue_properties[queue_properties_count];
  vkGetPhysicalDeviceQueueFamilyProperties(state->physical_device, &queue_properties_count, queue_properties);

  u32 family_index = 0;

  for (family_index; family_index < queue_properties_count; family_index++) {
    if (queue_properties[family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      break;
  }

  float queue_prios[] = {1.0f, 1.0f};

  VkDeviceQueueCreateInfo queue_create_info = {
    .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0,
    .queueFamilyIndex = family_index,
    .queueCount = 1,
    .pQueuePriorities = queue_prios
  };

  const char *device_ext_names[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

  VkDeviceCreateInfo device_create_info = {
    VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
    NULL,
    0,
    1,
    &queue_create_info,
    0,
    NULL,
    1,
    device_ext_names,
    NULL
  };

  res = vkCreateDevice(state->physical_device, &device_create_info, NULL, &state->device);

  if (res != VK_SUCCESS) {
    printf("[Geyser Error] Failed to create device! ");

    switch (res) {
    case VK_ERROR_OUT_OF_HOST_MEMORY:
      printf("(Host out of memory)\n");
      break;
    case VK_ERROR_OUT_OF_DEVICE_MEMORY:
      printf("(Out of device memory)\n");
      break;
    case VK_ERROR_INITIALIZATION_FAILED:
      printf("(Initialization failed)\n");
      break;
    case VK_ERROR_EXTENSION_NOT_PRESENT:
      printf("(Extension not found)\n");
      break;
    case VK_ERROR_FEATURE_NOT_PRESENT:
      printf("(Feature not found)\n");
      break;
    case VK_ERROR_TOO_MANY_OBJECTS:
      printf("(Too many objects)\n");
      break;
    case VK_ERROR_DEVICE_LOST:
      printf("(Device lost)\n");
      break;
    default:
      printf("\n");
    }

    return;
  }

  if (glfwCreateWindowSurface(state->instance, state->window, NULL, &state->surface) != VK_SUCCESS) {
    printf("[Geyser Error] Window surface initialization failed!\n");
    return;
  }

  VkSurfaceCapabilitiesKHR surface_capabilities;
  res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(state->physical_device, state->surface, &surface_capabilities);

  if (res != VK_SUCCESS) {
    printf("[Geyser] Failed to get physical device surface capabilities!\n");
    return;
  } else {
    printf(
      "[Geyser] min %ux%u, max %ux%u, min image count is %u, max image count is %u\n",
      surface_capabilities.minImageExtent.width,
      surface_capabilities.minImageExtent.height,
      surface_capabilities.maxImageExtent.width,
      surface_capabilities.maxImageExtent.height,
      surface_capabilities.minImageCount,
      surface_capabilities.maxImageCount
    );
  }

  u32 format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(state->physical_device, state->surface, &format_count, NULL);

  if (format_count <= 0) {
    printf("[Geyser Error] Physical device surface does not have any formats!\n");
    return;
  }

  VkSurfaceFormatKHR surface_formats[format_count];
  vkGetPhysicalDeviceSurfaceFormatsKHR(state->physical_device, state->surface, &format_count, surface_formats);

  VkFormat preferred_format = VK_FORMAT_UNDEFINED;
  VkColorSpaceKHR preferred_color_space = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

  if (format_count > 1 || surface_formats[0].format != VK_FORMAT_UNDEFINED) {
    for (u32 i = 0; i < format_count; i++) {
      if (surface_formats[0].format == VK_FORMAT_B8G8R8A8_UNORM) {
        preferred_format = surface_formats[0].format;
        preferred_color_space = surface_formats[0].colorSpace;
        break;
      }
    }
  }

  if (preferred_format == VK_FORMAT_UNDEFINED) {
    preferred_format = surface_formats[0].format;
    preferred_color_space = surface_formats[0].colorSpace;
  }

  VkExtent2D ext = {.width = 640, .height = 480};
  const u32 family_indices[] = {family_index};

  VkSwapchainCreateInfoKHR swapchain_create_info = {
    .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
    .pNext = NULL,
    .flags = 0,
    .surface = state->surface,
    .minImageCount = surface_capabilities.minImageCount + 1,
    .imageFormat = preferred_format,
    .imageColorSpace = preferred_color_space,
    .imageExtent = ext,
    .imageArrayLayers = 1,
    .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
    .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = 1,
    .pQueueFamilyIndices = family_indices,
    .preTransform = surface_capabilities.currentTransform,
    .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
    .presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR,
    .clipped = VK_TRUE,
    .oldSwapchain = VK_NULL_HANDLE
  };

  if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (surface_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  /* segfault */
  if (vkCreateSwapchainKHR(state->device, &swapchain_create_info, NULL, &state->swapchain) != VK_SUCCESS) {
    printf("[Geyser Error] Failed to create swapchain!\n");
    return;
  }

  printf("Created swapchain\n");
}

void geyser_destroy_vk(RenderState *state) {
  vkDestroySurfaceKHR(state->instance, state->surface, NULL);
}
