#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;
uniform vec3 u_lightPos;

const float k_ambiantCoef = 0.2;

in vec3 a_vertex_position;
in vec3 a_vertex_color;
in vec3 a_vertex_normal;
in float a_vertex_lifeValue;

in vec3 a_offset_position;
in vec4 a_offset_orientation; // quaternion
in vec3 a_offset_scale;
in vec3 a_offset_color;
in float a_offset_life;

flat out vec3 v_modelColor;
flat out vec3 v_offsetColor;
out float v_colorMixValue;
out float v_diffuseLightRatio;

vec3 apply_quat_to_vec3(vec3 position, vec4 q)
{
  vec3 v = position.xyz;
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

float getDiffuseLightingRatio(vec3 lightPos, vec3 normal, vec3 worldPos)
{
  normal = normalize(normal);
  vec3 lightDir = normalize(lightPos - worldPos);

  return max(dot(lightDir, normal), 0.0);
}

void main(void)
{
	vec3 worldSpacePosition = a_offset_position;
	worldSpacePosition += apply_quat_to_vec3(a_vertex_position * a_offset_scale, a_offset_orientation);
	vec3 worldSpaceNormal = apply_quat_to_vec3(a_vertex_normal, a_offset_orientation);

	gl_Position = u_composedMatrix * vec4(worldSpacePosition, 1.0);

	v_modelColor = a_vertex_color;
	v_offsetColor = a_offset_color;
	v_colorMixValue = a_offset_life - a_vertex_lifeValue;

  v_diffuseLightRatio = k_ambiantCoef + getDiffuseLightingRatio(
    u_lightPos,
    worldSpaceNormal,
    worldSpacePosition
  );
}
