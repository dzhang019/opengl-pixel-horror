// camera.cpp
#define _USE_MATH_DEFINES
#include <cmath>
#include "camera.h"

Camera::Camera(Eigen::Vector3f init_pos){
    position = init_pos;
    yaw = 0.0f;
    pitch= 0.0f;
}

void Camera::moveForward(float amount){
    Eigen::Vector3f forward;
    forward.x() = -std::sin(yaw);
    forward.y() = 0.0f;
    forward.z() = -std::cos(yaw);
    position += amount * forward;
    //add camera shake with random offset
}

void Camera::moveRight(float amount){
    Eigen::Vector3f right;
    right.x() = std::cos(yaw);
    right.y() = 0.0f;
    right.z() = -std::sin(yaw);
    position += amount * right;
}

void Camera::moveUp(float amount){
    position.y() += amount;
}

void Camera::rotateYaw(float radians){
    yaw += radians;
}

void Camera::rotatePitch(float radians){
    pitch += radians;
    const float limit = 1.5f;
    if (pitch > limit) pitch = limit;
    if (pitch < -limit) pitch = -limit;
}

Eigen::Matrix4f Camera::viewMatrix() const {
    Eigen::AngleAxisf yaw_rot(yaw,   Eigen::Vector3f::UnitY());
    Eigen::AngleAxisf pitch_rot(pitch, Eigen::Vector3f::UnitX());
    Eigen::Quaternionf q = yaw_rot * pitch_rot;

    Eigen::Matrix3f R = q.toRotationMatrix();
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    view.block<3,3>(0,0) = R.transpose();
    view.block<3,1>(0,3) = -R.transpose() * position;
    return view;
}