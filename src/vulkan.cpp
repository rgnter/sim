//
// Created by maros on 25.11.2023.
//

#include "sim/vulkan.hpp"

#include <cstdio>
#include <iostream>
#include <format>

namespace vulkan
{

namespace
{

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData)
{
  std::cerr << "[Vulkan] " << pCallbackData->pMessage << std::endl;
  return VK_FALSE;
}

void checkValidationLayerSupport(const std::vector<std::string_view>& layers)
{
  uint32_t layerCount;
  vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
  std::vector<VkLayerProperties> availableLayers(layerCount);
  vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

  for (const auto& requiredLayer: layers)
  {
    bool isLayerSupported = false;

    for (const auto& availableLayer: availableLayers)
    {
      if (requiredLayer == availableLayer.layerName)
      {
        isLayerSupported = true;
        break;
      }
    }

    if (!isLayerSupported)
    {
      throw std::runtime_error(
        std::format("Validation layer '{}' not supported", requiredLayer.data()));
    }
  }
}

}// namespace anon

Engine::Engine()
{
  if (debug)
  {
    _layers.emplace_back("VK_LAYER_KHRONOS_validation");
    _extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }
}

void Engine::create()
{
  const VkApplicationInfo appInfo{
    .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
    .pApplicationName = "Hello World",
    .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
    .pEngineName = "Engine",
    .engineVersion = VK_MAKE_VERSION(1, 0, 0),
    .apiVersion = VK_API_VERSION_1_0,
  };

  // Create contiguous array of extension name pointers.
  std::vector<const char*> extensions;
  for (const auto& extension: _extensions)
  {
    extensions.push_back(extension.data());
  }

  // Create contiguous array of layer name pointers,
  // also check for support of required layers.
  std::vector<const char*> layers;
  if (debug)
  {
    checkValidationLayerSupport(_layers);
    for (const auto& layer: _layers)
    {
      layers.push_back(layer.data());
    }
  }

  const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{
    .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
    .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
                       | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
    .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
                   | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
                   | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
    .pfnUserCallback = debugCallback,
    .pUserData = nullptr // Optional
  };

  VkInstanceCreateInfo createInfo {
    .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
    .pApplicationInfo = &appInfo,
    .enabledLayerCount = static_cast<uint32_t>(layers.size()),
    .ppEnabledLayerNames = layers.data(),
    .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
    .ppEnabledExtensionNames = extensions.data(),
  };

  if (debug)
    createInfo.pNext = &debugMessengerCreateInfo;

  if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
  {
    throw std::runtime_error("Unable to create vulkan instance");
  }

  if (debug)
  {
    if (ExtFunction<PFN_vkCreateDebugUtilsMessengerEXT>(_instance, "vkCreateDebugUtilsMessengerEXT")(
          _instance, &debugMessengerCreateInfo, nullptr, &_debugMessenger))
    {
      throw std::runtime_error("Couldn't create debug messenger");
    }
  }
}

void Engine::destroy()
{
  if (debug)
  {
    if (_debugMessenger)
      ExtFunction<PFN_vkDestroyDebugUtilsMessengerEXT>(_instance, "vkDestroyDebugUtilsMessengerEXT")(_instance, _debugMessenger, nullptr);
  }

  if (_instance)
    vkDestroyInstance(_instance, nullptr);
}

VkInstance& Engine::vk()
{
  return _instance;
}

Engine::Extensions& Engine::extensions()
{
  return _extensions;
}

Engine::Extensions& Engine::layers()
{
  return _layers;
}

Display::Display(Engine& engine)
    : _engine(engine) {}

void Display::create()
{
  if (!glfwInit())
    throw std::runtime_error("Couldn't initialize GLFW");

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions;
  glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

  // Copy the GLFW required extensions to engine's extension list.
  if (glfwExtensions)
  {
    auto& engineExtensions = _engine.extensions();
    for (int32_t i = 0; i < glfwExtensionCount; i++)
    {
      engineExtensions.emplace_back(glfwExtensions[i]);
    }
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  _window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
  if (!_window)
    throw std::runtime_error("Couldn't create window");
}

void Display::destroy()
{
  if (_window)
    glfwDestroyWindow(_window);

  glfwTerminate();
}

void Display::loop()
{
  while (!glfwWindowShouldClose(_window))
  {
    glfwPollEvents();
  }
}

}// namespace vulkan