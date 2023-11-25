//
// Created by maros on 23.11.2023.
//

#ifndef SIM_VULKAN_HPP
#define SIM_VULKAN_HPP

#include <vulkan/vulkan.hpp>
#include <GLFW/glfw3.h>

#include <string>
#include <string_view>
#include <vector>

namespace vulkan
{

  class Display;

  //! Engine.
  class Engine
  {

    friend Display;

  public:
    //! Creates the engine.
    //! @returns True if engine was created, otherwise false.
    void create();

    //! Destroys the engine and performs cleanup.
    void destroy();

  public:

    //! @returns Vulkan instance.
    VkInstance& vk();

  private:
    VkInstance _instance { };
    VkPhysicalDevice _physicalDevice { };
    VkPhysicalDeviceProperties _physicalDeviceProperties { };

  private:
    std::vector<std::string_view> _extensions;
    std::vector<std::string_view> _layers;

  };


  //! Display.
  class Display
  {
  public:
    Display(Engine& engine);

  public:
    //! Creates the display.
    //! @returns True if engine was created, otherwise false.
    [[nodiscard]] bool create() noexcept;

    //! Destroys the display.
    void destroy();

    //! Begins display loop.
    void loop();

  private:
    Engine& _engine;

  private:
    GLFWwindow* _window { nullptr };

  private:
    const std::string _title = "Game";
    const int32_t _width = 800;
    const int32_t _height = 600;

  private:
    VkDevice _device { };
    VkImageView _imageView { };
    VkFramebuffer _framebuffer { };

  };

}

#endif //SIM_VULKAN_HPP
