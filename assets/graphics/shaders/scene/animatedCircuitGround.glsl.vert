#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;
uniform float u_alpha;
uniform float u_lowerLimit;
uniform float u_upperLimit;

in vec3 a_vertex_position;
in vec3 a_vertex_color;
in vec3 a_vertex_normal;
in vec3 a_vertex_animatedNormal;
in float a_vertex_index;

out vec4 v_color;
out vec3 v_worldSpaceNormal;
out vec3 v_worldSpacePosition;

const vec3 k_white = vec3(1.0);

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
		// modified color (normal color <-> white color)
		v_color = vec4(mix(a_vertex_color, k_white, deformationCoef), u_alpha);

		// modified shape (normal shape <-> shattered shape)
		position -= a_vertex_animatedNormal * deformationCoef;
	}

	v_worldSpaceNormal = a_vertex_normal;
	v_worldSpacePosition = position;
	gl_Position = u_composedMatrix * vec4(position, 1.0);
}
