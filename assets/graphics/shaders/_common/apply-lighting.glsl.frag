
float getDiffuseLightingRatio(vec3 lightPos, vec3 normal, vec3 worldPos)
{
  normal = normalize(normal);
  vec3 lightDir = normalize(lightPos - worldPos);

  return max(dot(lightDir, normal), 0.0);
}
