#include "geyser.h"
#include <string.h>

#define GEYSER_MINIMAL_VK_STRUCT_INFO(t) .sType = t, .pNext = NULL

#define GEYSER_BASIC_VK_STRUCT_INFO(t) .sType = t, .pNext = NULL, .flags = 0

/* This pretty much remains the same all the time */
const VkSemaphoreCreateInfo semaphore_create_info = {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = NULL,
    .flags = 0};

static GLADapiproc glad_vulkan_load_func(void *user, const char *name) {
  return glfwGetInstanceProcAddress((VkInstance)user, name);
}

static GLADapiproc glad_vulkan_load_func_vk(void *user, const char *name) {
  return vkGetInstanceProcAddr((VkInstance)user, name);
}

VkBool32 debug_callback(VkDebugReportFlagsEXT _flags,
                        VkDebugReportObjectTypeEXT _object_type, u64 _object,
                        size_t _location, i32 _message_code,
                        const char *_layer_prefix, const char *message,
                        void *_userdata) {
  // printf("\033[1;32m[Vulkan Debug]\033[0m %s\n", message);
  return VK_FALSE;
}

void geyser_success_or_message(const VkResult res, const char *message) {
  if (res != VK_SUCCESS) {
    printf("\033[1;31m[Geyser Error]\033[0m %s\n", message);
    abort();
  }
}

