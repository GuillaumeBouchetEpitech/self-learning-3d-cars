
vec4 quat_from_axis_angle(vec3 axis, float angle)
{
  vec4 qr;
  float half_angle = (angle * 0.5);
  float sin_val = sin(half_angle);
  qr.x = axis.x * sin_val;
  qr.y = axis.y * sin_val;
  qr.z = axis.z * sin_val;
  qr.w = cos(half_angle);
  return qr;
}

vec3 apply_quat_to_vec3(vec3 pos, float angle, vec3 axis)
{
  return pos + 2.0 * cross(axis, cross(axis, pos) + angle * pos);
}
