//
// Created by maros on 23.11.2023.
//

#ifndef SIM_VULKAN_HPP
#define SIM_VULKAN_HPP

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <string>
#include <string_view>
#include <vector>

namespace vulkan
{

constexpr bool debug = true;

//! Engine.
class Engine
{

  //! String view of extension names.
  using Extensions = std::vector<std::string_view>;
  //! String view of validation layer names.
  using Layers = std::vector<std::string_view>;

public:
  Engine();

public:
  //! Creates the engine.
  void create();

  //! Destroys the engine and performs cleanup.
  void destroy();

public:
  //! @returns Reference to Vulkan instance.
  VkInstance& vk();

  //! Must be modified before the creation of the engine.
  //! @return Reference to enabled extensions.
  Extensions& extensions();

  //! Must be modified before the creation of the engine.
  //! @return Reference to enabled validation layers.
  Layers& layers();

private:
  VkInstance _instance{};
  VkPhysicalDevice _physicalDevice{};
  VkPhysicalDeviceProperties _physicalDeviceProperties{};

private:
  Extensions _extensions;
  Layers _layers;
};

//! Display.
class Display
{
public:
  Display(Engine& engine);

public:
  //! Creates the display.
  void create();

  //! Destroys the display.
  void destroy();

  //! Begins display loop.
  void loop();

private:
  Engine& _engine;

private:
  GLFWwindow* _window{nullptr};

private:
  const std::string _title = "Game";
  const int32_t _width = 800;
  const int32_t _height = 600;

private:
  VkDevice _device{};
  VkImageView _imageView{};
  VkFramebuffer _framebuffer{};
};

}// namespace vulkan

#endif//SIM_VULKAN_HPP
