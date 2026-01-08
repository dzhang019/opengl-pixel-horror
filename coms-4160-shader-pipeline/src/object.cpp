#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#include <cmath> 
#include <stb_image.h>
#include "object.h"
#include <iostream>
#include <igl/readOBJ.h>
#include <igl/opengl/glfw/Viewer.h>
#include "per_vertex_normals.h"
#include "plane.h"
Object::Object() {
    model_matrix.setIdentity();
    VAO = 0;
    VBO_V = 0;
    VBO_N = 0;
    VBO_UV = 0;
    EBO = 0;
    texture_id=0;
    
    ka = Eigen::Vector3f(0.1, 0.1, 0.1);
    kd = Eigen::Vector3f(1.0, 0.0, 0.5); 
    ks = Eigen::Vector3f(0.8, 0.8, 0.8);
    p  = 64.0; 
    id = -1;
}
Object::~Object() {
    if (VBO_V) glDeleteBuffers(1, &VBO_V);
    if (VBO_N) glDeleteBuffers(1, &VBO_N);
    if (VBO_UV) glDeleteBuffers(1, &VBO_UV);
    if (EBO) glDeleteBuffers(1, &EBO);
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (texture_id) glDeleteTextures(1, &texture_id);
}

void Object::load_from_file(const std::string& filename) {
    Eigen::MatrixXi F_int, UF_int, NF_int;
    Eigen::MatrixXd V_double, UV_double, NV_double;

    if (!igl::readOBJ(filename, V_double, UV_double, NV_double, F_int, UF_int, NF_int)) {
        std::cerr << "Error: Could not load " << filename << std::endl;
    }
    if (F_int.cols() == 4) {
        std::cout << "Warning: OBJ contains Quads. Triangulating..." << std::endl;
        
        // Manual Triangulation: Split 1 Quad (0,1,2,3) into 2 Triangles (0,1,2) and (0,2,3)
        Eigen::MatrixXi F_tri(F_int.rows() * 2, 3);
        
        for (int i = 0; i < F_int.rows(); ++i) {
            // Triangle 1: 0-1-2
            F_tri.row(2 * i + 0) << F_int(i, 0), F_int(i, 1), F_int(i, 2);
            // Triangle 2: 0-2-3
            F_tri.row(2 * i + 1) << F_int(i, 0), F_int(i, 2), F_int(i, 3);
        }
        
        // Overwrite F_int with the new triangulated list
        F_int = F_tri;
    }

    if (NV_double.rows()==0 || NV_double.rows() != V_double.rows()){
        per_vertex_normals(V_double,F_int,NV_double);
    }

    V = V_double.cast<float>();
    UV = UV_double.cast<float>();
    NV = NV_double.cast<float>();
    F = F_int.cast<GLuint>();
    update_gpu();
}
void Object::translate(float x, float y, float z) {
    Eigen::Matrix4f translate_mat = Eigen::Matrix4f::Identity();
    translate_mat(0,3) = x;
    translate_mat(1,3) = y;
    translate_mat(2,3) = z;
    model_matrix = translate_mat * model_matrix;
}

void Object::scale(float s) {
    Eigen::Matrix4f scale_mat = Eigen::Matrix4f::Identity();
    scale_mat(0,0) = s;
    scale_mat(1,1) = s;
    scale_mat(2,2) = s;
    model_matrix = model_matrix * scale_mat;
}