void geyser_init_vk(RenderState *restrict state) {
  u32 ext_count = 0;
  u32 validation_layer_count = 0;

  /* Load functions necessary for instance creation */
  gladLoadVulkanUserPtr(NULL, glad_vulkan_load_func, NULL);

  const char **extensions = glfwGetRequiredInstanceExtensions(&ext_count);
  const char *ext_names[16];
  const char *validation_layer_names[1];

  for (u8 i = 0; i < ext_count; i++)
    ext_names[i] = extensions[i];

  if (state->debug == 1) {
    u32 layer_count = 0;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    if (layer_count > 0) {
      VkLayerProperties *layer_properties =
          malloc(sizeof(VkLayerProperties) * layer_count);
      vkEnumerateInstanceLayerProperties(&layer_count, layer_properties);

      for (u32 i = 0; i < layer_count; i++) {
        if (strcmp(layer_properties[i].layerName,
                   "VK_LAYER_KHRONOS_validation") == 0) {
          validation_layer_names[0] = "VK_LAYER_KHRONOS_validation";
          validation_layer_count = 1;
          break;
        }
      }

      free(layer_properties);
    } else {
      printf("Validation layers unavailable\n");
      validation_layer_count = 0;
    }

    ext_names[ext_count] = "VK_EXT_debug_report";
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
        GEYSER_MINIMAL_VK_STRUCT_INFO(
            VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT),
        .flags = 0xf, .pfnCallback = debug_callback, .pUserData = NULL};

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

  VkPhysicalDevice *physical_devices =
      malloc(sizeof(VkPhysicalDevice) * device_count);
  vkEnumeratePhysicalDevices(state->instance, &device_count, physical_devices);

  /* Simply pick the first discrete GPU as our "ideal" GPU. */
  for (u32 i = 0; i < device_count; i++) {
    vkGetPhysicalDeviceProperties(physical_devices[i],
                                  &state->physical_device_properties);

    state->physical_device = physical_devices[i];

    if (state->physical_device_properties.deviceType ==
        VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
      if (state->debug == 1) {
        printf("[Geyser] Using a discrete GPU\n");
        printf("API Version: %u\nDriver Version: %u\nVendor ID: %u\nDevice: %s "
               "(ID: %u)\n",
               state->physical_device_properties.apiVersion,
               state->physical_device_properties.driverVersion,
               state->physical_device_properties.vendorID,
               state->physical_device_properties.deviceName,
               state->physical_device_properties.deviceID);
      }

      break;
    }
  }

  free(physical_devices);

  vkGetPhysicalDeviceFeatures(state->physical_device,
                              &state->physical_device_features);

  gladLoadVulkanUserPtr(state->physical_device, glad_vulkan_load_func_vk,
                        state->instance);

  u32 queue_properties_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(state->physical_device,
                                           &queue_properties_count, NULL);

  if (queue_properties_count == 0) {
    printf("[Geyser Error] Cannot fetch physical device queue properties\n");
    abort();
  }

  VkQueueFamilyProperties *queue_properties =
      malloc(sizeof(VkQueueFamilyProperties) * queue_properties_count);
  vkGetPhysicalDeviceQueueFamilyProperties(
      state->physical_device, &queue_properties_count, queue_properties);

  for (state->queue_family_index = 0;
       state->queue_family_index < queue_properties_count;
       state->queue_family_index++) {
    if (queue_properties[state->queue_family_index].queueFlags &
            VK_QUEUE_GRAPHICS_BIT &&
        glfwGetPhysicalDevicePresentationSupport(
            state->instance, state->physical_device, state->queue_family_index))
      break;
  }

  free(queue_properties);

  u32 family_indices[] = {state->queue_family_index};
  state->queue_family_indices = family_indices;
  state->queue_family_indices_count = 1;

  const float queue_prios[] = {1.0f, 1.0f};

  const VkDeviceQueueCreateInfo queue_create_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO),
      .queueFamilyIndex = state->queue_family_index, .queueCount = 1,
      .pQueuePriorities = queue_prios};

  const char *device_ext_names[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

  const VkDeviceCreateInfo device_create_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO),
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queue_create_info,
      .enabledLayerCount = validation_layer_count,
      .ppEnabledLayerNames = validation_layer_names,
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

  vkGetPhysicalDeviceMemoryProperties(
      state->physical_device, &state->physical_device_memory_properties);

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
                                state->physical_device,
                                state->queue_family_index, state->surface,
                                &device_surface_supported),
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
  VkPresentModeKHR *device_present_modes =
      malloc(sizeof(VkPresentModeKHR) * device_present_mode_count);
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

  free(device_present_modes);

  u32 format_count = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(state->physical_device, state->surface,
                                       &format_count, NULL);

  if (format_count < 1) {
    printf(
        "[Geyser Error] Physical device surface does not have any formats!\n");
    abort();
  }

  VkSurfaceFormatKHR *surface_formats =
      malloc(sizeof(VkSurfaceFormatKHR) * format_count);
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

  free(surface_formats);

  switch (state->preferred_color_format) {
  case VK_FORMAT_B8G8R8A8_UNORM:
  case VK_FORMAT_B8G8R8A8_SRGB:
    break;
  default:
    printf("[Geyser Error] Surface color format doesn't appear to be 8-bit "
           "BGRA!\n");
    abort();
  }

  const VkExtent2D ext = {.width = state->window_width,
                          .height = state->window_height};

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

  VkImage *swapchain_images = malloc(sizeof(VkImage) * swapchain_image_count);
  vkGetSwapchainImagesKHR(state->device, state->swapchain,
                          &swapchain_image_count, swapchain_images);
  state->swapchain_images = swapchain_images;

  geyser_create_semaphore(state, &state->image_semaphore);

  vkAcquireNextImageKHR(state->device, state->swapchain, UINT64_MAX,
                        state->image_semaphore, NULL,
                        &state->current_swapchain_image);

  vkDestroySemaphore(state->device, state->image_semaphore, NULL);

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
      .attachmentCount = 1, .pAttachments = attachment_description,
      .subpassCount = 1, .pSubpasses = subpass_description};

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
      .usage =
          VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
      .size = util_mebibytes(256),
      .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = state->queue_family_indices_count,
      .pQueueFamilyIndices = state->queue_family_indices};

  vkCreateBuffer(state->device, &general_buffer_info, NULL, &state->buffer);

  const VkMemoryAllocateInfo general_memory_allocation_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO),
      .allocationSize = util_mebibytes(256),
      .memoryTypeIndex = geyser_get_memory_type_index(
          state, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)};

  vkAllocateMemory(state->device, &general_memory_allocation_info, NULL,
                   &state->memory);
  vkBindBufferMemory(state->device, state->buffer, state->memory, 0);

  VkCommandPoolCreateInfo command_pool_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO),
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = state->queue_family_index};

  vkCreateCommandPool(state->device, &command_pool_info, NULL,
                      &state->command_pool);

  VkCommandBufferAllocateInfo command_buffer_info = {
      GEYSER_MINIMAL_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO),
      .commandPool = state->command_pool,
      .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY, .commandBufferCount = 1};

  vkAllocateCommandBuffers(state->device, &command_buffer_info,
                           &state->command_buffer);

  VkImageMemoryBarrier pre_draw_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = NULL,
      .srcAccessMask = 0,
      .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
      .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

  VkImageMemoryBarrier pre_present_barrier = {
      .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
      .pNext = NULL,
      .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
      .dstAccessMask = VK_ACCESS_MEMORY_READ_BIT,
      .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
      .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
      .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
      .subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}};

  state->pre_draw_barrier = pre_draw_barrier;
  state->pre_present_barrier = pre_present_barrier;
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
  for (u32 i = 0; i < state->physical_device_memory_properties.memoryTypeCount;
       i++) {
    if (state->physical_device_memory_properties.memoryTypes[i].propertyFlags &
        flag) {
      return i;
    }
  }

  printf("[Geyser Warning] Memory type index %i does not exist!\n", flag);

  return 0;
}

