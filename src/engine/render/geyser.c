#include "geyser.h"
#include <string.h>

#define GEYSER_MINIMAL_VK_STRUCT_INFO(t) .sType = t,\
.pNext = NULL

#define GEYSER_BASIC_VK_STRUCT_INFO(t) .sType = t,\
.pNext = NULL,\
.flags = 0

static GLADapiproc glad_vulkan_load_func(void *user, const char *name) {
  return glfwGetInstanceProcAddress((VkInstance)user, name);
}

static GLADapiproc glad_vulkan_load_func_vk(void *user, const char *name) {
  return vkGetInstanceProcAddr((VkInstance)user, name);
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

VkBool32 debug_callback(VkDebugReportFlagsEXT _flags,
                        VkDebugReportObjectTypeEXT _object_type, u64 _object,
                        size_t _location, i32 _message_code,
                        const char *_layer_prefix, const char *message,
                        void *_userdata) {
  printf("\033[1;32m[Vulkan Debug]\033[0m %s\n", message);
  return VK_FALSE;
}

void geyser_success_or_message(const VkResult res, const char *message) {
  if (res != VK_SUCCESS) {
    printf("\033[1;31m[Geyser Error]\033[0m %s\n", message);
    abort();
  }
}

void geyser_init_vk(RenderState *restrict state) {
  u32 ext_count = 2;
  u32 validation_layer_count = 0;

  /* Load functions necessary for instance creation */
  gladLoadVulkanUserPtr(NULL, glad_vulkan_load_func, NULL);

  const char *ext_names[3] = {"VK_KHR_surface", get_window_surface_extension()};
  const char *validation_layer_names[1] = {};

  if (state->debug == 1) {
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    if (layer_count > 0) {
      VkLayerProperties layer_properties[layer_count];
      vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

      for (u32 i = 0; i < layer_count; i++) {
        if (strcmp(layer_properties[i].layerName,
                   "VK_LAYER_KHRONOS_validation") == 0) {
          validation_layer_names[0] = "VK_LAYER_KHRONOS_validation";
          validation_layer_count = 1;
          break;
        }
      }
    } else {
      printf("Validation layers unavailable\n");
      validation_layer_count = 0;
    }

    ext_names[2] = "VK_EXT_debug_report";
    ext_count = 3;
  }

  const VkApplicationInfo app_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_APPLICATION_INFO),
      .pApplicationName = "Geyser",
      .applicationVersion = 1,
      .pEngineName = "Miniflow",
      .engineVersion = 1,
      .apiVersion = VK_API_VERSION_1_1 /* 1.1.0 */
  };

  const VkInstanceCreateInfo instance_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO),
      .pApplicationInfo = &app_info,
      .enabledLayerCount = validation_layer_count,
      .ppEnabledLayerNames = validation_layer_names,
      .enabledExtensionCount = ext_count,
      .ppEnabledExtensionNames = ext_names};

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

    abort();
  }

  /* Now that we have an instance, load all other functions */
  gladLoadVulkanUserPtr(NULL, glad_vulkan_load_func_vk, state->instance);

  if (state->debug == 1) {
    const VkDebugReportCallbackCreateInfoEXT debug_callback_info = {
        GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT),
        .flags = 0xf,
        .pfnCallback = debug_callback,
        .pUserData = NULL};

    VkDebugReportCallbackEXT debug_report_callback;

    geyser_success_or_message(
        vkCreateDebugReportCallbackEXT(state->instance, &debug_callback_info,
                                       NULL, &debug_report_callback),
        "Failed to create a debug callback!");
  }

  u32 device_count = 0;
  vkEnumeratePhysicalDevices(state->instance, &device_count, NULL);

  if (device_count <= 0) {
    printf("[Geyser Error] No physical graphics devices detected!\n");
    abort();
  }

  VkPhysicalDevice physical_devices[device_count];
  vkEnumeratePhysicalDevices(state->instance, &device_count, physical_devices);

  /* Simply pick the first discrete GPU as our "ideal" GPU. */
  for (u32 i = 0; i < device_count; i++) {
    vkGetPhysicalDeviceProperties(physical_devices[i], &state->physical_device_properties);

    state->physical_device = physical_devices[i];

    if (state->physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (state->debug == 1) {
        printf("[Geyser] Using a discrete GPU\n");
        printf("API Version: %u\nDriver Version: %u\nVendor ID: %u\nDevice: %s "
              "(ID: %u)\n",
              state->physical_device_properties.apiVersion, state->physical_device_properties.driverVersion,
              state->physical_device_properties.vendorID, state->physical_device_properties.deviceName,
              state->physical_device_properties.deviceID);
      }

      break;
    }
  }

  vkGetPhysicalDeviceFeatures(state->physical_device, &state->physical_device_features);

  gladLoadVulkanUserPtr(state->physical_device, glad_vulkan_load_func_vk,
                        state->instance);

  u32 queue_properties_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(state->physical_device,
                                           &queue_properties_count, NULL);

  if (queue_properties_count == 0) {
    printf("[Geyser Error] Cannot fetch physical device queue properties\n");
    abort();
  }

  VkQueueFamilyProperties queue_properties[queue_properties_count];
  vkGetPhysicalDeviceQueueFamilyProperties(
      state->physical_device, &queue_properties_count, queue_properties);

  for (state->queue_family_index = 0; state->queue_family_index < queue_properties_count; state->queue_family_index++) {
    if (queue_properties[state->queue_family_index].queueFlags & VK_QUEUE_GRAPHICS_BIT)
      break;
  }

  u32 family_indices[] = {state->queue_family_index};
  state->queue_family_indices = family_indices;
  state->queue_family_indices_count = 1;

  const float queue_prios[] = {1.0f, 1.0f};

  const VkDeviceQueueCreateInfo queue_create_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO),
      .queueFamilyIndex = state->queue_family_index,
      .queueCount = 1,
      .pQueuePriorities = queue_prios};

  const char *device_ext_names[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  const VkDeviceCreateInfo device_create_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queue_create_info,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = NULL,
      .enabledExtensionCount = 1,
      .ppEnabledExtensionNames = device_ext_names,
      .pEnabledFeatures = NULL};

  res = vkCreateDevice(state->physical_device, &device_create_info, NULL,
                       &state->device);

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

    abort();
  }

  vkGetPhysicalDeviceMemoryProperties(state->physical_device,
                                      &state->physical_device_memory_properties);

  geyser_success_or_message(glfwCreateWindowSurface(state->instance,
                                                    state->window, NULL,
                                                    &state->surface),
                            "Window surface initialization failed!");

  VkSurfaceCapabilitiesKHR surface_capabilities;

  if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
          state->physical_device, state->surface, &surface_capabilities) !=
      VK_SUCCESS) {
    printf(
        "[Geyser Error] Failed to get physical device surface capabilities!\n");
    abort();
  } else if (state->debug == 1) {
    printf("[Geyser] min %ux%u, max %ux%u, min image count is %u, max image "
           "count is %u\n",
           surface_capabilities.minImageExtent.width,
           surface_capabilities.minImageExtent.height,
           surface_capabilities.maxImageExtent.width,
           surface_capabilities.maxImageExtent.height,
           surface_capabilities.minImageCount,
           surface_capabilities.maxImageCount);
  }

  VkBool32 device_surface_supported = VK_FALSE;

  geyser_success_or_message(vkGetPhysicalDeviceSurfaceSupportKHR(
                                state->physical_device, state->queue_family_index,
                                state->surface, &device_surface_supported),
                            "Unable to determine device surface support!");

  if (device_surface_supported != VK_TRUE) {
    printf(
        "[Geyser Error] Device does not support the required surface type!\n");
    abort();
  }

  u32 device_present_mode_count = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      state->physical_device, state->surface, &device_present_mode_count, NULL);

  if (device_present_mode_count < 1) {
    printf("[Geyser Error] Device does not have any present modes!\n");
    abort();
  }

  VkPresentModeKHR preferred_present_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;
  VkPresentModeKHR device_present_modes[device_present_mode_count];
  vkGetPhysicalDeviceSurfacePresentModesKHR(
      state->physical_device, state->surface, &device_present_mode_count,
      device_present_modes);

  for (int i = 0; i < device_present_mode_count; i++) {
    if (device_present_modes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR) {
      preferred_present_mode = device_present_modes[i];
      break;
    }

    if (preferred_present_mode != VK_PRESENT_MODE_MAILBOX_KHR) {
      preferred_present_mode = device_present_modes[i];
    }
  }

  u32 format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(state->physical_device, state->surface,
                                       &format_count, NULL);

  if (format_count < 1) {
    printf(
        "[Geyser Error] Physical device surface does not have any formats!\n");
    abort();
  }

  VkSurfaceFormatKHR surface_formats[format_count];
  vkGetPhysicalDeviceSurfaceFormatsKHR(state->physical_device, state->surface,
                                       &format_count, surface_formats);

  state->preferred_color_format = surface_formats[0].format;
  VkColorSpaceKHR preferred_color_space = surface_formats[0].colorSpace;

  if (surface_formats[0].format != VK_FORMAT_UNDEFINED) {
    for (u32 i = 0; i < format_count; i++) {
      if (surface_formats[i].format == VK_FORMAT_B8G8R8A8_UNORM) {
        state->preferred_color_format = surface_formats[i].format;
        preferred_color_space = surface_formats[i].colorSpace;
        break;
      }
    }
  }

  switch (state->preferred_color_format) {
  case VK_FORMAT_B8G8R8A8_UNORM:
  case VK_FORMAT_B8G8R8A8_SRGB:
    break;
  default:
    printf("[Geyser Error] Surface color format doesn't appear to be 8-bit "
           "BGRA!\n");
    abort();
  }

  const VkExtent2D ext = {.width = state->window_width, .height = state->window_height};

  VkSwapchainCreateInfoKHR swapchain_create_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR),
      .surface = state->surface,
      .minImageCount = surface_capabilities.minImageCount + 1,
      .imageFormat = state->preferred_color_format,
      .imageColorSpace = preferred_color_space,
      .imageExtent = ext,
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = state->queue_family_indices_count,
      .pQueueFamilyIndices = state->queue_family_indices,
      .preTransform = surface_capabilities.currentTransform,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = preferred_present_mode,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE};

  if (surface_capabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
    swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  }

  if (surface_capabilities.supportedUsageFlags &
      VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
    swapchain_create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
  }

  geyser_success_or_message(vkCreateSwapchainKHR(state->device,
                                                 &swapchain_create_info, NULL,
                                                 &state->swapchain),
                            "Failed to create swapchain!");

  u32 swapchain_image_count = 0;
  vkGetSwapchainImagesKHR(state->device, state->swapchain,
                          &swapchain_image_count, NULL);

  if (swapchain_image_count < 1) {
    printf("[Geyser Error] Swapchain has no images!\n");
    abort();
  }

  VkImage swapchain_images[swapchain_image_count];
  vkGetSwapchainImagesKHR(state->device, state->swapchain,
                          &swapchain_image_count, swapchain_images);
  state->swapchain_images = swapchain_images;

  const VkSemaphoreCreateInfo semaphore_create_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO)
  };

  geyser_success_or_message(vkCreateSemaphore(state->device,
                                              &semaphore_create_info, NULL,
                                              &state->semaphore),
                            "Failed to create a semaphore!");

  vkAcquireNextImageKHR(state->device, state->swapchain, UINT64_MAX,
                        state->semaphore, NULL,
                        &state->current_swapchain_image);

  const VkImageCreateInfo backbuffer_image_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO),
      .imageType = VK_IMAGE_TYPE_2D,
      .format = state->preferred_color_format,
      .extent = {state->window_width, state->window_height, 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = state->queue_family_indices_count,
      .pQueueFamilyIndices = state->queue_family_indices,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

  geyser_success_or_message(vkCreateImage(state->device, &backbuffer_image_info,
                                          NULL, &state->backbuffer_image),
                            "Failed to create backbuffer image!");

  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(state->device, state->backbuffer_image,
                               &memory_requirements);

  const VkMemoryAllocateInfo backbuffer_allocation_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = geyser_get_memory_type_index(
          state, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};

  VkDeviceMemory backbuffer_memory;

  geyser_success_or_message(vkAllocateMemory(state->device,
                                             &backbuffer_allocation_info, NULL,
                                             &backbuffer_memory),
                            "Failed to allocate backbuffer image memory!");

  geyser_success_or_message(vkBindImageMemory(state->device,
                                              state->backbuffer_image,
                                              backbuffer_memory, 0),
                            "Failed to bind backbuffer image memory!");

  state->backbuffer = geyser_create_image_view(state, &state->backbuffer_image,
                                               VK_IMAGE_VIEW_TYPE_2D_ARRAY);

  vkGetDeviceQueue(state->device, state->queue_family_index, 0, &state->queue);

  const VkAttachmentDescription attachment_description[1] = {
      {.flags = 0,
       .format = state->preferred_color_format,
       .samples =
           VK_SAMPLE_COUNT_1_BIT, /* todo: query VkPhysicalDeviceLimits */
       .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
       .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
       .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
       .stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE,
       .initialLayout = VK_IMAGE_LAYOUT_GENERAL,
       .finalLayout = VK_IMAGE_LAYOUT_GENERAL}};

  const VkAttachmentReference attachment_ref[1] = {
      {.attachment = 0, .layout = VK_IMAGE_LAYOUT_GENERAL}};

  const VkSubpassDescription subpass_description[1] = {
      {.flags = 0,
       .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
       .inputAttachmentCount = 1,
       .pInputAttachments = attachment_ref,
       .colorAttachmentCount = 1,
       .pColorAttachments = attachment_ref,
       .pResolveAttachments = NULL,
       .pDepthStencilAttachment = NULL,
       .preserveAttachmentCount = 0,
       .pPreserveAttachments = NULL}};

  const VkRenderPassCreateInfo renderpass_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO),
      .attachmentCount = 1,
      .pAttachments = attachment_description,
      .subpassCount = 1,
      .pSubpasses = subpass_description};

  if (vkCreateRenderPass(state->device, &renderpass_info, NULL,
                         &state->renderpass) != VK_SUCCESS) {
    printf("[Geyser Error] Failed to create a render pass!\n");
    abort();
  }

  const VkImageView fb_attachments[] = {state->backbuffer};

  VkFramebufferCreateInfo framebuffer_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO),
      .renderPass = state->renderpass,
      .attachmentCount = 1,
      .pAttachments = fb_attachments,
      .width = state->window_width,
      .height = state->window_height,
      .layers = 1};

  if (vkCreateFramebuffer(state->device, &framebuffer_info, NULL,
                          &state->framebuffer) != VK_SUCCESS) {
    printf("[Geyser Error] Failed to create a frame buffer!\n");
    abort();
  }

  const VkBufferCreateInfo general_buffer_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO),
    .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    .size = util_mebibytes(256),
    .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
    .queueFamilyIndexCount = state->queue_family_indices_count,
    .pQueueFamilyIndices = state->queue_family_indices
  };

  vkCreateBuffer(state->device, &general_buffer_info, NULL, &state->buffer);

  const VkMemoryAllocateInfo general_memory_allocation_info = {
    GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
    .allocationSize = util_mebibytes(256),
    .memoryTypeIndex = geyser_get_memory_type_index(state, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
  };

  vkAllocateMemory(state->device, &general_memory_allocation_info, NULL, &state->memory);
  vkBindBufferMemory(state->device, state->buffer, state->memory, 0);

  VkCommandPoolCreateInfo command_pool_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO),
    .queueFamilyIndex = state->queue_family_index
  };

  vkCreateCommandPool(state->device, &command_pool_info, NULL, &state->command_pool);

  VkCommandBufferAllocateInfo command_buffer_info = {
    GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO),
    .commandPool = state->command_pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = 1
  };

  VkCommandBuffer command_buffers[] = {state->command_buffer};

  vkAllocateCommandBuffers(state->device, &command_buffer_info, command_buffers);
}