void Object::rotate(float angle_deg, float x, float y, float z) {
    float angle_rad = angle_deg * (M_PI / 180.0f);
    Eigen::Matrix4f rotation_mat = Eigen::Matrix4f::Identity();
    Eigen::Vector3f axis(x, y, z);
    rotation_mat.block(0,0,3,3) = Eigen::AngleAxisf(angle_rad, axis.normalized()).toRotationMatrix();
    model_matrix = rotation_mat * model_matrix;
}
void Object::load_texture(const std::string& texture_file) {
    if (texture_id) glDeleteTextures(1, &texture_id);

    int width, height, channels;
    unsigned char* data = stbi_load(texture_file.c_str(), &width, &height, &channels, 4); // Force 4 channels (RGBA)

    if (!data) {
        std::cerr << "Error: Could not load texture " << texture_file << std::endl;
        return;
    }

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Object::update_gpu() {
    // Clear old buffers if they exist
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO_V) glDeleteBuffers(1, &VBO_V);
    if (VBO_N) glDeleteBuffers(1, &VBO_N);
    if (VBO_UV) glDeleteBuffers(1, &VBO_UV);
    if (EBO) glDeleteBuffers(1, &EBO);

    // 1. Create the Master Object (VAO)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 2. Buffer Positions (Location 0)
    glGenBuffers(1, &VBO_V);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_V);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * V.size(), V.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 3. Buffer Normals (Location 1)
    glGenBuffers(1, &VBO_N);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_N);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NV.size(), NV.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    // 4. Buffer Texture Coords (Location 2)
    if (UV.rows() > 0) {
        glGenBuffers(1, &VBO_UV);
        glBindBuffer(GL_ARRAY_BUFFER, VBO_UV);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * UV.size(), UV.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(2);
    }

    // 5. Buffer Indices (EBO)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * F.size(), F.data(), GL_STATIC_DRAW);

    // Unbind to be safe
    glBindVertexArray(0);
}

void Object::draw(GLuint prog_id) {
    if (VAO == 0) return;

    // 1. Send Matrix Uniforms
    glUniformMatrix4fv(glGetUniformLocation(prog_id, "model"), 1, GL_FALSE, model_matrix.data());
    // glUniform1i(glGetUniformLocation(prog_id, "is_moon"), is_moon);

    // 2. Handle Texture
    if (texture_id != 0) {
        std::cout << "texture id !=0" << std::endl;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glUniform1i(glGetUniformLocation(prog_id, "tex_map"), 0); // Read from Unit 0
        glUniform1i(glGetUniformLocation(prog_id, "use_texture"), 1);
    } else {
        std::cout << "tecture id ==0" << std::endl;
        glUniform1i(glGetUniformLocation(prog_id, "use_texture"), 0);
    }
    GLint ka_loc = glGetUniformLocation(prog_id, "ka");
    if (ka_loc == -1) std::cout << "ERROR: Shader variable 'ka' not found!" << std::endl;
    GLint kd_loc = glGetUniformLocation(prog_id, "kd");
    if (kd_loc == -1) std::cout << "ERROR: Shader variable 'kd' not found!" << std::endl;
    GLint ks_loc = glGetUniformLocation(prog_id, "ks");
    if (ks_loc == -1) std::cout << "ERROR: Shader variable 'ks' not found!" << std::endl;
    GLint p_loc = glGetUniformLocation(prog_id, "p");
    if (p_loc == -1) std::cout << "ERROR: Shader variable 'p' not found!" << std::endl;
    glUniform3f(glGetUniformLocation(prog_id, "ka"), ka[0], ka[1], ka[2]);
    glUniform3f(glGetUniformLocation(prog_id, "kd"), kd[0], kd[1], kd[2]);
    glUniform3f(glGetUniformLocation(prog_id, "ks"), ks[0], ks[1], ks[2]);
    glUniform1f(glGetUniformLocation(prog_id, "p"),  p);
    glUniform1i(glGetUniformLocation(prog_id, "id"), id);
    std::cout << ka[0] << ka[1] << ka[2] << std::endl;
    GLfloat debug_values[3];
    glGetUniformfv(prog_id, ks_loc, debug_values);
    std::cout << "GPU received KS: " << debug_values[0] << " " << debug_values[1] << " " << debug_values[2] << std::endl;
    // 3. Draw
    glBindVertexArray(VAO);
    // Crucial: Use GL_PATCHES because you are using Tessellation Shaders!
    glDrawElements(GL_PATCHES, F.size(), GL_UNSIGNED_INT, 0); 
    glBindVertexArray(0);
}


void Object::create_procedural_plane() {
    Eigen::MatrixXd V_double, UV_double, NV_double;
    Eigen::MatrixXi F_int;

    plane(100, 100, 1000.0f, 1000.0f, V_double, F_int, UV_double, NV_double);

    V = V_double.cast<float>();
    UV = UV_double.cast<float>();
    NV = NV_double.cast<float>();
    F = F_int.cast<GLuint>();

    update_gpu();
}