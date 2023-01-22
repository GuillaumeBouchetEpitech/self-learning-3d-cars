
#pragma once

#include <cstdint>

enum class ShaderIds : uint32_t {
  stackRendererHud = 0,
  stackRendererScene,
  particles,
  simpleTexture,
  chessboardFloor,
  textRenderer,
  wireframes,
  animatedCircuitWalls,
  animatedCircuitGround,
  modelsCarWheels,
  modelsCarChassis,
  postProcess,
  backGroundTorus,
};

enum class GeometryIds : uint32_t {
  stackRendererWireframesHud = 0,
  stackRendererTrianglesHud,
  stackRendererWireframesScene,
  stackRendererTrianglesScene,
  particles,
  simpleTexture,
  chessboardFloor,
  textRenderer,
  wireframes,
  wireframesLineStrip,
  animatedCircuitWalls,
  animatedCircuitGround,
  modelsCarWheels,
  modelsCarChassis,
  // postProcess,
  backGroundTorus,
};

// enum class gero::graphic::Textures : uint32_t
// {
//   asciiFont = 0,
// };
