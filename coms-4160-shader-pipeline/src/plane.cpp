#include "plane.h"

void plane(
    const int res_x,
    const int res_z,
    const float size_x, 
    const float size_z,
    Eigen::MatrixXd & V,
    Eigen::MatrixXi & F,
    Eigen::MatrixXd & UV,
    Eigen::MatrixXd & N)
{
    int num_verts = (res_x + 1) * (res_z + 1);
    int num_faces = res_x * res_z * 2;

    V.resize(num_verts, 3);
    UV.resize(num_verts, 2);
    N.resize(num_verts, 3);
    F.resize(num_faces, 3);

    int v_idx = 0;
    for (int z = 0; z <= res_z; ++z) {
        for (int x = 0; x <= res_x; ++x) {
            // Normalized coordinates (0.0 to 1.0)
            float u = (float)x / (float)res_x;
            float v = (float)z / (float)res_z;

            // Position: Centered at 0,0
            V(v_idx, 0) = (u - 0.5f) * size_x; // X
            V(v_idx, 1) = 0.0f;                // Y (Flat)
            V(v_idx, 2) = (v - 0.5f) * size_z; // Z

            // Normals: Pointing straight up (Positive Y)
            N(v_idx, 0) = 0.0f;
            N(v_idx, 1) = 1.0f;
            N(v_idx, 2) = 0.0f;

            UV(v_idx, 0) = u;
            UV(v_idx, 1) = v;

            v_idx++;
        }
    }

    // 2. Generate Triangles (Indices)
    int f_idx = 0;
    for (int z = 0; z < res_z; ++z) {
        for (int x = 0; x < res_x; ++x) {
            // Calculate index of the 4 corners of this grid cell
            int topLeft = z * (res_x + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (res_x + 1) + x;
            int bottomRight = bottomLeft + 1;

            // Triangle 1 (Top-Left, Bottom-Left, Top-Right)
            F(f_idx, 0) = topLeft;
            F(f_idx, 1) = bottomLeft;
            F(f_idx, 2) = topRight;
            f_idx++;

            // Triangle 2 (Top-Right, Bottom-Left, Bottom-Right)
            F(f_idx, 0) = topRight;
            F(f_idx, 1) = bottomLeft;
            F(f_idx, 2) = bottomRight;
            f_idx++;
        }
    }
}