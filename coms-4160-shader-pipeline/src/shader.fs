uniform mat4 view;
uniform mat4 proj;

uniform vec3 light_position; 
uniform vec3 light_color;
uniform vec3 ambient_light;

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float p;
uniform int id;

uniform sampler2D tex_map;
uniform bool use_texture;


in vec4 pos_fs_in; 
in vec4 view_pos_fs_in; 
in vec3 normal_fs_in; 
in vec2 vTexCoord;
in vec3 pos_obj_fs_in;


out vec3 color;

float improved_perlin_noise(vec3 s);
float sand_height(vec3 s) {
    float grain = improved_perlin_noise(s * 50.0); 
    return grain * 0.5;
}


const float bayerMatrix[16] = float[](
    0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
    12.0/16.0, 4.0/16.0, 14.0/16.0,  6.0/16.0,
    3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
    15.0/16.0, 7.0/16.0, 13.0/16.0,  5.0/16.0
);

void main()
{

    vec3 object_color;
    if (use_texture) {
        object_color = texture(tex_map, vTexCoord).rgb;
    } else {
        object_color = kd; 
    }

    vec3 N = normalize(normal_fs_in);
    vec3 P = view_pos_fs_in.xyz;

    // apply geometric transform to ground using perlin noise
    if (id == 0) {
        vec3 st = pos_obj_fs_in; 
        float eps = 0.001; 

        float h_c = sand_height(st);
        float h_x = sand_height(st + vec3(eps, 0, 0));
        float h_z = sand_height(st + vec3(0, 0, eps));


        vec3 grain_grad = vec3(h_c - h_x, 1.0, h_c - h_z); // Y is up
        grain_grad = normalize(grain_grad);

        mat3 normal_matrix = transpose(inverse(mat3(view)));
        vec3 grain_grad_view = normalize(normal_matrix * grain_grad);


        N = normalize(N + grain_grad_view * 0.5); 
    }
    

    vec3 light_pos_view = (view * vec4(light_position, 1.0)).xyz;
    vec3 L = normalize(light_pos_view - P);

    vec3 V = normalize(-P);

    vec3 H = normalize(L + V);

    vec3 ambient_term = ka * ambient_light * object_color;

    float diff_impact = max(dot(N, L), 0.0);
    vec3 diffuse_term = kd * light_color * object_color * diff_impact;

    float spec_impact = pow(max(dot(N, H), 0.0), p);
    vec3 specular_term = ks * light_color * spec_impact;


    vec3 linear_color = (ambient_term + diffuse_term + specular_term);


    //applying stylistic dithering
    int x = int(gl_FragCoord.x) % 4;
    int y = int(gl_FragCoord.y) % 4;
    float threshold = bayerMatrix[x + 4 * y];
    float color_depth = 2.0;
    vec3 dithered_color;
    dithered_color.r = floor(linear_color.r * color_depth + threshold)/color_depth;
    dithered_color.g = floor(linear_color.g * color_depth + threshold)/color_depth;
    dithered_color.b = floor(linear_color.b * color_depth + threshold)/color_depth;
    // color = pow(dithered_color, vec3(1.0/2.2));
    color = pow(dithered_color, vec3(1.0/2.2));
    // color by normals
    // color = 0.5 * normalize(normal_fs_in) + 0.5;
}