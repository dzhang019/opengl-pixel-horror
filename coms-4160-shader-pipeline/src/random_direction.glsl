vec2 random2(vec2 st);
vec2 random2(vec3 st);

vec3 random_direction( vec3 seed)
{
  vec2 u = random2(seed);
  
  vec2 v = random2(seed.yz + u*17.0);

  float theta = 2.0 * M_PI * u.x;
  float z     = 2.0 * v.y - 1.0;
  float r     = sqrt(max(0.0, 1.0 - z*z));

  return vec3(r * cos(theta), r * sin(theta), z);
}