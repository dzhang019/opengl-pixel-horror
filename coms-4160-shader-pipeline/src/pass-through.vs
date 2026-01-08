// INPUTS (Match C++ Object.cpp)
layout(location = 0) in vec3 pos_vs_in;    // Position (vec3)
layout(location = 1) in vec3 normal_vs_in; // Normal
layout(location = 2) in vec2 tex_coord_vs_in; // UV

// OUTPUTS (To TCS)
out vec3 pos_cs_in;       // <--- MUST BE VEC3
out vec3 normal_cs_in;
out vec2 tex_coord_cs_in;

void main()
{
  pos_cs_in = pos_vs_in;
  normal_cs_in = normal_vs_in;
  tex_coord_cs_in = tex_coord_vs_in;
}