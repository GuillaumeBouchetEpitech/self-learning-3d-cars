#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec2 a_vertex_position;
in vec2 a_vertex_texCoord;
in vec3 a_offset_position;
in vec2 a_offset_texCoord;
in vec4 a_offset_color;
in float a_offset_scale;

out vec2 v_texCoord;
flat out vec4 v_color;

void main(void)
{
	vec2 position = a_vertex_position * a_offset_scale + a_offset_position.xy;

	gl_Position = u_composedMatrix * vec4(position, a_offset_position.z, 1.0);

	v_texCoord = a_vertex_texCoord + a_offset_texCoord;
	v_color = a_offset_color;
}
