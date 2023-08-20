#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;
uniform float u_alpha;
uniform float u_lowerLimit;
uniform float u_upperLimit;
uniform vec3 u_lightPos;

const vec4 k_white = vec4(1.0);
const float k_ambiantCoef = 0.4;

in vec3 a_vertex_position;
in vec3 a_vertex_color;
in vec3 a_vertex_normal;
in vec3 a_vertex_animatedNormal;
in float a_vertex_index;

out vec4 v_color;

#include "assets/graphics/shaders/_common/apply-lighting.glsl.frag"

void main(void)
{
	float deformationCoef = 0.0;

	if (a_vertex_index <= u_lowerLimit)
	{
		// not deformed (normal shape)
		deformationCoef = 0.0;
	}
	else if (a_vertex_index > u_upperLimit)
	{
		// fully deformed (shattered shape)
		deformationCoef = 1.0;
	}
	else
	{
		// animated deformation (normal shape <-> shattered shape)
		deformationCoef = (a_vertex_index - u_lowerLimit) / (u_upperLimit - u_lowerLimit);
	}

	vec3 position = a_vertex_position;

	if (deformationCoef == 1.0)
	{
		// invisible
		v_color = vec4(0.0);
	}
	else
	{
		float diffuseLightRatio = getDiffuseLightingRatio(
			u_lightPos,
			a_vertex_normal,
			position
		);

		vec4 defaultColor = vec4(a_vertex_color * (k_ambiantCoef + diffuseLightRatio), u_alpha);

		// modified color (normal color <-> white color)
		v_color = mix(defaultColor, k_white, deformationCoef);

		// modified shape (normal shape <-> shattered shape)
		position -= a_vertex_animatedNormal * deformationCoef;
	}

	gl_Position = u_composedMatrix * vec4(position, 1.0);
}
