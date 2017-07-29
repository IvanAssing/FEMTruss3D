#include "femtruss3d.h"
#include <QApplication>

#include "truss3d.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FEMTruss3D w;
    w.show();

//    Truss3D *mesh;

//    mesh = new Truss3D("truss_4.ft3d");
//    mesh->evalStiffnessMatrix();
//    mesh->solve();




    return a.exec();
}
