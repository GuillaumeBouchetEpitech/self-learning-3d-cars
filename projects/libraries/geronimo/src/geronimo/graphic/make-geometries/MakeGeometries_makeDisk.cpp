
#include "MakeGeometries.hpp"

#include "geronimo/system/math/constants.hpp"

namespace gero {
namespace graphic {

namespace MakeGeometries {

void makeDisk(Vertices& vertices, uint32_t quality, float radiusIn,
              float radiusOut) {
  std::vector<glm::vec3> outlineVertices;
  outlineVertices.reserve(1024);

  for (uint32_t ii = 0; ii < quality; ++ii) {
    const float coef = float(ii) / float(quality);
    const float angle = coef * math::pi2;

    const float cosVal = std::cos(angle);
    const float sinVal = std::sin(angle);
    outlineVertices.push_back({cosVal, sinVal, 0.0f});
  }

  const glm::vec3 normal = {0, 0, 1};

  for (std::size_t ii = 0; ii < std::size_t(quality); ++ii) {
    const std::size_t indexA = ii;
    const std::size_t indexB = (ii + 1) % std::size_t(quality);

    vertices.push_back({outlineVertices[indexA] * radiusIn, normal});
    vertices.push_back({outlineVertices[indexA] * radiusOut, normal});
    vertices.push_back({outlineVertices[indexB] * radiusOut, normal});

    vertices.push_back({outlineVertices[indexB] * radiusIn, normal});
    vertices.push_back({outlineVertices[indexA] * radiusIn, normal});
    vertices.push_back({outlineVertices[indexB] * radiusOut, normal});
  }
}

void makePartialDisk(Vertices& vertices, uint32_t quality, float radiusIn,
                     float radiusOut, float startAngle, float sweepAngle) {
  std::vector<glm::vec3> outlineVertices;
  outlineVertices.reserve(1024);

  for (uint32_t ii = 0; ii < quality; ++ii) {
    const float coef = float(ii) / float(quality);
    const float angle = startAngle + coef * sweepAngle;

    const float radAngle = glm::radians(angle);
    const float cosVal = std::cos(radAngle);
    const float sinVal = std::sin(radAngle);
    outlineVertices.push_back({cosVal, sinVal, 0.0f});
  }

  const glm::vec3 normal = {0, 0, 1};

  for (std::size_t ii = 0; ii + 1 < std::size_t(quality); ++ii) {
    const std::size_t indexA = ii;
    const std::size_t indexB = (ii + 1) % std::size_t(quality);

    vertices.push_back({outlineVertices[indexA] * radiusIn, normal});
    vertices.push_back({outlineVertices[indexA] * radiusOut, normal});
    vertices.push_back({outlineVertices[indexB] * radiusOut, normal});

    vertices.push_back({outlineVertices[indexB] * radiusIn, normal});
    vertices.push_back({outlineVertices[indexA] * radiusIn, normal});
    vertices.push_back({outlineVertices[indexB] * radiusOut, normal});
  }
}

} // namespace MakeGeometries

} // namespace graphic
} // namespace gero
