#version 300 es

precision highp float;

uniform sampler2D u_texture;
uniform vec3 u_lightPos;

const float k_ambiantCoef = 0.2;

in vec2 v_texCoord;
in vec3 v_worldSpaceNormal;
in vec3 v_worldSpacePosition;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

float getDiffuseLightingRatio(vec3 lightPos, vec3 normal, vec3 worldPos)
{
  // normal = normalize(normal);
  vec3 lightDir = normalize(lightPos - worldPos);

  return max(dot(lightDir, normal), 0.2);
}

void main(void)
{
  vec4 texColor = texture(u_texture, v_texCoord);

  float diffuseLightRatio = getDiffuseLightingRatio(
    u_lightPos,
    v_worldSpaceNormal,
    v_worldSpacePosition
  );

  out_color = vec4(texColor.rgb * (k_ambiantCoef + diffuseLightRatio * 0.8), texColor.a);

  out_outline = vec4(0.0);
}
