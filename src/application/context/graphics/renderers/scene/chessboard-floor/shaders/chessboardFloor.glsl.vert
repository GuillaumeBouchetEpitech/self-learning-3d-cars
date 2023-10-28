#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_vertex_position;
in vec3 a_vertex_normal;
in vec2 a_vertex_texCoord;

out vec2 v_texCoord;
out vec3 v_worldSpaceNormal;
out vec3 v_worldSpacePosition;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_vertex_position, 1.0);

	v_texCoord = a_vertex_texCoord;
	v_worldSpaceNormal = a_vertex_normal;
	v_worldSpacePosition = a_vertex_position;
}
