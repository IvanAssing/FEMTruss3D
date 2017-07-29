#ifndef NODE3D_H
#define NODE3D_H


class Node3D
{
public:
    int index;
    double *coordinates, *displacements, *loading;
    bool *restrictions;

    Node3D();
    Node3D(int index, double *coordinates, bool *restrictions = nullptr, double *loading = nullptr, double *displacements = nullptr);


};

#endif // NODE3D_H
