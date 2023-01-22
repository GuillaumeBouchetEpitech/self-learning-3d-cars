
#include "geronimo/helpers/GLMath.hpp"

#include <vector>

namespace gero {
namespace graphic {

namespace MakeGeometries {

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};

using Vertices = std::vector<Vertex>;

void makeShpere(Vertices& vertices, uint32_t quality, float radius);

void makeDisk(Vertices& vertices, uint32_t quality, float radiusIn,
              float radiusOut);

void makePartialDisk(Vertices& vertices, uint32_t quality, float radiusIn,
                     float radiusOut, float startAngle, float sweepAngle);

void makeCylinder(Vertices& vertices, uint32_t quality, float radiusTop,
                  float radiusBottom, float height, bool closedTop,
                  bool closedBottom);

void makePartialCylinder(Vertices& vertices, uint32_t quality, float radiusTop,
                         float radiusBottom, float height, bool closedTop,
                         bool closedBottom, float startAngle, float sweepAngle);

void makeTorus(Vertices& vertices, uint32_t ringQuality, uint32_t tubeQuality,
               float ringRadius, float tubeRadius);

void makePartialTorus(Vertices& vertices, uint32_t ringQuality,
                      uint32_t tubeQuality, float ringRadius, float tubeRadius,
                      bool closedStart, bool closedEnd, float startAngle,
                      float sweepAngle);

void makeBox(Vertices& vertices, float sizeX, float sizeY, float sizeZ);

void executeTransform(const glm::mat4& transform, Vertices& vertices);

void convertToPerFacesNormals(Vertices& vertices);

} // namespace MakeGeometries

} // namespace graphic
} // namespace gero
