#include "femtruss3d.h"
#include "ui_femtruss3d.h"

FEMTruss3D::FEMTruss3D(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FEMTruss3D)
{
    ui->setupUi(this);
}

FEMTruss3D::~FEMTruss3D()
{
    delete ui;
}
