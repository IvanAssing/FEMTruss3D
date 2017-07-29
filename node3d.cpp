#include "node3d.h"

Node3D::Node3D()
{

}

Node3D::Node3D(int index, double *coordinates, bool *restrictions,
               double *loading, double *displacements)
    :index(index), restrictions(restrictions),
      loading(loading), displacements(displacements)
{
    this->coordinates = new double[3];
    this->coordinates[0] = coordinates[0];
    this->coordinates[1] = coordinates[1];
    this->coordinates[2] = coordinates[2];
}
