#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3  a_vertex_position;

in vec3  a_offset_position;
in float a_offset_scale;
in vec3  a_offset_color;

out vec4 v_color;

void main(void)
{
	vec3 position = a_vertex_position * a_offset_scale + a_offset_position;

	gl_Position = u_composedMatrix * vec4(position, 1.0);

	v_color = vec4(a_offset_color, 1.0);
}
