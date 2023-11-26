#include <cstdio>
#include <iostream>

#include <sim/vulkan.hpp>

//  sim::Environment env {
//    ._wind = math::vec3d{0.0f}};
//
//  sim::BodyDynamicsSimulator dynamicsSimulator(env);
//  sim::BodyKinematicsSimulator kinematicsSimulator(env);
//
//  auto body = sim::Body {
//    ._weight = 0,
//    ._position = {0, -25, 0}};
//
//
//
//  auto extractFloat = [](const char* data, float& value) {
//    std::from_chars(data, data + std::strlen(data), value);
//  };
//
//  if (argc >= 6) {
//    extractFloat(argv[1], body._weight);
//
//    float duration = 0.0f;
//    math::vec3f impulse(0.0f);
//
//    extractFloat(argv[2], impulse._right);
//    extractFloat(argv[3], impulse._up);
//    extractFloat(argv[4], impulse._forward);
//    extractFloat(argv[5], duration);
//
//    body._impulseForces.emplace_back(
//      math::vec3d{
//        impulse._right,
//        impulse._up,
//        impulse._forward},
//      duration);
//  }
//
//  env.AddBody(body);
//
//  using Clock = std::chrono::high_resolution_clock;
//
//  // Time [s].
//  float simulationTime = 0.0f;
//  // Ticks per second.
//  const int32_t tps = 128;
//  // Time per tick [ms].
//  const long timePerTick = 1000 / tps;
//
//  Clock::time_point lastTick;
//  std::chrono::duration<float, std::chrono::milliseconds::period> lastTickDuration{};
//
//  bool simulation = true;
//  while (simulation) {
//    const auto tickNow = Clock::now();
//    // Delta between ticks
//    const auto tickDelta = std::chrono::duration_cast<std::chrono::milliseconds>(
//      tickNow - lastTick);
//
//    if (tickDelta < std::chrono::milliseconds(timePerTick)) {
//      std::this_thread::sleep_for(tickDelta);
//      continue;
//    }
//
//    // Tick simulation simulationTime [s].
//    const auto tickSimulationTime = timePerTick / 1000.0f;
//    simulationTime += tickSimulationTime;
//
//    dynamicsSimulator.Tick(tickSimulationTime);
//    kinematicsSimulator.Tick(tickSimulationTime);
//
//    lastTick = tickNow;
//    lastTickDuration = Clock::now() - tickNow;
//  }
//
//  return 0;

