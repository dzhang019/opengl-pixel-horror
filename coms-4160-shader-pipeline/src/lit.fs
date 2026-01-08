// Add (hard code) an orbiting (point or directional) light to the scene. Light
// the scene using the Blinn-Phong Lighting Model.
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
// expects: PI, blinn_phong
void main()
{
  /////////////////////////////////////////////////////////////////////////////
  // Replace with your code 
  vec3 N = normalize(normal_fs_in);
  vec3 P = view_pos_fs_in.xyz;
  vec3 V = normalize(-P);

  float omega  = 0.000001 * M_PI;
  float theta  = animation_seconds * omega;
  float radius = 100;

  vec3 light_pos_world = vec3(radius * cos(theta), 100.0, radius * sin(theta));
  vec3 Lpos = (view * vec4(light_pos_world, 1.0)).xyz;
  vec3 L    = normalize(Lpos - P);


  //material
  vec3 ka = vec3(0.1);
  vec3 ks = vec3(0.60);
  vec3 kd = vec3(1.0);
  float p = 400.0;
  if (is_moon) {
    kd = vec3(0.5, 0.5, 0.5);
  } else {
    kd = vec3(0.2, 0.2, 1.0);
  }
  color = blinn_phong(ka, kd, ks, p, N, V, L);
  /////////////////////////////////////////////////////////////////////////////
}
