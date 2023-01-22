#version 300 es

precision lowp float;

uniform sampler2D u_colorTexture;
uniform sampler2D u_outlineTexture;
uniform vec2 u_invResolution;

in vec2 v_texCoord;

out vec4 out_color;

void main(void)
{
  vec4 pixelColor = texture(u_colorTexture, v_texCoord);
  vec4 pixelOutline = texture(u_outlineTexture, v_texCoord);

  if (pixelOutline.x == 0.0)
  {
    float depth = 0.0;

    for (int yy = -1; yy <= +1; ++yy)
    for (int xx = -1; xx <= +1; ++xx)
    {
      vec2 offset = vec2(float(xx), float(yy)) * u_invResolution;
      depth += texture(u_outlineTexture, v_texCoord + offset).x;
    }

    const vec4 outlineColor = vec4(0.8, 0.8, 0.8, 1.0);

    if (depth > 1.0)
      out_color = outlineColor;
    else
      out_color = pixelColor;
  }
  else
  {
    out_color = pixelColor;
  }
}
