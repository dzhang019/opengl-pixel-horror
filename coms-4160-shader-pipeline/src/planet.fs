// Generate a procedural planet and orbiting moon. Use layers of (improved)
// Perlin noise to generate planetary features such as vegetation, gaseous
// clouds, mountains, valleys, ice caps, rivers, oceans. Don't forget about the
// moon. Use `animation_seconds` in your noise input to create (periodic)
// temporal effects.
//
// Uniforms:
uniform mat4 view;
uniform mat4 proj;
uniform float animation_seconds;
uniform bool is_moon;
// Inputs:
in vec3 sphere_fs_in;
in vec3 normal_fs_in;
in vec4 pos_fs_in; 
in vec4 view_pos_fs_in; 
// Outputs:
out vec3 color;
// expects: model, blinn_phong, bump_height, bump_position,
// improved_perlin_noise, tangent

float bump_height_max(vec3 s, out int winner)
{
    float h1 = improved_perlin_noise(5.0 * s);
    float h2 = improved_perlin_noise(0.5 * s + vec3(0.0, animation_seconds, 0.0));

    if (h1 >= h2) {
        winner = 1;
        return h1;
    } else {
        winner = 2;
        return h2;
    }
}

vec3 bump_position_max(bool is_moon, vec3 s, out int winner)
{
    float h = bump_height_max(s, winner);
    vec3 n = normalize(s);
    return s + h * n;
}

void main()
{
  /////////////////////////////////////////////////////////////////////////////
  // Replace with your code 
  vec3 s_obj = normalize(sphere_fs_in);
  //tangent frame
  vec3 T_obj, B_obj;
  tangent(s_obj, T_obj, B_obj);

  //calculate bumped position and bumped normal using partial derivatives
  float eps = 0.002;
  vec3 sT = normalize(s_obj + eps * T_obj);
  vec3 sB = normalize(s_obj + eps * B_obj);

  int w0, wx, wy;
  vec3 P0_obj = bump_position_max(is_moon, s_obj, w0);
  vec3 Px_obj = bump_position_max(is_moon, sT, wx);
  vec3 Py_obj = bump_position_max(is_moon, sB, wy);
  vec3 N_obj_bump = normalize(cross(Px_obj - P0_obj, Py_obj - P0_obj));

  mat4 M = model(is_moon, animation_seconds);
  mat3 Nmat = transpose(inverse(mat3(view * M)));
  vec3 N = normalize(Nmat * N_obj_bump);

  vec3 P = view_pos_fs_in.xyz;
  vec3 V = normalize(-P);

  float omega  = 2.0 * M_PI / 1.0;
  float theta  = animation_seconds * omega;
  float radius = 2.5;

  vec3 light_pos_world = vec3(radius * cos(theta), 2.0, radius * sin(theta));
  vec3 Lpos = (view * vec4(light_pos_world, 1.0)).xyz;
  vec3 L    = normalize(Lpos - P);

  vec3 ka = vec3(0.1);
  vec3 ks = vec3(0.60);
  vec3 kd = vec3(1.0);
  float p = 400.0;
  
  float lat = clamp(sphere_fs_in.y, -1.0, 1.0);
  float t = 0.5 * (lat + 1.0);

  float freq = 10.0;

  float n = improved_perlin_noise(8.0 * sphere_fs_in);
  t += 0.1 * n;

  float rings = 0.5 + 0.5 * sin(2.0 * M_PI * freq * t);

  vec3 cLo = vec3(0.05, 0.05, 0.2);
  vec3 cHi = vec3(0.2, 0.2, 1.0);
  kd = mix(cLo, cHi, rings);

  if (is_moon) {
    kd = kd + vec3(0.0, 1.0, 0.0);
    if (w0 == 1) {
        kd = kd + vec3(0.2, 0.8, 0.2);
    } else {
        kd = kd + vec3(0.8, 0.8, 0.8);
    }
  } else {
    kd = kd + vec3(0.5, 0.0, 0.0);
    if (w0 == 1) {
        kd = kd + vec3(0.1, 0.2, 0.2);
    } else {
        kd = kd + vec3(0.1, 0.6, 0.3);
    }
  }
  color = blinn_phong(ka, kd, ks, p, N, V, L);
  /////////////////////////////////////////////////////////////////////////////
}
