

#include "MakeGeometries.hpp"

#include "geronimo/system/containers/static_heap_grid_array.hpp"
#include "geronimo/system/math/constants.hpp"

namespace gero {
namespace graphic {

namespace MakeGeometries {

void executeTransform(const glm::mat4& transform, Vertices& vertices) {
  for (Vertex& vertex : vertices) {
    vertex.position = transform * glm::vec4(vertex.position, 1.0f);
    vertex.normal =
      glm::normalize(glm::vec3(transform * glm::vec4(vertex.normal, 0.0f)));
  }
}

void convertToPerFacesNormals(Vertices& vertices) {
  for (std::size_t index = 0; index < vertices.size(); index += 3) {
    const glm::vec3& posA = vertices[index + 0].position;
    const glm::vec3& posB = vertices[index + 1].position;
    const glm::vec3& posC = vertices[index + 2].position;

    const glm::vec3 normal = glm::cross(posA - posB, posA - posC);

    vertices[index + 0].normal = normal;
    vertices[index + 1].normal = normal;
    vertices[index + 2].normal = normal;
  }
}

} // namespace MakeGeometries

} // namespace graphic
} // namespace gero
