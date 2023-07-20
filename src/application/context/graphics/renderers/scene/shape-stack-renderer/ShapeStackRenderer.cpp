
#include "ShapeStackRenderer.hpp"


void ShapeStackRenderer::initialize()
{
  _geometriesStackRenderer.initialize();

  {
    //
    // sphere geometry
    //

    gero::graphics::MakeGeometries::Vertices vertices;
    gero::graphics::MakeGeometries::makeSphere(vertices, 0, 0.5f);
    gero::graphics::MakeGeometries::convertToPerFacesNormals(vertices);

    _geometriesStackRenderer.createAlias(2222, vertices);
  }

  {
    //
    // sphere geometry
    //

    gero::graphics::MakeGeometries::Vertices vertices;
    gero::graphics::MakeGeometries::makeSphere(vertices, 1, 0.5f);
    gero::graphics::MakeGeometries::convertToPerFacesNormals(vertices);

    _geometriesStackRenderer.createAlias(2223, vertices);
  }

  {
    //
    // box geometry
    //

    gero::graphics::MakeGeometries::Vertices vertices;
    gero::graphics::MakeGeometries::makeBox(vertices, 1, 1, 1);

    _geometriesStackRenderer.createAlias(3333, vertices);
  }

}

void ShapeStackRenderer::pushSphere(const GeometriesStackRenderer::GeometryInstance& instance)
{
  _geometriesStackRenderer.pushAlias(2222, instance);
}
void ShapeStackRenderer::pushSuperiorSphere(const GeometriesStackRenderer::GeometryInstance& instance)
{
  _geometriesStackRenderer.pushAlias(2223, instance);
}

void ShapeStackRenderer::pushBox(const GeometriesStackRenderer::GeometryInstance& instance)
{
  _geometriesStackRenderer.pushAlias(3333, instance);
}

void ShapeStackRenderer::render(const gero::graphics::Camera& inCamera)
{
  _geometriesStackRenderer.renderAll(inCamera);
}



