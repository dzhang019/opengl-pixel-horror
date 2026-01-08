#pragma once
#include <Eigen/Core>
#include <Eigen/Geometry>

class Camera {
public:
    Camera(Eigen::Vector3f init_pos);
    void moveForward(float amount);
    void moveRight(float amount);
    void moveLeft(float amount);
    void moveBack(float amount);
    void moveUp(float amount);
    void rotateYaw(float radians);
    void rotatePitch(float radians);
    void rotateRoll(float radians);
    Eigen::Matrix4f viewMatrix() const;
private:
    Eigen::Vector3f position;
    float yaw, pitch, roll;
};