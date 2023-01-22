
#include "MakeGeometries.hpp"

#include "geronimo/system/math/constants.hpp"

namespace gero {
namespace graphic {

namespace MakeGeometries {

void makeCylinder(Vertices& vertices, uint32_t quality, float radiusTop,
                  float radiusBottom, float height, bool closedTop,
                  bool closedBottom) {
  std::vector<glm::vec3> outlineVertices;
  outlineVertices.reserve(quality);

  for (uint32_t ii = 0; ii < quality; ++ii) {
    const float coef = float(ii) / float(quality);
    const float angle = coef * math::pi2;

    const float cosVal = std::cos(angle);
    const float sinVal = std::sin(angle);
    outlineVertices.push_back({cosVal, sinVal, 0.0f});
  }

  const float minHeight = height * -0.5f;
  const float maxHeight = height * +0.5f;

  for (uint32_t ii = 0; ii < quality; ++ii) {
    const uint32_t indexA = ii;
    const uint32_t indexB = (ii + 1) % quality;

    const glm::vec3& rayA = outlineVertices[indexA];
    const glm::vec3& rayB = outlineVertices[indexB];

    const glm::vec3 normalA = {rayA.x, rayA.y, 0};
    const glm::vec3 normalB = {rayB.x, rayB.y, 0};

    vertices.push_back(
      {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight}, normalA});
    vertices.push_back(
      {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight}, normalB});
    vertices.push_back(
      {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalB});

    vertices.push_back(
      {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight}, normalA});
    vertices.push_back(
      {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalB});
    vertices.push_back(
      {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalA});
  }

  if (closedTop || closedBottom) {
    const glm::vec3 normalUp = {0, 0, +1};
    const glm::vec3 normalDown = {0, 0, -1};

    if (quality == 3) {
      const glm::vec3& rayA = outlineVertices[0];
      const glm::vec3& rayB = outlineVertices[1];
      const glm::vec3& rayC = outlineVertices[2];

      if (closedBottom && radiusBottom > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayC.x * radiusBottom, rayC.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight},
           normalDown});
      }

      if (closedTop && radiusTop > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayC.x * radiusTop, rayC.y * radiusTop, maxHeight}, normalUp});
      }
    } else if (quality == 4) {
      const glm::vec3& rayA = outlineVertices[0];
      const glm::vec3& rayB = outlineVertices[1];
      const glm::vec3& rayC = outlineVertices[2];
      const glm::vec3& rayD = outlineVertices[3];

      if (closedBottom && radiusBottom > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayC.x * radiusBottom, rayC.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight},
           normalDown});

        vertices.push_back(
          {{rayC.x * radiusBottom, rayC.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayD.x * radiusBottom, rayD.y * radiusBottom, minHeight},
           normalDown});
      }

      if (closedTop && radiusTop > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayC.x * radiusTop, rayC.y * radiusTop, maxHeight}, normalUp});

        vertices.push_back(
          {{rayC.x * radiusTop, rayC.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayD.x * radiusTop, rayD.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
      }
    } else {

      const glm::vec3 bottomCenter = {0, 0, minHeight};
      const glm::vec3 topCenter = {0, 0, maxHeight};

      for (uint32_t ii = 0; ii < quality; ++ii) {
        const uint32_t indexA = ii;
        const uint32_t indexB = (ii + 1) % quality;

        const glm::vec3& rayA = outlineVertices[indexA];
        const glm::vec3& rayB = outlineVertices[indexB];

        if (closedBottom && radiusBottom > 0.0f) {
          vertices.push_back(
            {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
             normalDown});
          vertices.push_back({bottomCenter, normalDown});
          vertices.push_back(
            {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight},
             normalDown});
        }

        if (closedTop && radiusTop > 0.0f) {
          vertices.push_back(
            {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
          vertices.push_back(
            {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalUp});
          vertices.push_back({topCenter, normalUp});
        }
      }
    }
  }
}