GeyserImage *geyser_create_image(const RenderState *restrict state,
                                 const Vector2 size) {
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

  geyser_success_or_message(
      vkCreateImage(state->device, &image_creation_info, NULL, &gi->image),
      "Failed to create image!");

  return gi;
}

void geyser_allocate_image_memory(const RenderState *restrict state,
                                  GeyserImage *image) {
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

  geyser_success_or_message(
      vkBindImageMemory(state->device, image->image, image->memory, 0),
      "Failed to bind image memory!");
}

GeyserImage *geyser_create_and_allocate_image(const RenderState *restrict state,
                                              const Vector2 size) {
  GeyserImage *image = geyser_create_image(state, size);

  geyser_allocate_image_memory(state, image);

  return image;
}

GeyserPipeline *geyser_create_pipeline(
    const RenderState *restrict state,
    const VkDescriptorSetLayoutBinding descriptor_bindings[],
    const u32 descriptor_bindings_size,
    const VkPushConstantRange push_constant_ranges[],
    const u32 push_constant_ranges_size, const u8 vertex_shader_data[],
    const u32 vertex_shader_data_size, const u8 fragment_shader_data[],
    const u32 fragment_shader_data_size,
    GeyserVertexInputDescription *vertex_input_description) {
  GeyserPipeline *pipe = (GeyserPipeline *)malloc(sizeof(GeyserPipeline));

  const VkDescriptorSetLayoutCreateInfo descriptor_layout_create_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO),
      .bindingCount = descriptor_bindings_size,
      .pBindings = descriptor_bindings};

  vkCreateDescriptorSetLayout(state->device, &descriptor_layout_create_info,
                              NULL, &pipe->descriptor_set_layout);

  const VkDescriptorSetLayout layouts[] = {pipe->descriptor_set_layout};

  const VkPipelineLayoutCreateInfo pipeline_layout_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO),
      .setLayoutCount = 1, .pSetLayouts = layouts,
      .pushConstantRangeCount = push_constant_ranges_size,
      .pPushConstantRanges = push_constant_ranges};

  vkCreatePipelineLayout(state->device, &pipeline_layout_info, NULL,
                         &pipe->pipeline_layout);

  const VkShaderModuleCreateInfo vertex_shader_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO),
      .codeSize = vertex_shader_data_size, .pCode = (u32 *)vertex_shader_data};

  const VkShaderModuleCreateInfo fragment_shader_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO),
      .codeSize = fragment_shader_data_size,
      .pCode = (u32 *)fragment_shader_data};

  vkCreateShaderModule(state->device, &vertex_shader_info, NULL,
                       &pipe->vertex_shader);
  vkCreateShaderModule(state->device, &fragment_shader_info, NULL,
                       &pipe->fragment_shader);

  const VkPipelineShaderStageCreateInfo pipeline_shader_stages[] = {
      {GEYSER_BASIC_VK_STRUCT_INFO(
           VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO),
       .stage = VK_SHADER_STAGE_VERTEX_BIT, .module = pipe->vertex_shader,
       .pName = "main", .pSpecializationInfo = NULL},
      {GEYSER_BASIC_VK_STRUCT_INFO(
           VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO),
       .stage = VK_SHADER_STAGE_FRAGMENT_BIT, .module = pipe->fragment_shader,
       .pName = "main", .pSpecializationInfo = NULL},
  };

  const VkPipelineVertexInputStateCreateInfo vertex_input_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO),
      .vertexBindingDescriptionCount =
          vertex_input_description->input_binding_description_size,
      .pVertexBindingDescriptions =
          vertex_input_description->input_binding_descriptions,
      .vertexAttributeDescriptionCount =
          vertex_input_description->input_attribute_description_size,
      .pVertexAttributeDescriptions =
          vertex_input_description->input_attribute_descriptions};

  const VkPipelineInputAssemblyStateCreateInfo input_assembly_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO),
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
      .primitiveRestartEnable = VK_FALSE};

  const VkPipelineRasterizationStateCreateInfo raster_state_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO),
      .depthClampEnable = VK_FALSE,
      .rasterizerDiscardEnable = VK_FALSE,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .cullMode = VK_CULL_MODE_BACK_BIT,
      .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
      .depthBiasEnable = VK_FALSE,
      .depthBiasConstantFactor = 0.0f,
      .depthBiasClamp = 0.0f,
      .depthBiasSlopeFactor = 0.0f,
      .lineWidth = 1.0f};

  const VkPipelineMultisampleStateCreateInfo multisample_state_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO),
      .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
      .sampleShadingEnable = VK_FALSE,
      .minSampleShading = 0.0f,
      .pSampleMask = NULL,
      .alphaToCoverageEnable = VK_TRUE,
      .alphaToOneEnable = VK_FALSE};

  const VkPipelineDepthStencilStateCreateInfo stencil_state_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO),
      .depthTestEnable = VK_TRUE,
      .depthWriteEnable = VK_TRUE,
      .depthCompareOp = VK_COMPARE_OP_LESS,
      .depthBoundsTestEnable = VK_FALSE,
      .stencilTestEnable = VK_FALSE};

  const VkPipelineViewportStateCreateInfo viewport_state_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO),
      .viewportCount = 1, .pViewports = NULL, .scissorCount = 1,
      .pScissors = NULL};

  VkPipelineColorBlendAttachmentState color_attachment_states[] = {
      {.blendEnable = VK_TRUE,
       .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_COLOR,
       .dstColorBlendFactor = VK_BLEND_FACTOR_DST_COLOR,
       .colorBlendOp = VK_BLEND_OP_ADD,
       .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
       .dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
       .alphaBlendOp = VK_BLEND_OP_ADD,
       .colorWriteMask = 0xF}};

  const VkPipelineColorBlendStateCreateInfo color_blend_state_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO),
      .logicOpEnable = VK_FALSE,
      .logicOp = VK_LOGIC_OP_AND,
      .attachmentCount = 1,
      .pAttachments = color_attachment_states,
      .blendConstants = {1.0f, 1.0f, 1.0f, 1.0f}};

  const VkDynamicState dynamic_states[] = {VK_DYNAMIC_STATE_VIEWPORT,
                                           VK_DYNAMIC_STATE_SCISSOR};

  const VkPipelineDynamicStateCreateInfo dynamic_states_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(
          VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO),
      .dynamicStateCount = 2, .pDynamicStates = dynamic_states};

  const VkGraphicsPipelineCreateInfo pipeline_info[] = {
      {GEYSER_BASIC_VK_STRUCT_INFO(
           VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO),
       .layout = pipe->pipeline_layout, .renderPass = state->renderpass,
       .stageCount = 2, .pStages = pipeline_shader_stages,
       .pVertexInputState = &vertex_input_info,
       .pInputAssemblyState = &input_assembly_info, .pTessellationState = NULL,
       .pViewportState = &viewport_state_info,
       .pRasterizationState = &raster_state_info,
       .pMultisampleState = &multisample_state_info,
       .pDepthStencilState = &stencil_state_info,
       .pColorBlendState = &color_blend_state_info,
       .pDynamicState = &dynamic_states_info, .subpass = 0,
       .basePipelineHandle = NULL, .basePipelineIndex = 0}};

  VkPipeline pipelines[] = {pipe->pipeline};

  vkCreateGraphicsPipelines(state->device, NULL, 1, pipeline_info, NULL,
                            pipelines);

  return pipe;
}

