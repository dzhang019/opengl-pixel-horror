#include "triangle_area_normal.h"
#include <Eigen/Geometry>

Eigen::RowVector3d triangle_area_normal(
  const Eigen::RowVector3d & a, 
  const Eigen::RowVector3d & b, 
  const Eigen::RowVector3d & c)
{
  ////////////////////////////////////////////////////////////////////////////
  // Replace with your code:
  ////////////////////////////////////////////////////////////////////////////
  Eigen::RowVector3d ab = b - a;
  Eigen::RowVector3d ac = c - a;
  Eigen::RowVector3d cross_product = ab.cross(ac);
  double area = 0.5 * cross_product.norm();
  if (area > 0) {
    return cross_product.normalized() * area;
  }
}
