
#include "renderTextBackground.hpp"

#include "application/context/Context.hpp"

namespace helpers {

void
renderTextBackground(
  float textDepth, const glm::vec4& colorA, const glm::vec4& colorB,
  float extraSizeA, float extraSizeB) {
  auto& context = Context::get();
  auto& hud = context.graphic.hud;
  auto& textRenderer = hud.textRenderer;
  auto& triangles = hud.stackRenderers.getTrianglesStack();

  const auto& outRectangles = textRenderer.getLatestTextRectangles();

  {
    const float bgDepth = textDepth - 0.1f;

    for (const auto& rec : outRectangles)
      triangles.pushQuad(
        rec.pos + rec.size * 0.5f, rec.size + extraSizeA, colorA, bgDepth);
  }

  {
    const float bgDepth = textDepth - 0.2f;

    for (const auto& rec : outRectangles)
      triangles.pushQuad(
        rec.pos + rec.size * 0.5f, rec.size + extraSizeB, colorB, bgDepth);
  }
}

} // namespace helpers
