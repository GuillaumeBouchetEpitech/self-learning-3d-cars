#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;
uniform float u_alpha;
uniform float u_lowerLimit;
uniform float u_upperLimit;
uniform vec3 u_lightPos;

const vec3 k_white = vec3(1.0);
const float k_ambiantCoef = 0.2;

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

	vec4 currColor;

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
		currColor = vec4(0.0);
	}
	else
	{
		// modified color (normal color <-> white color)
		currColor = vec4(mix(a_vertex_color, k_white, deformationCoef), u_alpha);

		// modified shape (normal shape <-> shattered shape)
		position -= a_vertex_animatedNormal * deformationCoef;
	}

	gl_Position = u_composedMatrix * vec4(position, 1.0);

  float diffuseLightRatio = getDiffuseLightingRatio(
    u_lightPos,
    a_vertex_normal,
    position
  );

	v_color = currColor * (k_ambiantCoef + diffuseLightRatio);
}
