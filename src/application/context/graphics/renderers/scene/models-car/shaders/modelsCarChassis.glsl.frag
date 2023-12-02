#version 300 es

precision lowp float;

uniform float u_alphaValue;

const vec3 k_yellowColor = vec3(1.0, 1.0, 0.0);
const vec3 k_redColor = vec3(1.0, 0.0, 0.0);

flat in vec3 v_modelColor;
flat in vec3 v_offsetColor;
in float v_colorMixValue;
in float v_diffuseLightRatio;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

void main(void)
{
	vec3 blendColor =
    v_colorMixValue < -0.05
      ? k_yellowColor
      : v_colorMixValue > +0.05
        ? v_offsetColor.rgb
        : k_redColor;

	vec4 currColor = vec4(v_modelColor.rgb * blendColor, u_alphaValue);

  out_color = vec4(currColor.rgb * v_diffuseLightRatio, u_alphaValue);

  out_outline = vec4(1.0, 1.0, 1.0, 1.0);
}
