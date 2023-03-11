
const vec3 k_specColor = vec3(1.0, 1.0, 1.0);

vec4 applyLighting(vec3 lightPos, vec4 currentColor, vec3 normal, vec3 worldPos, float ambiantCoef)
{
  normal = normalize(normal);
  vec3 lightDir = normalize(lightPos - worldPos);

  float diffuseCoef = max(dot(lightDir, normal.xyz), 0.0);
  float specularCoef = 0.0;

  if (diffuseCoef > 0.0)
  {
    // lighting specular

    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 viewDir = normalize(-worldPos);

    float specAngle = max(dot(reflectDir, viewDir), 0.0);
    specularCoef = pow(specAngle, 64.0);
  }

  // lighting output

  vec3 ambiantColor = currentColor.xyz * ambiantCoef;
  vec3 diffuseColor = currentColor.xyz * diffuseCoef;
  vec3 specularColor = k_specColor * specularCoef;

  return vec4(ambiantColor + diffuseColor + specularColor, currentColor.a);
}

vec4 applyLighting_no_specular(vec3 lightPos, vec4 currentColor, vec3 normal, vec3 worldPos, float ambiantCoef)
{
  normal = normalize(normal);
  vec3 lightDir = normalize(lightPos - worldPos);

  float diffuseCoef = max(dot(lightDir, normal.xyz), 0.0);
  float specularCoef = 0.0;

  // if (diffuseCoef > 0.0)
  // {
  //   // lighting specular

  //   vec3 reflectDir = reflect(-lightDir, normal);
  //   vec3 viewDir = normalize(-worldPos);

  //   float specAngle = max(dot(reflectDir, viewDir), 0.0);
  //   specularCoef = pow(specAngle, 64.0);
  // }

  // lighting output

  vec3 ambiantColor = currentColor.xyz * ambiantCoef;
  vec3 diffuseColor = currentColor.xyz * diffuseCoef;
  vec3 specularColor = k_specColor * specularCoef;

  return vec4(ambiantColor + diffuseColor + specularColor, currentColor.a);
}
