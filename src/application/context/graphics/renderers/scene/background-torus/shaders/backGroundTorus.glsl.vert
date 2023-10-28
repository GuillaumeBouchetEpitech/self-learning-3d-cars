#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;
uniform float u_animationCoef;

in vec3 a_vertex_position;
in vec2 a_vertex_texCoord;

out vec2 v_texCoord;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_vertex_position, 1.0);
	v_texCoord = vec2(a_vertex_texCoord.x + u_animationCoef, a_vertex_texCoord.y);
}
