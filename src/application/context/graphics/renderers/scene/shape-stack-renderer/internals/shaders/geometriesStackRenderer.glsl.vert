#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_vertex_position;

in vec3 a_offset_position;
in vec4 a_offset_orientation; // quaternion
in vec3 a_offset_scale;
in vec4 a_offset_color;
in float a_offset_outlineValue;

flat out vec4 v_color;
flat out float v_outlineValue;

vec3 apply_quat_to_vec3(vec3 position, vec4 q)
{
  vec3 v = position.xyz;
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}

void main(void)
{
	vec3 worldSpacePosition = a_offset_position;
	worldSpacePosition += apply_quat_to_vec3(a_vertex_position * a_offset_scale, a_offset_orientation);

	gl_Position = u_composedMatrix * vec4(worldSpacePosition, 1.0);

	v_color = a_offset_color;
	v_outlineValue = a_offset_outlineValue;
}
