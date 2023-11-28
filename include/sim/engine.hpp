//
// Created by maros on 28.11.2023.
//

#ifndef SIM_ENGINE_HPP
#define SIM_ENGINE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <vector>

namespace sdk
{

  class Actor {
  public:
    glm::vec3 _position { 0.0f };
    glm::vec3 _direction { 0.0f };
    glm::vec3 _rotation { 0.0f };
    glm::vec3 _scale { 0.0f };

  public:
    virtual ~Actor() = default;

  public:
    //! Called when actor is created in a world.
    virtual void OnCreate() = 0;

    //! Called when actor is destroyed in a world.
    virtual void OnDestroy() = 0;

  public:
    //! Translates actor model matrix.
    //! @param translationVector Translation vector.
    //! @returns Reference to actor.
    Actor& translate(const glm::vec3& translationVector)
    {
      _position += translationVector;
      return *this;
    }

    //! Rotates actor model matrix.
    //! @param angle Angle [rad].
    //! @param axis Normalized vector representing axis to rotate on.
    Actor& rotate(float angle, const glm::vec3& axis)
    {
      _rotation += axis * angle;
      return *this;
    }
  };

  //! Viewport.
  class Viewport {
  public:
    glm::mat4x4 _projection;

  public:
    Viewport(float fov, float aspect, float near, float far)
      : _projection(glm::perspective(glm::radians<float>(fov), aspect, near, far))
    {}
  };

  //! Camera actor.
  class Camera
      : public Actor
  {
  public:
    Viewport _viewport;

  public:
    Camera()
      : _viewport(45.0f, 1.0f, 0.1f, 100.0f)
    {}

    void OnCreate() override
    {
    }
    void OnDestroy() override
    {
    }
  };

  class State
  {
  public:
    sdk::Camera _activeCamera{};

  public:
    void setActiveCamera(const sdk::Camera& camera) {
      _activeCamera = camera;
    }

    const sdk::Camera& getActiveCamera() const noexcept
    {
      return _activeCamera;
    }

    sdk::Camera& getActiveCamera() noexcept
    {
      return _activeCamera;
    }
  };


}

#endif//SIM_ENGINE_HPP
