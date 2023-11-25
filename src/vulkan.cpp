//
// Created by maros on 25.11.2023.
//

#include "sim/vulkan.hpp"

#include <cstdio>
#include <format>

namespace vulkan
{

  namespace
  {
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
            std::format(
              "Validation layer '{}' not supported", requiredLayer.data()));
        }
      }
    }
  } // anon namespace

  void Engine::create() {
    const VkApplicationInfo appInfo {
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

    // Create contiguous array of layer name pointers.
    checkValidationLayerSupport(_layers);
    std::vector<const char*> layers;
    for (const auto& layer: _layers)
    {
      layers.push_back(layer.data());
    }

    const VkInstanceCreateInfo createInfo {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pApplicationInfo = &appInfo,
      .enabledLayerCount = static_cast<uint32_t>(layers.size()),
      .ppEnabledLayerNames = layers.data(),
      .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
      .ppEnabledExtensionNames = extensions.data(),
    };

    if (vkCreateInstance(&createInfo, nullptr, &_instance) != VK_SUCCESS)
    {
      throw std::runtime_error("Unable to create vulkan instance");
    }
  }

  void Engine::destroy() {
    if (_instance)
      vkDestroyInstance(_instance, nullptr);
  }

  Display::Display(Engine& engine)
      : _engine(engine) {}

  bool Display::create() noexcept {
    if (!glfwInit())
      return false;

    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // Copy the GLFW required extensions to engine's extension list.
    if(glfwExtensions)
    {
      for (int32_t i = 0; i < glfwExtensionCount; i++)
      {
        _engine._extensions.emplace_back(glfwExtensions[i]);
      }
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(_width, _height, _title.c_str(), nullptr, nullptr);
    if (!_window)
      return false;

    return true;
  }

  void Display::destroy() {
    if (_window)
      glfwDestroyWindow(_window);

    glfwTerminate();
  }

  void Display::loop() {
    while(!glfwWindowShouldClose(_window))
    {
      glfwPollEvents();
    }
  }

} // vulkan namespace