//  if (!glfwInit())
//    throw std::runtime_error("Couldn't initialize GLFW.");
//
//  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
//
//  auto window = glfwCreateWindow(
//    800, 600, "Title", nullptr, nullptr);
//
//  std::vector<const char*> extensions;
//
//  // Copy GLFW extensions
//  uint32_t glfwExtensionCount = 0;
//  auto glfwExtensions = glfwGetRequiredInstanceExtensions(
//    &glfwExtensionCount);
//  for (int i = 0; i < glfwExtensionCount; ++i)
//  {
//    extensions.emplace_back(glfwExtensions[i]);
//  }
//
//  // Debug extensions
//  extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
//
//  // Debug messenger
//  const vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {
//    .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError
//                       | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
//                       | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
//    .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral
//                   | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
//                   | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
//  .pfnUserCallback = [](
//       VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
//       VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
//       const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
//       void*                                            pUserData) -> VkBool32
//    {
//      std::cout << "[Vulkan] " << pCallbackData->pMessage << "\n";
//      return VK_FALSE;
//    }};
//
//  // Initialize the application
//  const vk::ApplicationInfo applicationInfo {
//    .pNext = &debugMessengerInfo,
//    .pApplicationName = "Hello World",
//    .applicationVersion = 1,
//    .pEngineName = "Engine",
//    .engineVersion = 1,
//    .apiVersion = VK_API_VERSION_1_1,
//  };
//
//  const vk::InstanceCreateInfo instanceCreateInfo {
//    .pApplicationInfo = &applicationInfo,
//    .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
//    .ppEnabledExtensionNames = extensions.data()};
//
//  const vkr::Context ctx;
//  const auto instance = vkr::Instance(
//    ctx, instanceCreateInfo);
//
//  // Initialize debugger
//  vkr::DebugUtilsMessengerEXT debugMessenger(
//    instance, debugMessengerInfo);
//
//  // Create surface
//  VkSurfaceKHR directSurface;
//  glfwCreateWindowSurface(*instance, window, nullptr, &directSurface);
//  vkr::SurfaceKHR surface(instance, directSurface);
//
//  // Choose physical device
//  const vkr::PhysicalDevices physicalDevices(instance);
//  const auto& physicalDevice = physicalDevices.front();
//
//  // Index queue families
//  struct QueueFamilyHints
//  {
//    std::optional<uint32_t> graphicsFamily;
//    std::optional<uint32_t> presentFamily;
//  } queueFamilyHints;
//  float queuePriorities[] = {0.0f};
//
//  const auto queueFamilyProperties
//    = physicalDevice.getQueueFamilyProperties();
//
//  for (int32_t index = 0; const auto& queueFamily: queueFamilyProperties)
//  {
//    if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
//    {
//      queueFamilyHints.graphicsFamily = index;
//    }
//    if (physicalDevice.getSurfaceSupportKHR(index, *surface))
//    {
//      queueFamilyHints.presentFamily = index;
//    }
//    index++;
//  }
//
//  // There must be family supporting presenting and graphics
//  if (!queueFamilyHints.graphicsFamily || !queueFamilyHints.presentFamily)
//    throw std::runtime_error("No queue family supporting graphics and presentation found");
//
//  // Create logical device
//  vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
//    .queueFamilyIndex = queueFamilyHints.graphicsFamily.value(),
//    .queueCount = 1,
//    .pQueuePriorities = queuePriorities,
//  };
//
//  std::vector<const char*> deviceExtensions;
//  deviceExtensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
//
//  vk::DeviceCreateInfo deviceCreateInfo{
//    .queueCreateInfoCount = 1,
//    .pQueueCreateInfos = &deviceQueueCreateInfo,
//    .enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
//    .ppEnabledExtensionNames = deviceExtensions.data()
//  };
//
//  vkr::Device device(physicalDevice, deviceCreateInfo);
//
//  // Create swapChain
//  const auto surfaceFormats
//    = physicalDevice.getSurfaceFormatsKHR(*surface);
//  if (surfaceFormats.empty())
//    throw std::runtime_error("No surface formats supported.");
//
//  const auto& imageFormat = surfaceFormats.front().format == vk::Format::eUndefined
//                        ? vk::Format::eB8G8R8A8Unorm
//                        : surfaceFormats.front().format;
//
//  const auto surfaceCapabilities
//    = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
//
//  vk::Extent2D swapChainExtent =
//    surfaceCapabilities.currentExtent;
//  vk::PresentModeKHR swapChainPresentMode = vk::PresentModeKHR::eFifo;
//
//  vk::SurfaceTransformFlagBitsKHR preTransform =
//    surfaceCapabilities.supportedTransforms & vk::SurfaceTransformFlagBitsKHR::eIdentity
//     ? vk::SurfaceTransformFlagBitsKHR::eIdentity : surfaceCapabilities.currentTransform;
//
//  vk::CompositeAlphaFlagBitsKHR compositeAlpha =
//    surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied
//      ? vk::CompositeAlphaFlagBitsKHR::ePreMultiplied : surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied
//          ? vk::CompositeAlphaFlagBitsKHR::ePostMultiplied : surfaceCapabilities.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eInherit
//              ? vk::CompositeAlphaFlagBitsKHR::eInherit : vk::CompositeAlphaFlagBitsKHR::eOpaque;
//
//  vk::SwapchainCreateInfoKHR swapChainCreateInfo {
//    .surface = *surface,
//    .minImageCount = surfaceCapabilities.minImageCount,
//    .imageFormat = imageFormat,
//    .imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear,
//    .imageExtent = swapChainExtent,
//    .imageArrayLayers = 1,
//    .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
//    .imageSharingMode = vk::SharingMode::eExclusive,
//    .preTransform = preTransform,
//    .compositeAlpha = compositeAlpha,
//    .presentMode = swapChainPresentMode,
//    .clipped = true,
//  };
//
//  if (queueFamilyHints.graphicsFamily != queueFamilyHints.presentFamily)
//  {
//    // If the graphics and present queues are from different queue families, we either have to explicitly transfer
//    // ownership of images between the queues, or we have to create the swapChain with imageSharingMode as VK_SHARING_MODE_CONCURRENT
//    uint32_t indices[] = {
//      *queueFamilyHints.graphicsFamily,
//      *queueFamilyHints.presentFamily
//    };
//
//    swapChainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
//    swapChainCreateInfo.queueFamilyIndexCount = 2;
//    swapChainCreateInfo.pQueueFamilyIndices = indices;
//  }
//
//  vkr::SwapchainKHR swapChain(device, swapChainCreateInfo);
//  const auto swapChainImages = swapChain.getImages();
//  std::vector<vkr::ImageView> swapChainImageViews;
//  swapChainImageViews.reserve(swapChainImages.size());
//
//  vk::ImageViewCreateInfo imageViewCreateInfo {
//    .viewType = vk::ImageViewType::e2D,
//    .format = imageFormat,
//    .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}
//  };
//
//  for (const auto& image : swapChainImages)
//  {
//    imageViewCreateInfo.image = image;
//    swapChainImageViews.emplace_back(device, imageViewCreateInfo);
//  }


int main(int argc, char** argv)
{
  vulkan::Engine engine;
  engine.run();

  return 0;
}
