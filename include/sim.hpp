//
// Created by maros on 11.11.2023.
//

#ifndef SIM_SIM_HPP
#define SIM_SIM_HPP

#include "math.hpp"
#include <array>
#include <list>
#include <vector>
#include <numeric>

namespace sim {

struct Body
{
  float _weight = 0.0f;
  bool _onGround = false;

  //! Position
  math::vec3d _position{0.0f};

  //! Constant forces [kg * m * s(-2)].
  std::vector<math::vec3d> _forces;
  //! Impulse forces [kg * m * s(-2)].
  std::list<std::tuple<math::vec3d, float>> _impulseForces;

  //! Velocity [m * s].
  math::vec3d _velocity{0.0f};
  //! Acceleration [m * s(-2)]
  math::vec3d _acceleration{0.0f};
};

class Environment
{
public:
  //! List of bodies in this environment.
  std::list<Body> _bodies;

  //! Gravity acceleration constant in this environment [m*s-2].
  math::vec3d _gravity = {
          0.0f,
          -9.81f,
          0.0f
  };

  //! Wind acceleration constant in this environment [m*s-2].
  math::vec3d _wind = {
          -20.0f,
          0.0f,
          0.0f
  };

  void AddBody(Body body)
  {
    _bodies.emplace_back(std::move(body));
  }

};

//! Simulator.
class Simulator
{
  protected:
    Environment& _environment;

public:
  explicit Simulator(Environment& environment) noexcept
      : _environment(environment)
  {}

  virtual void Tick(float time) noexcept = 0;
};

//! Body dynamics simulator.
class BodyDynamicsSimulator
    : public Simulator
{
public:
  explicit BodyDynamicsSimulator(Environment& env)
      : Simulator(env) {}

public:
  void Tick(float time) noexcept override {
    for (auto &body: _environment._bodies)
    {
      const math::vec3d kineticFrictionForce
              = math::vec3d {_environment._gravity._up * (0.50 / 0.20)} * math::SidewaysVector;

      const math::vec3d staticFrictionForce
              = math::vec3d {_environment._gravity._up * (0.50 / 0.35)} * math::SidewaysVector;

      std::array<math::vec3d, 3> forces {
        // Add gravity force to the body.
        _environment._gravity * body._weight, // F = m*g

        // Add wind force to the body.
        _environment._wind * body._weight,

        // Add kinetic friction force to the body.
        body._onGround && (math::SidewaysVector * body._velocity).magnitudeSquared() > 0.1
                      ? kineticFrictionForce : math::ZeroVector,
      };

      auto force = std::accumulate(
              forces.begin(),
              forces.end(),
              math::vec3d(0));

      // Apply impulse forces.
      for (auto& [impulseForce, impulseTime] : body._impulseForces)
      {
        if (impulseTime > 0.0f)
          force += impulseForce;
        impulseTime -= time;
      }

      // If body is on ground and has no velocity,
      // force must be greater than static friction force to get the body moving.
      {
        if (body._onGround
            && body._velocity._right == 0.0
            && force._right + staticFrictionForce._right < 0.0)
          force._right = 0;

        if (body._onGround
            && body._velocity._forward == 0.0
            && force._forward + staticFrictionForce._forward < 0.0)
          force._forward = 0;
      }

      body._acceleration = force / body._weight;
    }
  }
};

//! Body kinematics simulator.
class BodyKinematicsSimulator
    : public Simulator
{
public:
  explicit BodyKinematicsSimulator(Environment& env)
      : Simulator(env) {}

public:
  void Tick(float time) noexcept override
  {
    for (auto &body: _environment._bodies)
    {
      body._velocity += body._acceleration * time;

      if((body._velocity * math::SidewaysVector).magnitudeSquared() < 0.1
            && (body._acceleration * math::SidewaysVector).magnitudeSquared() < 0.1)
      {
        body._velocity *= math::UpwardVector;
      }

      body._position += body._velocity * time;
    }
  }
};

}// namespace sim

#endif//SIM_SIM_PP
