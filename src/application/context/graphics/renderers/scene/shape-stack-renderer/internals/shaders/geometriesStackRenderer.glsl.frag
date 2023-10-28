#version 300 es

precision lowp float;

flat in vec4 v_color;
flat in float v_outlineValue;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

void main(void)
{
	out_color = v_color;
	out_outline = vec4(v_outlineValue, 0.0, 0.0, v_outlineValue);
}
