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

  //! Setup frame buffers.
  void framebuffers();

  //! Setup depth buffer.
  void depthBuffer();

  //! Setup uniform buffer.
  void uniformBuffer();

  //! Setup vertex buffer.
  void vertexBuffer();

  //! Setup pipeline.
  void pipeline();

  //! Setup render pass.
  void renderPass();

  //! Setup shaders.
  void shaders();

  //! Setup command pool and command buffers.
  void commands();

  //! Setup
  void setup();

public:
  const vkr::Context _ctx {};
  vkr::Instance _instance { nullptr };
  vkr::PhysicalDevice _physicalDevice { nullptr };
  vkr::Device _device { nullptr };

  vkr::Queue _graphicsQueue { nullptr };
  vkr::Queue _presentQueue { nullptr };

  vkr::CommandPool _commandPool { nullptr };
  vkr::CommandBuffers _commandBuffers { nullptr };

  vkr::Image _depthImage { nullptr };
  vk::Format _depthImageFormat {};
  vkr::ImageView _depthImageView { nullptr };
  vkr::DeviceMemory _depthMemory { nullptr };

  vkr::DeviceMemory _uniformBufferMemory { nullptr };
  vkr::Buffer _uniformBuffer { nullptr };

  vkr::DescriptorSetLayout _uniformDescriptorLayout { nullptr };
  vkr::DescriptorPool _uniformDescriptorPool { nullptr };
  vkr::DescriptorSets _uniformDescriptorSets { nullptr };

  vkr::DeviceMemory _vertexBufferMemory { nullptr };
  vkr::Buffer _vertexBuffer { nullptr };

  vkr::SurfaceKHR _surface { nullptr };
  vk::SurfaceCapabilitiesKHR _surfaceCapabilities {};
  vkr::SwapchainKHR _swapChain { nullptr };
  std::vector<vkr::ImageView> _swapChainImageViews;
  vk::Format _surfaceImageFormat {};
  std::vector<vkr::Framebuffer> _framebuffers;

  vkr::RenderPass _renderPass { nullptr };
  vkr::Pipeline _pipeline { nullptr };
  vkr::PipelineLayout _pipelineLayout { nullptr };

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

    _renderer.shaders();

    _renderer.swapChain();

    _renderer.depthBuffer();
    _renderer.uniformBuffer();
    _renderer.vertexBuffer();

    _renderer.renderPass();

    _renderer.framebuffers();

    _renderer.pipeline();

    _renderer.commands();

    vkr::Semaphore imageAcquiredSemaphore(
      _renderer._device,
      vk::SemaphoreCreateInfo {});

    auto [result, imageIndex] = _renderer._swapChain.acquireNextImage(
      0, *imageAcquiredSemaphore);

    assert(result == vk::Result::eSuccess);
    assert(imageIndex < _renderer._swapChainImageViews.size());

    auto& commandBuffer = _renderer._commandBuffers.front();
    commandBuffer.begin({});

    std::array clearValues {
      vk::ClearValue {
        .color = vk::ClearColorValue {
          .float32 = {{ 0.2f, 0.2f, 0.2f, 0.2f}}
        }
      },
      vk::ClearValue {
        .depthStencil = vk::ClearDepthStencilValue {
          .depth = 1.0f,
          .stencil = 0,
        }
      }
    };

    vk::RenderPassBeginInfo renderPassBeginInfo {
      .renderPass = *_renderer._renderPass,
      .framebuffer = *_renderer._framebuffers[imageIndex],
      .renderArea = vk::Rect2D {
        .offset = {0, 0},
        .extent = _renderer._surfaceCapabilities.currentExtent
      },
      .clearValueCount = clearValues.size(),
      .pClearValues = clearValues.data()
    };

    commandBuffer.beginRenderPass(
      renderPassBeginInfo, vk::SubpassContents::eInline);
    commandBuffer.bindPipeline(
      vk::PipelineBindPoint::eGraphics,
      *_renderer._pipeline);
    commandBuffer.bindDescriptorSets(
      vk::PipelineBindPoint::eGraphics,
      *_renderer._pipelineLayout,
      0,
      *_renderer._uniformDescriptorSets.front(),
      nullptr);
    commandBuffer.bindVertexBuffers(
      0, {*_renderer._vertexBuffer}, {0});

    // Dynamic state
    commandBuffer.setScissor(
      0, vk::Rect2D(vk::Offset2D( 0, 0 ), _renderer._surfaceCapabilities.currentExtent));
    commandBuffer.setViewport(
      0, vk::Viewport(
           0.0f,
           0.0f,
           static_cast<float>(_renderer._surfaceCapabilities.currentExtent.width),
           static_cast<float>(_renderer._surfaceCapabilities.currentExtent.height),
           0.0f,
           1.0f)
      );

    // State

    commandBuffer.draw( 12 * 4, 1, 0, 0 );
    commandBuffer.endRenderPass();
    commandBuffer.end();

    vkr::Fence drawFence(
      _renderer._device, vk::FenceCreateInfo());

    vk::PipelineStageFlags waitDestinationStageMask(
      vk::PipelineStageFlagBits::eColorAttachmentOutput);

    vk::SubmitInfo submitInfo {
      .pWaitSemaphores = &(*imageAcquiredSemaphore),
      .pWaitDstStageMask = &waitDestinationStageMask,
      .commandBufferCount = 1,
      .pCommandBuffers = &(*commandBuffer),
    };

    _renderer._graphicsQueue.submit(submitInfo, *drawFence);

    while ( vk::Result::eTimeout == _renderer._device.waitForFences( { *drawFence }, VK_TRUE, 0 ) )
      ;

    vk::PresentInfoKHR presentInfoKHR {
      .swapchainCount = 1,
      .pSwapchains = &(*_renderer._swapChain),
      .pImageIndices = &imageIndex
    };
    result = _renderer._presentQueue.presentKHR( presentInfoKHR );

    switch ( result )
    {
      case vk::Result::eSuccess: break;
      case vk::Result::eSuboptimalKHR: printf("vk::Queue::presentKHR returned vk::Result::eSuboptimalKHR !\n"); break;
      default: assert( false );  // an unexpected result is returned !
    }

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
