
#pragma once

#include <cstdint>

enum class ShadersAliases : uint32_t {
  stackRendererHud = 0,
  stackRendererScene,
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
  geometriesStackRenderer,
};

enum class GeometriesAliases : uint32_t {
  stackRendererWireFramesHud = 0,
  stackRendererTrianglesHud,
  stackRendererWireFramesScene,
  stackRendererTrianglesScene,
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
  geometriesStackRenderer,
};

// enum class gero::graphics::Textures : uint32_t
// {
//   asciiFont = 0,
// };
