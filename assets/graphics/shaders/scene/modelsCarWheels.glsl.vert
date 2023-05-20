#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_vertex_position;
in vec3 a_vertex_color;

in vec3 a_offset_position;
in vec4 a_offset_orientation; // quaternion
in vec3 a_offset_scale;
in vec3 a_offset_color;

out vec3 v_color;

#include "./assets/graphics/shaders/_common/quat-rotations.glsl.vert"

void main(void)
{
	float angle = a_offset_orientation.w;
	vec3 axis = a_offset_orientation.xyz;

	vec3 worldSpacePosition = a_offset_position;
	worldSpacePosition += apply_quat_to_vec3(a_vertex_position * a_offset_scale, angle, axis);

	gl_Position = u_composedMatrix * vec4(worldSpacePosition, 1.0);

	v_color = a_vertex_color * a_offset_color;
}
