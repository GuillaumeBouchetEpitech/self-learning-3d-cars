#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_vertex_position;
in vec3 a_vertex_color;

in vec3 a_offset_position;
in vec4 a_offset_orientation; // quaternion
in vec3 a_offset_scale;
in vec4 a_offset_color;
in vec4 a_offset_outlineColor;

out vec4 v_color;
out vec4 v_outlineColor;

#include "./assets/graphics/shaders/scene/_common-quat-rotations.glsl.vert"

void main(void)
{
	vec3 worldSpacePosition = a_offset_position + apply_quat_to_vec3(a_vertex_position * a_offset_scale, a_offset_orientation);

	gl_Position = u_composedMatrix * vec4(worldSpacePosition, 1.0);

	v_color = vec4(a_vertex_color * a_offset_color.rgb, a_offset_color.a);
	v_outlineColor = a_offset_outlineColor;
}
