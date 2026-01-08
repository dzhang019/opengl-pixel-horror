// Set the pixel color using Blinn-Phong shading (e.g., with constant blue and
// gray material color) with a bumpy texture.
// 
// Uniforms:
uniform mat4 view;
uniform mat4 proj;
uniform float animation_seconds;
uniform bool is_moon;
// Inputs:
//                     linearly interpolated from tessellation evaluation shader
//                     output
in vec3 sphere_fs_in;
in vec3 normal_fs_in;
in vec4 pos_fs_in; 
in vec4 view_pos_fs_in; 
// Outputs:
//               rgb color of this pixel
out vec3 color;
// expects: model, blinn_phong, bump_height, bump_position,
// improved_perlin_noise, tangent
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

  vec3 P0_obj = bump_position(is_moon, s_obj);
  vec3 Px_obj = bump_position(is_moon, sT);
  vec3 Py_obj = bump_position(is_moon, sB);
  vec3 N_obj_bump = normalize(cross(Px_obj - P0_obj, Py_obj - P0_obj));

  mat4 M = model(is_moon, animation_seconds);
  mat3 Nmat = transpose(inverse(mat3(view * M)));
  vec3 N = normalize(Nmat * N_obj_bump);

  vec3 P = view_pos_fs_in.xyz;
  vec3 V = normalize(-P);

  float omega  = 2.0 * M_PI / 8.0;
  float theta  = animation_seconds * omega;
  float radius = 2.5;

  vec3 light_pos_world = vec3(radius * cos(theta), 2.0, radius * sin(theta));
  vec3 Lpos = (view * vec4(light_pos_world, 1.0)).xyz;
  vec3 L    = normalize(Lpos - P);

  vec3 ka = vec3(0.1);
  vec3 ks = vec3(0.60);
  vec3 kd = is_moon ? vec3(0.5) : vec3(0.2, 0.2, 1.0);
  float p = 400.0;

  color = blinn_phong(ka, kd, ks, p, N, V, L);
  /////////////////////////////////////////////////////////////////////////////
}