void makePartialCylinder(Vertices& vertices, uint32_t quality, float radiusTop,
                         float radiusBottom, float height, bool closedTop,
                         bool closedBottom, float startAngle,
                         float sweepAngle) {
  std::vector<glm::vec3> outlineVertices;
  outlineVertices.reserve(quality);

  for (uint32_t ii = 0; ii < quality; ++ii) {
    const float coef = float(ii) / float(quality);
    const float angle = startAngle + coef * sweepAngle;

    const float radAngle = glm::radians(angle);
    const float cosVal = std::cos(radAngle);
    const float sinVal = std::sin(radAngle);
    outlineVertices.push_back({cosVal, sinVal, 0.0f});
  }

  const float minHeight = height * -0.5f;
  const float maxHeight = height * +0.5f;

  for (uint32_t ii = 0; ii + 1 < quality; ++ii) {
    const uint32_t indexA = ii;
    const uint32_t indexB = (ii + 1) % quality;

    const glm::vec3& rayA = outlineVertices[indexA];
    const glm::vec3& rayB = outlineVertices[indexB];

    const glm::vec3 normalA = {rayA.x, rayA.y, 0};
    const glm::vec3 normalB = {rayB.x, rayB.y, 0};

    vertices.push_back(
      {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight}, normalA});
    vertices.push_back(
      {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight}, normalB});
    vertices.push_back(
      {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalB});

    vertices.push_back(
      {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight}, normalA});
    vertices.push_back(
      {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalB});
    vertices.push_back(
      {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalA});
  }

  if (closedTop || closedBottom) {
    const glm::vec3 normalUp = {0, 0, +1};
    const glm::vec3 normalDown = {0, 0, -1};

    if (quality == 3) {
      const glm::vec3& rayA = outlineVertices[0];
      const glm::vec3& rayB = outlineVertices[1];
      const glm::vec3& rayC = outlineVertices[2];

      if (closedBottom && radiusBottom > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayC.x * radiusBottom, rayC.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight},
           normalDown});
      }

      if (closedTop && radiusTop > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayC.x * radiusTop, rayC.y * radiusTop, maxHeight}, normalUp});
      }
    } else if (quality == 4) {
      const glm::vec3& rayA = outlineVertices[0];
      const glm::vec3& rayB = outlineVertices[1];
      const glm::vec3& rayC = outlineVertices[2];
      const glm::vec3& rayD = outlineVertices[3];

      if (closedBottom && radiusBottom > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayC.x * radiusBottom, rayC.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight},
           normalDown});

        vertices.push_back(
          {{rayC.x * radiusBottom, rayC.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
           normalDown});
        vertices.push_back(
          {{rayD.x * radiusBottom, rayD.y * radiusBottom, minHeight},
           normalDown});
      }

      if (closedTop && radiusTop > 0.0f) {
        vertices.push_back(
          {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayC.x * radiusTop, rayC.y * radiusTop, maxHeight}, normalUp});

        vertices.push_back(
          {{rayC.x * radiusTop, rayC.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayD.x * radiusTop, rayD.y * radiusTop, maxHeight}, normalUp});
        vertices.push_back(
          {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
      }
    } else {

      const glm::vec3 bottomCenter = {0, 0, minHeight};
      const glm::vec3 topCenter = {0, 0, maxHeight};

      for (uint32_t ii = 0; ii + 1 < quality; ++ii) {
        const uint32_t indexA = ii;
        const uint32_t indexB = (ii + 1) % quality;

        const glm::vec3& rayA = outlineVertices[indexA];
        const glm::vec3& rayB = outlineVertices[indexB];

        if (closedBottom && radiusBottom > 0.0f) {
          vertices.push_back(
            {{rayA.x * radiusBottom, rayA.y * radiusBottom, minHeight},
             normalDown});
          vertices.push_back({bottomCenter, normalDown});
          vertices.push_back(
            {{rayB.x * radiusBottom, rayB.y * radiusBottom, minHeight},
             normalDown});
        }

        if (closedTop && radiusTop > 0.0f) {
          vertices.push_back(
            {{rayA.x * radiusTop, rayA.y * radiusTop, maxHeight}, normalUp});
          vertices.push_back(
            {{rayB.x * radiusTop, rayB.y * radiusTop, maxHeight}, normalUp});
          vertices.push_back({topCenter, normalUp});
        }
      }
    }
  }
}

} // namespace MakeGeometries

} // namespace graphic
} // namespace gero
