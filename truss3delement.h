#ifndef TRUSS3DELEMENT_H
#define TRUSS3DELEMENT_H

#include "node3d.h"
#include "material.h"
#include <mth/matrix.h>


class Truss3DElement
{
public:
    int index;
    Node3D *node1;
    Node3D *node2;
    Material *material;
    double E, A;

    Truss3DElement();
    Truss3DElement(int index, Truss3DElement *element);
    Truss3DElement(int index, Node3D *node1, Node3D *node2, Material *material);
    double getStress(Mth::Matrix &ue);
    void getStiffnessMatrix(Mth::Matrix &ke);
    void draw(void);

};

#endif // TRUSS3DELEMENT_H
