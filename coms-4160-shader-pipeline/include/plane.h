#pragma once
#include <Eigen/Core>
void plane(
    const int res_x,
    const int res_z,
    const float size_x, 
    const float size_z,
    Eigen::MatrixXd & V,
    Eigen::MatrixXi & F,
    Eigen::MatrixXd & UV,
    Eigen::MatrixXd & N
);