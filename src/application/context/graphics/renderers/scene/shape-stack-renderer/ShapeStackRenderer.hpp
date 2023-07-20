
#pragma once

#include "internals/GeometriesStackRenderer.hpp"

class ShapeStackRenderer {
public:
  ShapeStackRenderer() = default;
  ShapeStackRenderer(const ShapeStackRenderer& other) = delete;
  ShapeStackRenderer(ShapeStackRenderer&& other) = delete;
  ShapeStackRenderer& operator=(const ShapeStackRenderer& other) = delete;
  ShapeStackRenderer& operator=(ShapeStackRenderer&& other) = delete;
  ~ShapeStackRenderer() = default;

public:
  void initialize();

public:
  void pushSphere(const GeometriesStackRenderer::GeometryInstance& instance);
  void pushSuperiorSphere(const GeometriesStackRenderer::GeometryInstance& instance);
  void pushBox(const GeometriesStackRenderer::GeometryInstance& instance);

public:
  void render(const gero::graphics::Camera& inCamera);

private:
  GeometriesStackRenderer _geometriesStackRenderer;
};
