
#pragma once

#include <cstdint>

enum class ShadersAliases : uint32_t {
  stackRendererHud = 0,
  stackRendererScene,
  particles,
  simpleTexture,
  chessboardFloor,
  textRenderer,
  wireFrames,
  animatedCircuitWalls,
  animatedCircuitGround,
  modelsCarWheels,
  modelsCarChassis,
  postProcess,
  backGroundTorus,
};

enum class GeometriesAliases : uint32_t {
  stackRendererWireFramesHud = 0,
  stackRendererTrianglesHud,
  stackRendererWireFramesScene,
  stackRendererTrianglesScene,
  particles,
  simpleTexture,
  chessboardFloor,
  textRenderer,
  wireFrames,
  wireFramesLineStrip,
  animatedCircuitWalls,
  animatedCircuitGround,
  modelsCarWheels,
  modelsCarChassis,
  // postProcess,
  backGroundTorus,
};

// enum class gero::graphics::Textures : uint32_t
// {
//   asciiFont = 0,
// };
