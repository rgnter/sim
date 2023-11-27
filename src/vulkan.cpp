//
// Created by maros on 25.11.2023.
//

#include "sim/vulkan.hpp"

namespace vulkan
{

void Renderer::surface(GLFWwindow* window)
{
  // Create surface
  VkSurfaceKHR directSurface;
  glfwCreateWindowSurface(*_instance, window, nullptr, &directSurface);
  _surface = vkr::SurfaceKHR(_instance, directSurface);

  _devExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

void Renderer::physicalDevice()
{
  const vkr::PhysicalDevices physicalDevices(_instance);
  _physicalDevice = physicalDevices.front();
  printf("Selected physical device: %s\n", _physicalDevice.getProperties().deviceName.data());
}

void Renderer::logicalDevice()
{
  float queuePriorities[] = {0.0f};

  const auto queueFamilyProperties
    = _physicalDevice.getQueueFamilyProperties();

  for (int32_t index = 0; const auto& queueFamily: queueFamilyProperties)
  {
    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
    {
      _queueFamilyHints.graphicsFamily = index;
    }
    if (_physicalDevice.getSurfaceSupportKHR(index, *_surface))
    {
      _queueFamilyHints.presentFamily = index;
    }
    index++;
  }

  if (!_queueFamilyHints.graphicsFamily
      || !_queueFamilyHints.presentFamily)
    throw std::runtime_error("No queue family supporting graphics and presentation found");

  std::vector<const char*> extensions;
  extensions.reserve(_devExtensions.size());
  for (const auto& ext: _devExtensions)
  {
    extensions.emplace_back(ext.data());
  }

  const vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
    .queueFamilyIndex = _queueFamilyHints.graphicsFamily.value(),
    .queueCount = 1,
    .pQueuePriorities = queuePriorities,
  };

  _device = vkr::Device(
    _physicalDevice,
    vk::DeviceCreateInfo {
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &deviceQueueCreateInfo,
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data()
    });
}

void Renderer::swapChain()
{
  const auto surfaceFormats
    = _physicalDevice.getSurfaceFormatsKHR(*_surface);

  if (surfaceFormats.empty())
    throw std::runtime_error("No surface formats supported.");

  _surfaceImageFormat = surfaceFormats.front().format == vk::Format::eUndefined
                              ? vk::Format::eB8G8R8A8Unorm
                              : surfaceFormats.front().format;

  _surfaceCapabilities = _physicalDevice.getSurfaceCapabilitiesKHR(*_surface);

  const vk::Extent2D swapChainExtent =
    _surfaceCapabilities.currentExtent;

  const vk::PresentModeKHR swapChainPresentMode
    = vk::PresentModeKHR::eFifo;

  const vk::SurfaceTransformFlagBitsKHR preTransform =
    _surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity
      ? vk::SurfaceTransformFlagBitsKHR::eIdentity : _surfaceCapabilities.currentTransform;

  const vk::CompositeAlphaFlagBitsKHR compositeAlpha =
    _surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
      ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied : _surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
          ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied : _surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit
              ? vk::CompositeAlphaFlagBitsKHR::eInherit : vk::CompositeAlphaFlagBitsKHR::eOpaque;

  _swapChain = vkr::SwapchainKHR(
    _device,
    vk::SwapchainCreateInfoKHR {
      .surface = *_surface,
      .minImageCount = _surfaceCapabilities.minImageCount,
      .imageFormat = _surfaceImageFormat,
      .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
      .imageExtent = swapChainExtent,
      .imageArrayLayers = 1,
      .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
      .imageSharingMode = vk::SharingMode::eExclusive,
      .preTransform = preTransform,
      .compositeAlpha = compositeAlpha,
      .presentMode = swapChainPresentMode,
      .clipped = true,
    });

  const auto swapChainImages = _swapChain.getImages();
  _swapChainImageViews.reserve(swapChainImages.size());

  for (const auto& image : swapChainImages)
  {
    _swapChainImageViews.emplace_back(
      _device,
      vk::ImageViewCreateInfo {
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = _surfaceImageFormat,
        .subresourceRange = {
          .aspectMask = vk::ImageAspectFlagBits::eColor,
          .baseMipLevel = 0,
          .levelCount = 1,
          .baseArrayLayer = 0,
          .layerCount = 1,
        },
      });
  }

}

void Renderer::depthBuffer()
{
  _depthImageFormat = vk::Format::eD16Unorm;
  const vk::FormatProperties formatProperties
    = _physicalDevice.getFormatProperties(_depthImageFormat);

  // Determine tiling available for depth buffer
  vk::ImageTiling imageTiling;
  if (formatProperties.linearTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment)
    imageTiling = vk::ImageTiling::eLinear;
  else if ( formatProperties.optimalTilingFeatures & vk::FormatFeatureFlagBits::eDepthStencilAttachment )
    imageTiling = vk::ImageTiling::eOptimal;
  else
    throw std::runtime_error("DepthStencilAttachment is not supported for D16Unorm depth format." );

  _depthImage = vkr::Image(
    _device,
    vk::ImageCreateInfo {
      .imageType = vk::ImageType::e2D,
      .format = _depthImageFormat,
      .extent = vk::Extent3D {
        .width = _surfaceCapabilities.currentExtent.width,
        .height = _surfaceCapabilities.currentExtent.height,
        .depth = 1
      },
      .mipLevels = 1,
      .arrayLayers = 1,
      .samples = vk::SampleCountFlagBits::e1,
      .tiling = imageTiling,
      .usage = vk::ImageUsageFlagBits::eDepthStencilAttachment
    });

  const vk::PhysicalDeviceMemoryProperties memoryProperties
    = _physicalDevice.getMemoryProperties();
  const vk::MemoryRequirements memoryRequirements
    = _depthImage.getMemoryRequirements();

  uint32_t memoryTypeBits = memoryRequirements.memoryTypeBits;
  auto memoryTypeIndex = uint32_t( ~0 );
  for ( uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++ )
  {
    const auto& memoryType = memoryProperties.memoryTypes[i];
    if ((memoryTypeBits & 1)
        && (memoryType.propertyFlags & vk::MemoryPropertyFlagBits::eDeviceLocal) == vk::MemoryPropertyFlagBits::eDeviceLocal)
    {
      memoryTypeIndex = i;
      break;
    }
    memoryTypeBits >>= 1;
  }
  assert(memoryTypeIndex != uint32_t( ~0 ) );

  _depthMemory = vkr::DeviceMemory (
    _device,
    vk::MemoryAllocateInfo {
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex,
    });

  _depthImageView = vkr::ImageView(
    _device,
    vk::ImageViewCreateInfo {
      .image = *_depthImage,
      .viewType = vk::ImageViewType::e2D,
      .format = _depthImageFormat,
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eDepth,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
      }
    });
}

