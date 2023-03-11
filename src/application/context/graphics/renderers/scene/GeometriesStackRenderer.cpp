
#include "GeometriesStackRenderer.hpp"

#include "application/context/Context.hpp"
#include "application/context/graphics/graphicsAliases.hpp"

namespace {

glm::quat quat_from_axis_angle(const glm::vec3& axis, float angle)
{
  glm::quat qr;
  // float half_angle = (angle * 0.5) * 3.14159 / 180.0;
  const float half_angle = (angle * 0.5f);
  const float sin_val = std::sin(half_angle);
  qr.x = axis.x * sin_val;
  qr.y = axis.y * sin_val;
  qr.z = axis.z * sin_val;
  qr.w = std::cos(half_angle);
  return qr;
}

glm::vec3 apply_quat_to_vec3(const glm::vec3& position, const glm::quat& q)
{
  const glm::vec3 axis(q.x, q.y, q.z);
  return position + 2.0f * glm::cross(axis, glm::cross(axis, position) + q.w * position);
}

}

void GeometriesStackRenderer::initialize()
{
  auto& resourceManager = Context::get().graphic.resourceManager;

  _shader = resourceManager.getShader(gero::asValue(ShadersAliases::geometriesStackRenderer));
}

void
GeometriesStackRenderer::setMatricesData(const gero::graphics::Camera:: MatricesData& matricesData) {
  _matricesData = matricesData;
}

void GeometriesStackRenderer::createAlias(int32_t alias, const gero::graphics::MakeGeometries::Vertices& vertices)
{
  auto& resourceManager = Context::get().graphic.resourceManager;

  auto newAlias = std::make_shared<AliasedGeometry>();

  auto geoDef = resourceManager.getGeometryDefinition(gero::asValue(GeometriesAliases::geometriesStackRenderer));
  newAlias->geometry.initialize(*_shader, geoDef);
  newAlias->geometry.updateBuffer(0, vertices, false);
  newAlias->geometry.setPrimitiveStart(0);
  newAlias->geometry.setPrimitiveCount(uint32_t(vertices.size()));

  newAlias->instanceVertices.reserve(256); // pre-allocate

  _aliasedGeometriesMap[alias] = newAlias;
}

void GeometriesStackRenderer::deleteAlias(int32_t alias)
{
  auto it = _aliasedGeometriesMap.find(alias);
  if (it == _aliasedGeometriesMap.end())
    D_THROW(std::runtime_error, "alias not found: " << alias);

  _aliasedGeometriesMap.erase(it);
}
void GeometriesStackRenderer::clearAlias(int32_t alias)
{
  auto it = _aliasedGeometriesMap.find(alias);
  if (it == _aliasedGeometriesMap.end())
    D_THROW(std::runtime_error, "alias not found: " << alias);

  it->second->instanceVertices.clear();
}

void GeometriesStackRenderer::pushAlias(int32_t alias, const GeometryInstance& newInstance, float inForwardOffset /*= 0.0f*/)
{
  auto it = _aliasedGeometriesMap.find(alias);
  if (it == _aliasedGeometriesMap.end())
    D_THROW(std::runtime_error, "alias not found: " << alias);

  GeometryInstance copyInstance = newInstance;

  glm::vec3 axis = glm::vec3(copyInstance.orientation.x, copyInstance.orientation.y, copyInstance.orientation.z);
  copyInstance.orientation = quat_from_axis_angle(axis, copyInstance.orientation.w);

  const glm::vec3 dir = apply_quat_to_vec3(glm::vec3(1,0,0), copyInstance.orientation);

  copyInstance.position += dir * inForwardOffset;

  it->second->instanceVertices.push_back(copyInstance);
}

void GeometriesStackRenderer::clearAll()
{
  _aliasedGeometriesMap.clear();
}

// void GeometriesStackRenderer::renderAll(const glm::vec3& inLightPos)
void GeometriesStackRenderer::renderAll()
{
  if (!_shader)
    D_THROW(std::runtime_error, "not initialized");

  if (_aliasedGeometriesMap.empty())
    return;

  _shader->bind();
  _shader->setUniform("u_composedMatrix", _matricesData.composed);
  // _shader->setUniform("u_ambiantCoef", 0.2f);
  // _shader->setUniform("u_lightPos", inLightPos.x, inLightPos.y, inLightPos.z);

  for (const auto& pair : _aliasedGeometriesMap)
  {
    auto& vertices = pair.second->instanceVertices;
    if (vertices.empty())
      continue;

    // D_MYLOG("step");

    auto& geometry = pair.second->geometry;

    geometry.updateBuffer(1, vertices, true);
    geometry.setInstancedCount(uint32_t(vertices.size()));
    geometry.render();

    vertices.clear();
  }

}
