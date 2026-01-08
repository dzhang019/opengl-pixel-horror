// --- PROTOTYPES ADDED ---
vec3 random_direction(vec3 p);
vec3 improved_smooth_step(vec3 f);
// ------------------------

float improved_perlin_noise( vec3 st) 
{
  vec3 p0 = floor(st);
  vec3 loc = fract(st);

  vec3 g000 = random_direction(p0 + vec3(0.0,0.0,0.0));
  vec3 g001 = random_direction(p0 + vec3(0.0,0.0,1.0));
  vec3 g010 = random_direction(p0 + vec3(0.0,1.0,0.0));
  vec3 g100 = random_direction(p0 + vec3(1.0,0.0,0.0));
  vec3 g011 = random_direction(p0 + vec3(0.0,1.0,1.0));
  vec3 g111 = random_direction(p0 + vec3(1.0,1.0,1.0));
  vec3 g101 = random_direction(p0 + vec3(1.0,0.0,1.0));
  vec3 g110 = random_direction(p0 + vec3(1.0,1.0,0.0));

  vec3 d000 = loc - vec3(0.0,0.0,0.0);
  vec3 d001 = loc - vec3(0.0,0.0,1.0);
  vec3 d010 = loc - vec3(0.0,1.0,0.0);
  vec3 d100 = loc - vec3(1.0,0.0,0.0);
  vec3 d011 = loc - vec3(0.0,1.0,1.0);
  vec3 d111 = loc - vec3(1.0,1.0,1.0);
  vec3 d101 = loc - vec3(1.0,0.0,1.0);
  vec3 d110 = loc - vec3(1.0,1.0,0.0);

  float c000 = dot(g000, d000);
  float c001 = dot(g001, d001);
  float c010 = dot(g010, d010);
  float c100 = dot(g100, d100);
  float c011 = dot(g011, d011);
  float c111 = dot(g111, d111);
  float c101 = dot(g101, d101);
  float c110 = dot(g110, d110);

  // Note: Using the vec3 version of smooth_step here
  vec3 u = improved_smooth_step(loc);

  float nx00 = mix(c000, c100, u.x);
  float nx10 = mix(c010, c110, u.x);
  float nx01 = mix(c001, c101, u.x);
  float nx11 = mix(c011, c111, u.x);

  float nxy0 = mix(nx00, nx10, u.y);
  float nxy1 = mix(nx01, nx11, u.y);

  float nxyz = mix(nxy0, nxy1, u.z);
  return nxyz;
}