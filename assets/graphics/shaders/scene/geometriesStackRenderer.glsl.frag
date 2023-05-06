#version 300 es

precision lowp float;

// uniform float u_ambiantCoef;
// uniform vec3 u_lightPos;

// in vec3 v_worldSpacePosition;
// in vec3 v_worldSpaceNormal;
flat in vec4 v_color;
flat in float v_outlineValue;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

// #include "./assets/graphics/shaders/_common/apply-lighting.glsl.frag"

void main(void)
{
	out_color = v_color;
	// out_color = applyLighting(
	// 	u_lightPos,
	// 	v_color,
	// 	v_worldSpaceNormal,
	// 	v_worldSpacePosition,
	// 	u_ambiantCoef
	// );

    out_outline = vec4(v_outlineValue, 0.0, 0.0, 1.0);
}