GeyserVertexInputDescription geyser_create_vertex_input_description() {
  GeyserVertexInputDescription description;

  description.input_binding_description_size = 0U;
  description.input_attribute_description_size = 0U;

  return description;
}

void geyser_add_vertex_input_binding(GeyserVertexInputDescription *description,
                                     const u32 binding, const u32 stride,
                                     const VkVertexInputRate input_rate) {
  const VkVertexInputBindingDescription binding_description = {binding, stride,
                                                               input_rate};
  description->input_binding_descriptions
      [description->input_binding_description_size++] = binding_description;
};

void geyser_add_vertex_input_attribute(
    GeyserVertexInputDescription *description, const u32 location,
    const u32 binding, VkFormat format, const u32 offset) {
  const VkVertexInputAttributeDescription attribute_description = {
      location, binding, format, offset};
  description->input_attribute_descriptions
      [description->input_attribute_description_size++] = attribute_description;
}

void geyser_create_semaphore(const RenderState *restrict state,
                             VkSemaphore *semaphore) {
  geyser_success_or_message(
      vkCreateSemaphore(state->device, &semaphore_create_info, NULL, semaphore),
      "Failed to create a semaphore!");
}

void geyser_cmd_begin_draw(RenderState *restrict state) {
  geyser_create_semaphore(state, &state->image_semaphore);
  geyser_create_semaphore(state, &state->draw_semaphore);

  const VkResult res = vkAcquireNextImageKHR(
      state->device, state->swapchain, UINT64_MAX, state->image_semaphore, NULL,
      &state->current_swapchain_image);

  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    if (state->debug)
      printf("[Geyser Debug] Target out of date, rebuilding...\n");

    return;
  } else if (res != VK_SUBOPTIMAL_KHR && res != 0) {
    printf("[Geyser Error] Failed to acquire new swapchain image!\n");
    abort();
  }

  const VkCommandBufferBeginInfo cmd_begin_info = {
      GEYSER_BASIC_VK_STRUCT_INFO(VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO),
      .pInheritanceInfo = NULL};

  vkBeginCommandBuffer(state->command_buffer, &cmd_begin_info);

  state->pre_draw_barrier.image =
      state->swapchain_images[state->current_swapchain_image];

  vkCmdPipelineBarrier(state->command_buffer,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
                       NULL, 1, &state->pre_draw_barrier);
}

