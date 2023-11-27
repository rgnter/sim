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
#include <filesystem>
#include <array>
#include <memory>
#include <fstream>
#include <format>
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

  void framebuffers()
  {
    _framebuffers.reserve(_swapChainImageViews.size());

    for (const auto& swapChainImageView : _swapChainImageViews)
    {
      std::array framebufferAttachements = {
        *swapChainImageView,
        *_depthImageView,
      };

      _framebuffers.emplace_back(
        _device,
        vk::FramebufferCreateInfo {
          .renderPass = *_renderPass,
          .attachmentCount = framebufferAttachements.size(),
          .pAttachments = framebufferAttachements.data(),
          .width = _surfaceCapabilities.currentExtent.width,
          .height = _surfaceCapabilities.currentExtent.height,
          .layers = 1
        });
    }
  }

  //! Setup depth buffer.
  void depthBuffer();

  //! Setup uniform buffer.
  void uniformBuffer();

  //! Setup pipeline.
  void pipeline();

  //! Setup render pass.
  void renderPass();

  //! Setup shaders.
  void shaders()
  {
    const auto readSpvBinary = [](const std::filesystem::path& shaderBinaryPath) {
      const auto size = std::filesystem::file_size(shaderBinaryPath);
      std::ifstream input(shaderBinaryPath, std::ios::binary);
      if (input.bad())
        throw std::runtime_error(
          std::format("Couldn't find shader at '{}'", shaderBinaryPath.c_str()));

      std::vector<uint8_t> buffer(size);
      input.read(reinterpret_cast<char*>(buffer.data()), size);
      return std::pair{buffer, size};
    };

    const auto [vertexData, vertexLength] = readSpvBinary("cube-vert.spv");
    const auto [fragmentData, fragmentLength] = readSpvBinary("cube-frag.spv");

    assert(vertexData.capacity() % sizeof(uint32_t) == 0);
    assert(fragmentData.capacity() % sizeof(uint32_t) == 0);

    _vertexShader = vkr::ShaderModule(
      _device,
      vk::ShaderModuleCreateInfo{
        .codeSize = vertexLength,
        .pCode = reinterpret_cast<const uint32_t*>(vertexData.data())
      });

     _fragmentShader = vkr::ShaderModule(
      _device,
      vk::ShaderModuleCreateInfo{
        .codeSize = fragmentLength,
        .pCode = reinterpret_cast<const uint32_t*>(fragmentData.data())
      });
  }

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
  vk::Format _depthImageFormat {};
  vkr::ImageView _depthImageView { nullptr };
  vkr::DeviceMemory _depthMemory { nullptr };

private:
  vkr::DeviceMemory _uniformBufferMemory { nullptr };
  vkr::Buffer _uniformBuffer { nullptr };

  vkr::DescriptorSetLayout _uniformDescriptorLayout { nullptr };
  vkr::DescriptorPool _uniformDescriptorPool { nullptr };
  vkr::DescriptorSets _uniformDescriptorSets { nullptr };

private:
  vkr::SurfaceKHR _surface { nullptr };
  vk::SurfaceCapabilitiesKHR _surfaceCapabilities {};
  vkr::SwapchainKHR _swapChain { nullptr };
  std::vector<vkr::ImageView> _swapChainImageViews;
  vk::Format _surfaceImageFormat {};
  std::vector<vkr::Framebuffer> _framebuffers;

private:
  vkr::RenderPass _renderPass { nullptr };
  vkr::PipelineLayout _pipelineLayout { nullptr };

private:
  vkr::ShaderModule _fragmentShader { nullptr };
  vkr::ShaderModule _vertexShader { nullptr };

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

    _renderer.swapChain();
    _renderer.depthBuffer();
    _renderer.uniformBuffer();
    _renderer.pipeline();
    _renderer.shaders();
    _renderer.renderPass();

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
