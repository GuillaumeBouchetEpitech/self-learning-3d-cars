#version 300 es

precision lowp float;

uniform sampler2D u_texture;
uniform vec3 u_lightPos;

const float k_ambiantCoef = 0.2;

in vec2 v_texCoord;
in vec3 v_worldSpaceNormal;
in vec3 v_worldSpacePosition;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

#include "assets/graphics/shaders/scene/_common-apply-lighting.glsl.frag"

void main(void)
{
  vec4 texColor = texture(u_texture, v_texCoord);

  out_color = applyLighting_no_specular(
    u_lightPos,
    texColor,
    v_worldSpaceNormal,
    v_worldSpacePosition,
    k_ambiantCoef
  );

  out_outline = vec4(0.0);
}