void geyser_destroy_vk(RenderState *state) {
  vkDestroySwapchainKHR(state->device, state->swapchain, NULL);
  vkDestroySurfaceKHR(state->instance, state->surface, NULL);
}

void geyser_fill_image_view_creation_structs(
    RenderState *state, VkImageSubresourceRange *resource_range,
    VkComponentMapping *mapping, VkImageViewCreateInfo *creation_info) {
  resource_range->aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  resource_range->baseMipLevel = 0;
  resource_range->levelCount = 1;
  resource_range->baseArrayLayer = 0;
  resource_range->layerCount = 1;

  mapping->r = VK_COMPONENT_SWIZZLE_R;
  mapping->g = VK_COMPONENT_SWIZZLE_G;
  mapping->b = VK_COMPONENT_SWIZZLE_B;
  mapping->a = VK_COMPONENT_SWIZZLE_A;

  creation_info->sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  creation_info->pNext = NULL;
  creation_info->flags = 0;
  creation_info->viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
  creation_info->format = state->preferred_color_format;
  creation_info->components = *mapping;
  creation_info->subresourceRange = *resource_range;
}

VkImageView geyser_create_image_view(RenderState *state, VkImage *image,
                                     VkImageViewType type) {
  VkImageView image_view;
  VkImageSubresourceRange resource_range;
  VkComponentMapping mapping;
  VkImageViewCreateInfo image_view_creation_info;

  geyser_fill_image_view_creation_structs(state, &resource_range, &mapping,
                                          &image_view_creation_info);

  image_view_creation_info.image = *image;
  image_view_creation_info.viewType = type;

  if (vkCreateImageView(state->device, &image_view_creation_info, NULL,
                        &image_view) != VK_SUCCESS) {
    printf("[Geyser Error] Unable to create image view!\n");
    abort();
  }

  return image_view;
}

