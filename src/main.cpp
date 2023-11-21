#include <cstdio>

#include <cmath>
#include <chrono>
#include <thread>
#include <cstring>
#include <charconv>

#include "sim/sim.hpp"

int main(int argc, char** argv) {
  sim::Environment env;
  env._wind = 0.0f;

  sim::BodyDynamicsSimulator dynamicsSimulator(env);
  sim::BodyKinematicsSimulator kinematicsSimulator(env);

  auto body = sim::Body {
    ._weight = 0,
    ._position = {0, -25, 0}
  };

  auto extractFloat = [](const char* data, float & value)
  {
    std::from_chars(data, data + std::strlen(data), value);
  };

  if (argc >= 6)
  {
    extractFloat(argv[1], body._weight);

    float duration = 0.0f;
    math::vec3f impulse(0.0f);
    extractFloat(argv[2], impulse._right);
    extractFloat(argv[3], impulse._up);
    extractFloat(argv[4], impulse._forward);
    extractFloat(argv[5], duration);

    body._impulseForces.emplace_back(math::vec3d{impulse._right, impulse._up, impulse._forward}, duration);
  }

  env.AddBody(body);

  using Clock = std::chrono::high_resolution_clock;

  // Time [s].
  float simulationTime = 0.0f;
  // Ticks per second.
  const int32_t tps = 128;
  // Time per tick [ms].
  const long timePerTick = 1000 / tps;

  Clock::time_point lastTick;
  std::chrono::duration<float, std::chrono::milliseconds::period> lastTickDuration{};

  bool simulation = true;
  while (simulation) {
    const auto tickNow = Clock::now();
    // Delta between ticks
    const auto tickDelta = std::chrono::duration_cast<std::chrono::milliseconds>(
            tickNow - lastTick);

    if (tickDelta < std::chrono::milliseconds(timePerTick))
    {
      std::this_thread::sleep_for(tickDelta);
      continue;
    }

    // Tick simulation simulationTime [s].
    const auto tickSimulationTime = timePerTick / 1000.0f;
    simulationTime += tickSimulationTime;

    dynamicsSimulator.Tick(tickSimulationTime);
    kinematicsSimulator.Tick(tickSimulationTime);

    for (auto &body: env._bodies)
    {
      printf("\033[2J\033[0;0f\033[97m");
      printf("Accel [m*s-2]: (right: %.2f, up: %.2f), ", body._acceleration._right, body._acceleration._up);
      printf("Velocity [m*s]: (right: %.2f, up: %.2f), \n", body._velocity._right, body._velocity._up);
      printf("Pos [m]: (right: %.2f, up: %.2f) \n", body._position._right, body._position._up);
      printf("Time: %.2f [s], TPS: %d, MSPT: %.5f [ms]", simulationTime, tps, lastTickDuration.count());

      auto [x, y, z] = body._position;

      if(y <= -50)
      {
        y = -50;
        body._onGround = true;
        return 0;
      }

      printf("\033[%d;%df",
             (static_cast<int>(-std::round(y)) + 5),
             static_cast<int>(std::round(x)));

      printf("\033[91mo");
      fflush(stdout);
    }

    lastTick = tickNow;
    lastTickDuration =  Clock::now() - tickNow;
  }



  return 0;
}
