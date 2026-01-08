#pragma once
#include "gl.h"
#include "Eigen/Core"
#include "Eigen/Geometry"
#include <string>
#include <vector>

class Object {
public:
    Eigen::Matrix4f model_matrix = Eigen::Matrix4f::Identity();
    Object();
    ~Object();
    void load_from_file(const std::string& filename);
    void load_texture(const std::string& texture_file);
    void draw(GLuint shader_program_id);
    void create_procedural_plane();
    void scale(float s);
    void translate(float x, float y, float z);
    void rotate(float angle_deg, float x, float y, float z);
    Eigen::Vector3f ka;
    Eigen::Vector3f kd;
    Eigen::Vector3f ks;
    float p;
    int id;
private:
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> V;
    Eigen::Matrix<float, Eigen::Dynamic, 3, Eigen::RowMajor> NV;
    Eigen::Matrix<float, Eigen::Dynamic, 2, Eigen::RowMajor> UV;
    Eigen::Matrix<GLuint, Eigen::Dynamic, 3, Eigen::RowMajor> F;
    GLuint VAO = 0;
    GLuint VBO_V = 0;
    GLuint VBO_N = 0;
    GLuint VBO_UV = 0;
    GLuint EBO = 0;

    GLuint texture_id = 0; 
    void update_gpu();
};