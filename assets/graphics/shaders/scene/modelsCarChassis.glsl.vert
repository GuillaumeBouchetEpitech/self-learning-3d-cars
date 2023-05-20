#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

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
out vec3 v_worldSpacePosition;
out vec3 v_worldSpaceNormal;

#include "./assets/graphics/shaders/_common/quat-rotations.glsl.vert"

void main(void)
{
	float angle = a_offset_orientation.w;
	vec3 axis = vec3(a_offset_orientation.xyz);

	vec3 worldSpacePosition = a_offset_position;
	worldSpacePosition += apply_quat_to_vec3(a_vertex_position * a_offset_scale, angle, axis);
	vec3 worldSpaceNormal = apply_quat_to_vec3(a_vertex_normal, angle, axis);

	gl_Position = u_composedMatrix * vec4(worldSpacePosition, 1.0);

	v_modelColor = a_vertex_color;
	v_offsetColor = a_offset_color;
	v_colorMixValue = a_offset_life - a_vertex_lifeValue;
	v_worldSpacePosition = worldSpacePosition;
	v_worldSpaceNormal = worldSpaceNormal;
}
