//
// Created by maros on 23.11.2023.
//

#ifndef SIM_VULKAN_HPP
#define SIM_VULKAN_HPP

#define VULKAN_HPP_NO_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string_view>
#include <vector>

namespace vulkan
{

namespace vkr = vk::raii;

//! Renderer.
class Renderer
{

public:

  //! Setup surface.
  void surface(GLFWwindow* window);

  //! Setup physical device.
  void physicalDevice();

  //! Setup device and queues.
  void logicalDevice();

  //! Setup swap chain.
  void swapChain();

  //! Setup depth buffer.
  void depthBuffer();

  //! Setup uniform buffer.
  void uniformBuffer();

  //! Setup command pool and command buffers.
  void commands();

  //! Setup
  void setup();

private:
  const vkr::Context _ctx {};
  vkr::Instance _instance { nullptr };
  vkr::PhysicalDevice _physicalDevice { nullptr };
  vkr::Device _device { nullptr };

private:
  vkr::CommandPool _commandPool { nullptr };
  vkr::CommandBuffers _commandBuffers { nullptr };

private:
  vkr::Image _depthImage { nullptr };
  vkr::ImageView _depthImageView { nullptr };
  vkr::DeviceMemory _depthMemory { nullptr };

private:
  vkr::DeviceMemory _uniformBufferMemory { nullptr };
  vkr::Buffer _uniformBuffer { nullptr };

private:
  vkr::SurfaceKHR _surface { nullptr };
  vk::SurfaceCapabilitiesKHR _surfaceCapabilities {};
  vkr::SwapchainKHR _swapChain { nullptr };

private:
  struct QueueFamilyHints
  {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
  } _queueFamilyHints;

public:
  std::vector<std::string_view> _extensions;
  std::vector<std::string_view> _layers;

  std::vector<std::string_view> _devExtensions;
  std::vector<std::string_view> _devLayers;
};


// Display.
class Display
{
public:
  //! Sets up the display.
  void setup(Renderer& renderer) {
    if (!glfwInit())
      throw std::runtime_error("Couldn't initialize GLFW.");

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    _window = glfwCreateWindow(
      width, height, "Title", nullptr, nullptr);

    // Copy GLFW extensions
    uint32_t glfwExtensionCount = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(
      &glfwExtensionCount);
    for (int i = 0; i < glfwExtensionCount; ++i)
    {
      renderer._extensions.emplace_back(glfwExtensions[i]);
    }
  }

public:
  GLFWwindow* _window;
  int width = 800;
  int height = 600;
};

class Engine
{

public:
  void run()
  {
    _display.setup(_renderer);
    _renderer.setup();
    _renderer.surface(_display._window);
    _renderer.physicalDevice();
    _renderer.logicalDevice();
    _renderer.depthBuffer();
    _renderer.swapChain();
    _renderer.commands();

    while(!glfwWindowShouldClose(_display._window))
    {
      glfwPollEvents();
      if(glfwGetKey(_display._window, GLFW_KEY_ESCAPE))
      {
        glfwSetWindowShouldClose(_display._window, GLFW_TRUE);
      }
    }
  }

private:
  Renderer _renderer;
  Display _display;
};

} // namespace vulkan

#endif//SIM_VULKAN_HPP