void geyser_cmd_end_draw(RenderState *restrict state) {
  state->pre_present_barrier.image =
      state->swapchain_images[state->current_swapchain_image];

  vkCmdPipelineBarrier(state->command_buffer,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, 0, NULL, 0,
                       NULL, 1, &state->pre_present_barrier);

  vkEndCommandBuffer(state->command_buffer);

  const VkFence no_fence = VK_NULL_HANDLE;
  const VkPipelineStageFlags pipe_stage_flags =
      VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
  const VkSubmitInfo submit_info = {.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                    .pNext = NULL,
                                    .waitSemaphoreCount = 1,
                                    .pWaitSemaphores = &state->image_semaphore,
                                    .pWaitDstStageMask = &pipe_stage_flags,
                                    .commandBufferCount = 1,
                                    .pCommandBuffers = &state->command_buffer,
                                    .signalSemaphoreCount = 1,
                                    .pSignalSemaphores =
                                        &state->draw_semaphore};

  geyser_success_or_message(
      vkQueueSubmit(state->queue, 1, &submit_info, no_fence),
      "Failed to submit to queue!");

  const VkPresentInfoKHR present_info = {
      .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
      .pNext = NULL,
      .waitSemaphoreCount = 1,
      .pWaitSemaphores = &state->draw_semaphore,
      .swapchainCount = 1,
      .pSwapchains = &state->swapchain,
      .pImageIndices = &state->current_swapchain_image,
  };

  const VkResult res = vkQueuePresentKHR(state->queue, &present_info);

  if (res == VK_ERROR_OUT_OF_DATE_KHR) {
    if (state->debug)
      printf("[Geyser Debug] Target out of date, rebuilding...\n");
  } else if (res != VK_SUBOPTIMAL_KHR && res != 0) {
    printf("[Geyser Error] Failed to present queue!\n");
    abort();
  }

  geyser_success_or_message(vkQueueWaitIdle(state->queue),
                            "Failed to wait for queue idle state!");

  vkDestroySemaphore(state->device, state->image_semaphore, NULL);
  vkDestroySemaphore(state->device, state->draw_semaphore, NULL);
}

void geyser_cmd_begin_renderpass(const RenderState *restrict state) {
  const VkClearValue clear_values[2] = {
      [0] = {.color.float32 = {0.0f, 0.0f, 0.0f, 1.0f}},
      [1] = {.depthStencil = {1.0f, 0}},
  };

  const VkRenderPassBeginInfo render_pass_begin_info = {
      .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
      .pNext = NULL,
      .renderPass = state->renderpass,
      .framebuffer = state->framebuffer,
      .renderArea.offset.x = 0,
      .renderArea.offset.y = 0,
      .renderArea.extent.width = state->window_width,
      .renderArea.extent.height = state->window_height,
      .clearValueCount = 2,
      .pClearValues = clear_values,
  };

  vkCmdBeginRenderPass(state->command_buffer, &render_pass_begin_info,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void geyser_cmd_end_renderpass(const RenderState *restrict state) {
  vkCmdEndRenderPass(state->command_buffer);
}