u32 geyser_get_memory_type_index(const RenderState *restrict state,
                                 const VkMemoryPropertyFlagBits flag) {
  for (u32 i = 0; i < state->physical_device_memory_properties.memoryTypeCount; i++) {
    if (state->physical_device_memory_properties.memoryTypes[i].propertyFlags & flag) {
      return i;
    }
  }

  printf("[Geyser Warning] Memory type index %i does not exist!\n", flag);

  return 0;
}

GeyserImage *geyser_create_image(const RenderState *restrict state, const Vector2 size) {
  GeyserImage *gi = (GeyserImage *)malloc(sizeof(GeyserImage));

  const VkImageCreateInfo image_creation_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO),
      .imageType = VK_IMAGE_TYPE_2D,
      .format = state->preferred_color_format,
      .extent = {.width = size.x, .height = size.y, .depth = 1},
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = VK_SAMPLE_COUNT_1_BIT,
      .tiling = VK_IMAGE_TILING_OPTIMAL,
      .usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
               VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = state->queue_family_indices_count,
      .pQueueFamilyIndices = state->queue_family_indices,
      .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED};

  geyser_success_or_message(vkCreateImage(state->device, &image_creation_info,
                                          NULL, &gi->image),
                            "Failed to create image!");

  return gi;
}

