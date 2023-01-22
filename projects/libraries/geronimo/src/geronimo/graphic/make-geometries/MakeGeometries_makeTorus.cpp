

#include "MakeGeometries.hpp"

#include "geronimo/system/containers/static_heap_grid_array.hpp"
#include "geronimo/system/math/constants.hpp"

namespace gero {
namespace graphic {

namespace {

glm::vec3 getForward(const MakeGeometries::Vertex& inVertex, float inRadius) {
  return inVertex.position + inVertex.normal * inRadius;
}

} // namespace

namespace MakeGeometries {

void makeTorus(Vertices& vertices, uint32_t ringQuality, uint32_t tubeQuality,
               float ringRadius, float tubeRadius) {
  Vertices tubeSliceVertices;
  tubeSliceVertices.resize(tubeQuality);

  for (uint32_t tubeIndex = 0; tubeIndex < tubeQuality; ++tubeIndex) {
    const float coef = float(tubeIndex) / float(tubeQuality);
    const float angle = math::pi2 * 0.125f + coef * math::pi2;

    const float cosVal = std::cos(angle);
    const float sinVal = std::sin(angle);

    const glm::vec3 normal = {cosVal, 0.0f, sinVal};

    tubeSliceVertices[tubeIndex].normal = normal;
    tubeSliceVertices[tubeIndex].position = normal * tubeRadius;
  }

  static_heap_grid_array<Vertex> ringVertices;
  ringVertices.allocate(ringQuality, tubeQuality);

  for (int ringIndex = 0; ringIndex < int(ringQuality); ++ringIndex) {
    const float coef = float(ringIndex) / float(ringQuality);
    const float angle = coef * math::pi2;

    {
      const glm::mat4 transformNormal =
        glm::rotate(glm::identity<glm::mat4>(), angle, glm::vec3(0, 0, 1));
      const glm::mat4 transformVertex =
        glm::translate(transformNormal, glm::vec3(ringRadius, 0, 0));

      for (int tubeIndex = 0; tubeIndex < int(tubeQuality); ++tubeIndex) {
        const Vertex& sliceVertex = tubeSliceVertices[std::size_t(tubeIndex)];

        Vertex& vertex = ringVertices(ringIndex, tubeIndex);
        vertex.position =
          transformVertex * glm::vec4(sliceVertex.position, 1.0f);
        vertex.normal = transformNormal * glm::vec4(sliceVertex.normal, 0.0f);
      }
    }
  }

  for (int ringIndex = 0; ringIndex < int(ringQuality); ++ringIndex) {
    const int indexA1 = ringIndex;
    const int indexB1 = (ringIndex + 1) % int(ringQuality);

    for (int tubeIndex = 0; tubeIndex < int(tubeQuality); ++tubeIndex) {
      const int indexA2 = tubeIndex;
      const int indexB2 = (tubeIndex + 1) % int(tubeQuality);

      const Vertex& vertexAA = ringVertices(indexA1, indexA2);
      const Vertex& vertexAB = ringVertices(indexA1, indexB2);
      const Vertex& vertexBA = ringVertices(indexB1, indexA2);
      const Vertex& vertexBB = ringVertices(indexB1, indexB2);

      const glm::vec3 posAA = getForward(vertexAA, tubeRadius);
      const glm::vec3 posAB = getForward(vertexAB, tubeRadius);
      const glm::vec3 posBA = getForward(vertexBA, tubeRadius);
      const glm::vec3 posBB = getForward(vertexBB, tubeRadius);

      // if (!perFaceNormals)
      {
        vertices.push_back({posAA, vertexAA.normal});
        vertices.push_back({posBB, vertexBB.normal});
        vertices.push_back({posAB, vertexAB.normal});

        vertices.push_back({posAA, vertexAA.normal});
        vertices.push_back({posBA, vertexBA.normal});
        vertices.push_back({posBB, vertexBB.normal});
      }
      // else
      // {
      //   const glm::vec3 normal1 = -glm::cross(posAA - posAB, posAA - posBB);
      //   const glm::vec3 normal2 = glm::cross(posAA - posBA, posAA - posBB);

      //   vertices.push_back({ posAA, normal1 });
      //   vertices.push_back({ posAB, normal1 });
      //   vertices.push_back({ posBB, normal1 });

      //   vertices.push_back({ posAA, normal2 });
      //   vertices.push_back({ posBA, normal2 });
      //   vertices.push_back({ posBB, normal2 });
      // }
    }
  }
}

void makePartialTorus(Vertices& vertices, uint32_t ringQuality,
                      uint32_t tubeQuality, float ringRadius, float tubeRadius,
                      bool closedStart, bool closedEnd, float startAngle,
                      float sweepAngle) {
  Vertices tubeSliceVertices;
  tubeSliceVertices.resize(tubeQuality);

  for (uint32_t tubeIndex = 0; tubeIndex < tubeQuality; ++tubeIndex) {
    const float coef = float(tubeIndex) / float(tubeQuality);
    const float angle = math::pi2 * 0.125f + coef * math::pi2;

    const float cosVal = std::cos(angle);
    const float sinVal = std::sin(angle);

    const glm::vec3 normal = {cosVal, 0.0f, sinVal};

    tubeSliceVertices[tubeIndex].normal = normal;
    tubeSliceVertices[tubeIndex].position = normal * tubeRadius;
  }

  std::vector<glm::vec3> ringCenters;
  ringCenters.reserve(ringQuality);

  static_heap_grid_array<Vertex> ringVertices;
  ringVertices.allocate(ringQuality, tubeQuality);

  for (int ringIndex = 0; ringIndex < int(ringQuality); ++ringIndex) {
    const float coef = float(ringIndex) / float(ringQuality);
    const float angle = startAngle + coef * sweepAngle;

    const float radAngle = glm::radians(angle);
    ringCenters.push_back(
      {ringRadius * std::cos(radAngle), ringRadius * std::sin(radAngle), 0});

    const glm::mat4 transformNormal =
      glm::rotate(glm::identity<glm::mat4>(), radAngle, glm::vec3(0, 0, 1));
    const glm::mat4 transformVertex =
      glm::translate(transformNormal, glm::vec3(ringRadius, 0, 0));

    for (int tubeIndex = 0; tubeIndex < int(tubeQuality); ++tubeIndex) {
      const Vertex& sliceVertex = tubeSliceVertices[std::size_t(tubeIndex)];

      Vertex& vertex = ringVertices(ringIndex, tubeIndex);
      vertex.position = transformVertex * glm::vec4(sliceVertex.position, 1.0f);
      vertex.normal = transformNormal * glm::vec4(sliceVertex.normal, 0.0f);
    }
  }

  for (int ringIndex = 0; ringIndex + 1 < int(ringQuality); ++ringIndex) {
    const int indexA1 = ringIndex;
    const int indexB1 = (ringIndex + 1) % int(ringQuality);

    for (int tubeIndex = 0; tubeIndex < int(tubeQuality); ++tubeIndex) {
      const int indexA2 = tubeIndex;
      const int indexB2 = (tubeIndex + 1) % int(tubeQuality);

      const Vertex& vertexAA = ringVertices(indexA1, indexA2);
      const Vertex& vertexAB = ringVertices(indexA1, indexB2);
      const Vertex& vertexBA = ringVertices(indexB1, indexA2);
      const Vertex& vertexBB = ringVertices(indexB1, indexB2);

      const glm::vec3 posAA = getForward(vertexAA, tubeRadius);
      const glm::vec3 posAB = getForward(vertexAB, tubeRadius);
      const glm::vec3 posBA = getForward(vertexBA, tubeRadius);
      const glm::vec3 posBB = getForward(vertexBB, tubeRadius);

      // if (!perFaceNormals)
      {
        vertices.push_back({posAA, vertexAA.normal});
        vertices.push_back({posBB, vertexBB.normal});
        vertices.push_back({posAB, vertexAB.normal});

        vertices.push_back({posAA, vertexAA.normal});
        vertices.push_back({posBA, vertexBA.normal});
        vertices.push_back({posBB, vertexBB.normal});
      }
      // else
      // {
      //   const glm::vec3 normal1 = -glm::cross(posAA - posAB, posAA - posBB);
      //   const glm::vec3 normal2 = glm::cross(posAA - posBA, posAA - posBB);

      //   vertices.push_back({ posAA, normal1 });
      //   vertices.push_back({ posAB, normal1 });
      //   vertices.push_back({ posBB, normal1 });

      //   vertices.push_back({ posAA, normal2 });
      //   vertices.push_back({ posBA, normal2 });
      //   vertices.push_back({ posBB, normal2 });
      // }
    }
  }

  if (closedStart || closedEnd) {
    if (tubeQuality == 3) {
      if (closedStart) {
        const glm::vec3 posA = getForward(ringVertices(0, 0), tubeRadius);
        const glm::vec3 posB = getForward(ringVertices(0, 1), tubeRadius);
        const glm::vec3 posC = getForward(ringVertices(0, 2), tubeRadius);

        const glm::vec3 normal = glm::cross(posA - posB, posA - posC);

        vertices.push_back({posA, normal});
        vertices.push_back({posB, normal});
        vertices.push_back({posC, normal});
      }

      if (closedEnd) {
        auto it =
          ringVertices.beginColumns(uint32_t(ringVertices.height()) - 1);
        const glm::vec3 posA = getForward(*(it + 0), tubeRadius);
        const glm::vec3 posB = getForward(*(it + 1), tubeRadius);
        const glm::vec3 posC = getForward(*(it + 2), tubeRadius);

        const glm::vec3 normal = -glm::cross(posA - posB, posA - posC);

        vertices.push_back({posA, normal});
        vertices.push_back({posC, normal});
        vertices.push_back({posB, normal});
      }
    } else if (tubeQuality == 4) {
      if (closedStart) {
        const glm::vec3 posA = getForward(ringVertices(0, 0), tubeRadius);
        const glm::vec3 posB = getForward(ringVertices(0, 1), tubeRadius);
        const glm::vec3 posC = getForward(ringVertices(0, 2), tubeRadius);
        const glm::vec3 posD = getForward(ringVertices(0, 3), tubeRadius);

        const glm::vec3 normal = glm::cross(posA - posB, posA - posC);

        vertices.push_back({posA, normal});
        vertices.push_back({posB, normal});
        vertices.push_back({posC, normal});

        vertices.push_back({posA, normal});
        vertices.push_back({posC, normal});
        vertices.push_back({posD, normal});
      }

      if (closedEnd) {
        auto it =
          ringVertices.beginColumns(uint32_t(ringVertices.height()) - 1);
        const glm::vec3 posA = getForward(*(it + 0), tubeRadius);
        const glm::vec3 posB = getForward(*(it + 1), tubeRadius);
        const glm::vec3 posC = getForward(*(it + 2), tubeRadius);
        const glm::vec3 posD = getForward(*(it + 3), tubeRadius);

        const glm::vec3 normal = -glm::cross(posA - posB, posA - posC);

        vertices.push_back({posA, normal});
        vertices.push_back({posC, normal});
        vertices.push_back({posB, normal});

        vertices.push_back({posA, normal});
        vertices.push_back({posD, normal});
        vertices.push_back({posC, normal});
      }
    } else {
      const glm::vec3& centerF = ringCenters.front();
      const glm::vec3& centerB = ringCenters.back();

      for (int tubeIndex = 0; tubeIndex < int(tubeQuality); ++tubeIndex) {
        const int index1 = tubeIndex;
        const int index2 = (tubeIndex + 1) % int(tubeQuality);

        if (closedStart) {
          const glm::vec3 posF1 =
            getForward(ringVertices(0, index1), tubeRadius);
          const glm::vec3 posF2 =
            getForward(ringVertices(0, index2), tubeRadius);
          const glm::vec3 normalF =
            glm::cross(centerF - posF1, centerF - posF2);

          vertices.push_back({centerF, normalF});
          vertices.push_back({posF1, normalF});
          vertices.push_back({posF2, normalF});
        }

        if (closedEnd) {
          auto it =
            ringVertices.beginColumns(uint32_t(ringVertices.height()) - 1);
          const glm::vec3 posB1 = getForward(*(it + index1), tubeRadius);
          const glm::vec3 posB2 = getForward(*(it + index2), tubeRadius);
          const glm::vec3 normalB =
            -glm::cross(centerB - posB1, centerB - posB2);

          vertices.push_back({centerB, normalB});
          vertices.push_back({posB2, normalB});
          vertices.push_back({posB1, normalB});
        }
      }
    }
  }
}

} // namespace MakeGeometries

} // namespace graphic
} // namespace gero
