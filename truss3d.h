#ifndef TRUSS3D_H
#define TRUSS3D_H

#include "node3d.h"
#include "truss3delement.h"

#include "dxfreader.h"

#include <mth/matrix.h>
#include <mth/vector.h>

class Truss3D
{
private:
    int ndi, nlo, nre, nma;

public:
    Node3D **nodes;
    Truss3DElement **elements;
    Material **materials;


    double **displacements, **loading;
    bool **restrictions;

    int nNodes;
    int nElements;
    bool isSolved;

    Mth::Matrix k;
    Mth::Vector f;
    Mth::Vector u;

    Mth::Vector reactions;
    Mth::Vector stress;


    Truss3D();
    Truss3D(char *filename);
    Truss3D(DXFReader *file);
    void loadfile(char *filename);

    void evalStiffnessMatrix(void);

    void stresslimits(double &min, double &max);

    void solve(void);

protected:
    virtual ~Truss3D();
};

#endif // TRUSS3D_H