void geyser_allocate_image_memory(const RenderState *restrict state, GeyserImage *image) {
  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(state->device, image->image,
                               &memory_requirements);

  const VkMemoryAllocateInfo image_allocation_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
      .allocationSize = memory_requirements.size,
      .memoryTypeIndex = geyser_get_memory_type_index(
          state, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)};

  geyser_success_or_message(vkAllocateMemory(state->device,
                                             &image_allocation_info, NULL,
                                             &image->memory),
                            "Failed to allocate image memory!");

  geyser_success_or_message(vkBindImageMemory(state->device,
                                              image->image,
                                              image->memory, 0),
                            "Failed to bind image memory!");
}

GeyserImage *geyser_create_and_allocate_image(const RenderState *restrict state, const Vector2 size) {
  GeyserImage *image = geyser_create_image(state, size);

  geyser_allocate_image_memory(state, image);

  return image;
}

GeyserPipeline *geyser_create_pipeline(
  const RenderState *restrict state,
  const VkDescriptorSetLayoutBinding descriptor_bindings[],
  const u32 descriptor_bindings_size,
  const VkPushConstantRange push_constant_ranges[],
  const u32 push_constant_ranges_size,
  const u8 vertex_shader_data[],
  const u32 vertex_shader_data_size,
  const u8 fragment_shader_data[],
  const u32 fragment_shader_data_size
) {
  GeyserPipeline *pipe = (GeyserPipeline *)malloc(sizeof(GeyserPipeline));

  const VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO),
    .bindingCount = descriptor_bindings_size,
    .pBindings = descriptor_bindings
  };

  vkCreateDescriptorSetLayout(state->device, &descriptor_layout_create_info, NULL, &pipe->descriptor_set_layout);

  const VkDescriptorSetLayout layouts[] = {pipe->descriptor_set_layout};

  const VkPipelineLayoutCreateInfo pipeline_layout_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO),
    .setLayoutCount = 1,
    .pSetLayouts = layouts,
    .pushConstantRangeCount = push_constant_ranges_size,
    .pPushConstantRanges = push_constant_ranges
  };

  vkCreatePipelineLayout(state->device, &pipeline_layout_info, NULL, &pipe->pipeline_layout);

  const VkShaderModuleCreateInfo vertex_shader_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO),
    .codeSize = vertex_shader_data_size,
    .pCode = (u32 *)vertex_shader_data
  };

  const VkShaderModuleCreateInfo fragment_shader_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO),
    .codeSize = fragment_shader_data_size,
    .pCode = (u32 *)fragment_shader_data
  };

  vkCreateShaderModule(state->device, &vertex_shader_info, NULL, &pipe->vertex_shader);
  vkCreateShaderModule(state->device, &fragment_shader_info, NULL, &pipe->fragment_shader);

  const VkPipelineShaderStageCreateInfo pipeline_shader_stages[] = {
    {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO),
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = pipe->vertex_shader,
      .pName = "main",
      .pSpecializationInfo = NULL
    },
    {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO),
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = pipe->fragment_shader,
      .pName = "main",
      .pSpecializationInfo = NULL
    },
  };

  const VkPipelineVertexInputStateCreateInfo vertex_input_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO),
    .vertexBindingDescriptionCount = 0,
    .pVertexBindingDescriptions = NULL,
    .vertexAttributeDescriptionCount = 0,
    .pVertexAttributeDescriptions = NULL
  };

  const VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO),
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    .primitiveRestartEnable = VK_FALSE
  };

  const VkPipelineRasterizationStateCreateInfo raster_state_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO),
    .depthClampEnable = VK_FALSE,
    .rasterizerDiscardEnable = VK_FALSE,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .cullMode = VK_CULL_MODE_BACK_BIT,
    .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
    .depthBiasEnable = VK_FALSE,
    .depthBiasConstantFactor = 1.0f,
    .depthBiasClamp = 1.0f,
    .depthBiasSlopeFactor = 1.0f,
    .lineWidth = 1.0f
  };

  const VkPipelineMultisampleStateCreateInfo multisample_state_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO),
    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
    .sampleShadingEnable = VK_FALSE,
    .minSampleShading = 0.0f,
    .pSampleMask = NULL,
    .alphaToCoverageEnable = VK_TRUE,
    .alphaToOneEnable = VK_FALSE
  };

  const VkPipelineDepthStencilStateCreateInfo stencil_state_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO),
    .depthTestEnable = VK_TRUE,
    .depthWriteEnable = VK_TRUE,
    .depthCompareOp = VK_COMPARE_OP_LESS,
    .depthBoundsTestEnable = VK_FALSE,
    .stencilTestEnable = VK_FALSE
  };

  const VkPipelineColorBlendStateCreateInfo color_blend_state_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO),
    .logicOpEnable = VK_TRUE,
    .logicOp = VK_LOGIC_OP_AND,
    .attachmentCount = 0,
    .pAttachments = NULL,
    .blendConstants = {1.0f, 1.0f, 1.0f, 1.0f}
  };

  const VkDynamicState dynamic_states[] = {
    VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
  };

  const VkPipelineDynamicStateCreateInfo dynamic_states_info = {
    GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO),
    .dynamicStateCount = 2,
    .pDynamicStates = dynamic_states
  };

  const VkGraphicsPipelineCreateInfo pipeline_info[] = {
    {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO),
      .layout = pipe->pipeline_layout,
      .renderPass = state->renderpass,
      .stageCount = 2,
      .pStages = pipeline_shader_stages,
      .pVertexInputState = &vertex_input_info,
      .pInputAssemblyState = &input_assembly_info,
      .pTessellationState = NULL,
      .pViewportState = NULL,
      .pRasterizationState = &raster_state_info,
      .pMultisampleState = &multisample_state_info,
      .pDepthStencilState = &stencil_state_info,
      .pColorBlendState = &color_blend_state_info,
      .pDynamicState = &dynamic_states_info,
      .subpass = 0,
      .basePipelineHandle = NULL,
      .basePipelineIndex = 0
    }
  };

  VkPipeline pipelines[] = {pipe->pipeline};

  vkCreateGraphicsPipelines(state->device, NULL, 1, pipeline_info, NULL, pipelines);

  return pipe;
}
