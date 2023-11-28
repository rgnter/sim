#include <cstdio>
#include <iostream>

#include <sim/engine.hpp>
#include <sim/vulkan.hpp>


//  sim::Environment env {
//    ._wind = math::vec3d{0.0f}};
//
//  sim::BodyDynamicsSimulator dynamicsSimulator(env);
//  sim::BodyKinematicsSimulator kinematicsSimulator(env);
//
//  auto body = sim::Body {
//    ._weight = 0,
//    ._position = {0, -25, 0}};
//
//
//
//  auto extractFloat = [](const char* data, float& value) {
//    std::from_chars(data, data + std::strlen(data), value);
//  };
//
//  if (argc >= 6) {
//    extractFloat(argv[1], body._weight);
//
//    float duration = 0.0f;
//    math::vec3f impulse(0.0f);
//
//    extractFloat(argv[2], impulse._right);
//    extractFloat(argv[3], impulse._up);
//    extractFloat(argv[4], impulse._forward);
//    extractFloat(argv[5], duration);
//
//    body._impulseForces.emplace_back(
//      math::vec3d{
//        impulse._right,
//        impulse._up,
//        impulse._forward},
//      duration);
//  }
//
//  env.AddBody(body);
//
//  using Clock = std::chrono::high_resolution_clock;
//
//  // Time [s].
//  float simulationTime = 0.0f;
//  // Ticks per second.
//  const int32_t tps = 128;
//  // Time per tick [ms].
//  const long timePerTick = 1000 / tps;
//
//  Clock::time_point lastTick;
//  std::chrono::duration<float, std::chrono::milliseconds::period> lastTickDuration{};
//
//  bool simulation = true;
//  while (simulation) {
//    const auto tickNow = Clock::now();
//    // Delta between ticks
//    const auto tickDelta = std::chrono::duration_cast<std::chrono::milliseconds>(
//      tickNow - lastTick);
//
//    if (tickDelta < std::chrono::milliseconds(timePerTick)) {
//      std::this_thread::sleep_for(tickDelta);
//      continue;
//    }
//
//    // Tick simulation simulationTime [s].
//    const auto tickSimulationTime = timePerTick / 1000.0f;
//    simulationTime += tickSimulationTime;
//
//    dynamicsSimulator.Tick(tickSimulationTime);
//    kinematicsSimulator.Tick(tickSimulationTime);
//
//    lastTick = tickNow;
//    lastTickDuration = Clock::now() - tickNow;
//  }
//
//  return 0;


int main(int argc, char** argv)
{
  sdk::State state;
  state.getActiveCamera()
    .translate({-5.0f, -6.0f, -10.0f});



  vulkan::Engine engine;
  engine.run(state);

  return 0;
}
