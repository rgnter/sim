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
          0.0f,
          0.0f,
          0.0f
  };

  void AddBody(Body body);

};

//! Simulator.
class Simulator
{
  protected:
    Environment& _environment;

public:
  explicit Simulator(Environment& environment) noexcept;

  virtual void Tick(float time) noexcept = 0;
};

//! Body dynamics simulator.
class BodyDynamicsSimulator
    : public Simulator
{
public:
  explicit BodyDynamicsSimulator(Environment& env);

public:
  void Tick(float time) noexcept override;
};

//! Body kinematics simulator.
class BodyKinematicsSimulator
    : public Simulator
{
public:
  explicit BodyKinematicsSimulator(Environment& env);

public:
  void Tick(float time) noexcept override;
};

}// namespace sim

#endif//SIM_SIM_PP
