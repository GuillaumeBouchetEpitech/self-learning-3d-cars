
#include "MakeGeometries.hpp"

#include "geronimo/system/math/constants.hpp"

#include <array>

namespace gero {
namespace graphic {

namespace MakeGeometries {

void makeBox(Vertices& vertices, float sizeX, float sizeY, float sizeZ) {
  std::vector<glm::vec3> tubeDiskVertices;
  tubeDiskVertices.reserve(6 * 3 * 2);

  const float hSizeX = sizeX * 0.5f;
  const float hSizeY = sizeY * 0.5f;
  const float hSizeZ = sizeZ * 0.5f;

  std::array<glm::vec3, 6> normals{{
    {-1, 0, 0}, // 0
    {+1, 0, 0}, // 1
    {0, -1, 0}, // 2
    {0, +1, 0}, // 3
    {0, 0, -1}, // 4
    {0, 0, +1}, // 5
  }};

  std::array<glm::vec3, 8> tmpVertices{{
    {-hSizeX, -hSizeY, -hSizeZ}, // 0
    {+hSizeX, -hSizeY, -hSizeZ}, // 1
    {-hSizeX, +hSizeY, -hSizeZ}, // 2
    {+hSizeX, +hSizeY, -hSizeZ}, // 3
    {-hSizeX, -hSizeY, +hSizeZ}, // 4
    {+hSizeX, -hSizeY, +hSizeZ}, // 5
    {-hSizeX, +hSizeY, +hSizeZ}, // 6
    {+hSizeX, +hSizeY, +hSizeZ}, // 7
  }};

  using Indices = std::array<std::size_t, 4>;

  std::array<Indices, 12> indices{{
    // -z 0123
    {0, 2, 1, /*normal => */ 4},
    {2, 3, 1, /*normal => */ 4},
    // +z 4567
    {4, 5, 6, /*normal => */ 5},
    {6, 5, 7, /*normal => */ 5},

    // +x 1357
    {1, 3, 5, /*normal => */ 1},
    {5, 3, 7, /*normal => */ 1},
    // -x 0246
    {0, 4, 2, /*normal => */ 0},
    {4, 6, 2, /*normal => */ 0},

    // +y 2367
    {2, 6, 3, /*normal => */ 3},
    {6, 7, 3, /*normal => */ 3},
    // -y 0145
    {0, 1, 4, /*normal => */ 2},
    {4, 1, 5, /*normal => */ 2},
  }};

  for (Indices& index : indices) {
    vertices.push_back({tmpVertices[index[0]], normals[index[3]]});
    vertices.push_back({tmpVertices[index[1]], normals[index[3]]});
    vertices.push_back({tmpVertices[index[2]], normals[index[3]]});
  }
}

} // namespace MakeGeometries

} // namespace graphic
} // namespace gero
