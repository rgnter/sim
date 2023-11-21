//
// Created by maros on 21.11.2023.
//

#include "sim/sim.hpp"

void sim::Environment::AddBody(sim::Body body) {
  _bodies.emplace_back(std::move(body));
}

sim::Simulator::Simulator(sim::Environment &environment) noexcept
        : _environment(environment)
{}

sim::BodyDynamicsSimulator::BodyDynamicsSimulator(sim::Environment &env)
        : Simulator(env) {}

void sim::BodyDynamicsSimulator::Tick(float time) noexcept {
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

sim::BodyKinematicsSimulator::BodyKinematicsSimulator(sim::Environment &env)
        : Simulator(env) {}

void sim::BodyKinematicsSimulator::Tick(float time) noexcept {
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
