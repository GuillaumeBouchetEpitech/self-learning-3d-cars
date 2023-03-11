#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_vertex_position;
// in vec3 a_vertex_normal;

in vec3 a_offset_position;
in vec4 a_offset_orientation; // quaternion
in vec3 a_offset_scale;
in vec4 a_offset_color;
in float a_offset_outlineValue;

// out vec3 v_worldSpacePosition;
// out vec3 v_worldSpaceNormal;
flat out vec4 v_color;
flat out float v_outlineValue;

#include "./assets/graphics/shaders/scene/_common-quat-rotations.glsl.vert"

void main(void)
{
	float angle = a_offset_orientation.w;
	vec3 axis = vec3(a_offset_orientation.x, a_offset_orientation.y, a_offset_orientation.z);

	vec3 worldSpacePosition = a_offset_position;
	worldSpacePosition += apply_quat_to_vec3(a_vertex_position * a_offset_scale, angle, axis);
	// vec3 worldSpaceNormal = apply_quat_to_vec3(a_vertex_normal, angle, axis);

	gl_Position = u_composedMatrix * vec4(worldSpacePosition, 1.0);

	// v_worldSpacePosition = worldSpacePosition;
	// v_worldSpaceNormal = worldSpaceNormal;
	v_color = a_offset_color;
	v_outlineValue = a_offset_outlineValue;
}