void Renderer::uniformBuffer()
{
  const auto model = glm::mat4x4( 1.0f );
  const auto view = glm::lookAt(
    glm::vec3(-5.0f, 3.0f, -10.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, -1.0f, 0.0f));

  const auto projection = glm::perspective(
    glm::radians( 45.0f ),
    1.0f,
    0.1f,
    100.0f);

  const auto clip = glm::mat4x4(
    1.0f,  0.0f, 0.0f, 0.0f,
    0.0f, -1.0f, 0.0f, 0.0f,
    0.0f,  0.0f, 0.5f, 0.0f,
    0.0f,  0.0f, 0.5f, 1.0f);  // vulkan clip space has inverted y and half z !

  const auto mvpc = clip * projection * view * model;

  _uniformBuffer =  vkr::Buffer(
    _device,
    vk::BufferCreateInfo {
      .size = sizeof(mvpc),
      .usage = vk::BufferUsageFlagBits::eUniformBuffer
    });

  const auto memoryProperties = _physicalDevice.getMemoryProperties();
  const auto memoryRequirements = _uniformBuffer.getMemoryRequirements();
  uint32_t memoryTypeIndex = 0;

  // Find the right memory for the uniform buffer.
  for (; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
  {
    const auto memoryTypeBit = 1 << memoryTypeIndex;
    if (memoryRequirements.memoryTypeBits & memoryTypeBit)
    {
      const auto memoryType = memoryProperties.memoryTypes[memoryTypeIndex];
      if (memoryType.propertyFlags & (vk::MemoryPropertyFlagBits::eHostVisible
                                      | vk::MemoryPropertyFlagBits::eHostCoherent))
      {
        // we found the right memory, yippie!!
        break;
      }
    }
  }

  _uniformBufferMemory = vkr::DeviceMemory(
    _device,
    vk::MemoryAllocateInfo {
      .allocationSize = memoryRequirements.size,
      .memoryTypeIndex = memoryTypeIndex,
    });

  auto* bufferData = static_cast<uint8_t*>(
    _uniformBufferMemory.mapMemory(0, memoryRequirements.size));
  std::memcpy(bufferData, &mvpc, sizeof(mvpc));
  _uniformBufferMemory.unmapMemory();

  _uniformBuffer.bindMemory(*_uniformBufferMemory, 0);
}

void Renderer::renderPass()
{
  std::array<vk::AttachmentDescription, 2> attachmentDescriptions = {
    // Surface attachment
    vk::AttachmentDescription {
      .format = _surfaceImageFormat,
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eStore,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::ePresentSrcKHR},
    // Depth attachment
    vk::AttachmentDescription {
      .format = _depthImageFormat,
      .samples = vk::SampleCountFlagBits::e1,
      .loadOp = vk::AttachmentLoadOp::eClear,
      .storeOp = vk::AttachmentStoreOp::eDontCare,
      .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
      .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
      .initialLayout = vk::ImageLayout::eUndefined,
      .finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal},
  };

  const vk::AttachmentReference surfaceAttachmentReference =  {
    .attachment = 0,
    .layout = vk::ImageLayout::eColorAttachmentOptimal};

  const vk::AttachmentReference depthAttachmentReference =  {
    .attachment = 1,
    .layout = vk::ImageLayout::eDepthAttachmentOptimal};

  const vk::SubpassDescription subpassDescription = {
    .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
    .colorAttachmentCount = 1,
    .pColorAttachments = &surfaceAttachmentReference,
    .pDepthStencilAttachment = &depthAttachmentReference};

  _renderPass = vkr::RenderPass(
    _device,
    vk::RenderPassCreateInfo {
      .attachmentCount = 2,
      .pAttachments = attachmentDescriptions.data(),
      .subpassCount = 1,
      .pSubpasses = &subpassDescription});
}

void Renderer::pipeline()
{
  const vk::DescriptorSetLayoutBinding uniformDescriptorSetLayoutBinding {
    .descriptorType = vk::DescriptorType::eUniformBuffer,
    .descriptorCount = 1,
    .stageFlags = vk::ShaderStageFlagBits::eVertex};

  _uniformDescriptorLayout = vkr::DescriptorSetLayout(
    _device,
    vk::DescriptorSetLayoutCreateInfo {
      .bindingCount = 1,
      .pBindings = &uniformDescriptorSetLayoutBinding});

  _pipelineLayout = vkr::PipelineLayout(
    _device,
    vk::PipelineLayoutCreateInfo {
      .setLayoutCount = 1,
      .pSetLayouts = &(*_uniformDescriptorLayout),});

  const vk::DescriptorPoolSize uniformDescriptorPoolSize{
    .type = vk::DescriptorType::eUniformBuffer,
    .descriptorCount = 1};

  _uniformDescriptorPool = vkr::DescriptorPool(
    _device,
    vk::DescriptorPoolCreateInfo {
      .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
      .maxSets = 1,
      .poolSizeCount = 1,
      .pPoolSizes = &uniformDescriptorPoolSize});

  _uniformDescriptorSets = vkr::DescriptorSets(
    _device,
    vk::DescriptorSetAllocateInfo {
      .descriptorPool = *_uniformDescriptorPool,
      .descriptorSetCount = 1,
      .pSetLayouts = &(*_uniformDescriptorLayout)});

  const vk::DescriptorBufferInfo uniformDescriptorBufferInfo{
    .buffer = *_uniformBuffer,
    .offset = 0,
    .range = sizeof( glm::mat4x4 )};

  const vk::WriteDescriptorSet writeUniformDescriptorSet {
    .dstSet = *_uniformDescriptorSets.front(),
    .descriptorCount = 1,
    .descriptorType = vk::DescriptorType::eUniformBuffer,
    .pBufferInfo = &uniformDescriptorBufferInfo};

  _device.updateDescriptorSets(writeUniformDescriptorSet, nullptr);
}

void Renderer::commands()
{
  _commandPool = vkr::CommandPool(
    _device,
    vk::CommandPoolCreateInfo {
      .queueFamilyIndex = _queueFamilyHints.graphicsFamily.value()
    });

  _commandBuffers = vkr::CommandBuffers(
    _device,
    vk::CommandBufferAllocateInfo {
      .commandPool = *_commandPool,
      .level = vk::CommandBufferLevel::ePrimary
    });
}

void Renderer::setup()
{

  _extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  _extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
  _layers.emplace_back("VK_LAYER_KHRONOS_validation");

  const vk::ApplicationInfo applicationInfo {
    .pApplicationName = "Hello World",
    .applicationVersion = 1,
    .pEngineName = "Engine",
    .engineVersion = 1,
    .apiVersion = VK_API_VERSION_1_1,
  };

  // Extensions in contiguous array
  std::vector<const char*> extensions;
  extensions.reserve(_extensions.size());
  for (const auto& extension: _extensions)
  {
    extensions.emplace_back(extension.data());
  }

  // Layers in contiguous array
  std::vector<const char*> layers;
  extensions.reserve(_layers.size());
  for (const auto& layer: _layers)
  {
    layers.emplace_back(layer.data());
  }

  const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {
    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
                       | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
                       | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
                   | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
                   | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
    .pfnUserCallback = [](
                         VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                         VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                         const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                         void* pUserData) -> VkBool32
    {
      printf("[Vulkan] %s\n", pCallbackData->pMessage);
      return VK_FALSE;
    }};

  const vk::InstanceCreateInfo instanceCreateInfo {
    .pNext = &debugMessengerInfo,
    .pApplicationInfo = &applicationInfo,
    .enabledLayerCount = static_cast<uint32_t>(layers.size()),
    .ppEnabledLayerNames = layers.data(),
    .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data(),
  };

  // Setup instance
  _instance = vkr::Instance(_ctx, instanceCreateInfo);

  // Setup debugger
  vkr::DebugUtilsMessengerEXT debugMessenger(
    _instance, debugMessengerInfo);
}

} // namespace vulkan