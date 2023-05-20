#version 300 es

precision lowp float;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform float u_alphaValue;

const float k_ambiantCoef = 0.2;
const vec3 k_redColor = vec3(1.0, 1.0, 0.0);

flat in vec3 v_modelColor;
flat in vec3 v_offsetColor;
in float v_colorMixValue;
in vec3 v_worldSpacePosition;
in vec3 v_worldSpaceNormal;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

#include "assets/graphics/shaders/_common/apply-lighting.glsl.frag"

void main(void)
{
	vec3 blendColor = v_colorMixValue < 0.0 ? k_redColor : v_offsetColor.rgb;
	vec4 currColor = vec4(v_modelColor.rgb * blendColor, u_alphaValue);

  out_color = applyLighting(
    u_viewPos,
    u_lightPos,
    currColor,
    v_worldSpaceNormal,
    v_worldSpacePosition,
    k_ambiantCoef
  );
  out_outline = vec4(1.0, 0.0, 0.0, 1.0);
}
