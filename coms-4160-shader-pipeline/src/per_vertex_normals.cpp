#include "per_vertex_normals.h"
#include "triangle_area_normal.h"
#include <unordered_map>
void per_vertex_normals(
  const Eigen::MatrixXd & V,
  const Eigen::MatrixXi & F,
  Eigen::MatrixXd & N)
{
  N = Eigen::MatrixXd::Zero(V.rows(),3);
  ////////////////////////////////////////////////////////////////////////////
  // Add your code here:
  for (int i = 0; i< F.rows(); i++) {
    Eigen::RowVector3d a = V.row(F(i,0));
    Eigen::RowVector3d b = V.row(F(i,1));
    Eigen::RowVector3d c = V.row(F(i,2));
    Eigen::RowVector3d normal = triangle_area_normal(a,b,c);
    N.row(F(i,0)) +=normal;
    N.row(F(i,1)) +=normal;
    N.row(F(i,2)) +=normal;
  }
  for (int i =0; i< N.rows(); i++) {
    N.row(i).normalize();
  }
  ////////////////////////////////////////////////////////////////////////////
}
