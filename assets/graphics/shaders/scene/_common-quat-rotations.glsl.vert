
vec4 quat_from_axis_angle(vec3 axis, float angle)
{
  vec4 qr;
  // float half_angle = (angle * 0.5) * 3.14159 / 180.0;
  float half_angle = (angle * 0.5);
  qr.x = axis.x * sin(half_angle);
  qr.y = axis.y * sin(half_angle);
  qr.z = axis.z * sin(half_angle);
  qr.w = cos(half_angle);
  return qr;
}

// vec4 quat_conj(vec4 q)
// {
//   return vec4(-q.x, -q.y, -q.z, q.w);
// }

// vec4 quat_mult(vec4 q1, vec4 q2)
// {
//   vec4 qr;
//   qr.x = (q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y);
//   qr.y = (q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x);
//   qr.z = (q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w);
//   qr.w = (q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z);
//   return qr;
// }

// vec3 rotate_vec3(vec3 position, vec3 axis, float angle)
// {
//   vec4 qr = quat_from_axis_angle(axis, angle);
//   vec4 qr_conj = quat_conj(qr);
//   vec4 q_pos = vec4(position.x, position.y, position.z, 0);

//   vec4 q_tmp = quat_mult(qr, q_pos);
//   qr = quat_mult(q_tmp, qr_conj);

//   return vec3(qr.x, qr.y, qr.z);
// }

// vec3 rotate_vec3(vec3 position, vec3 axis, float angle)
// {
//   vec4 q = quat_from_axis_angle(axis, angle);
//   vec3 v = position.xyz;
//   return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
// }

vec3 apply_quat_to_vec3(vec3 position, vec4 q)
{
  vec3 v = position.xyz;
  return v + 2.0 * cross(q.xyz, cross(q.xyz, v) + q.w * v);
}
