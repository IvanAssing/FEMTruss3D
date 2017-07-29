#ifndef FEMTRUSS3D_H
#define FEMTRUSS3D_H

#include <QMainWindow>

namespace Ui {
class FEMTruss3D;
}

class FEMTruss3D : public QMainWindow
{
    Q_OBJECT

public:
    explicit FEMTruss3D(QWidget *parent = 0);
    ~FEMTruss3D();

private:
    Ui::FEMTruss3D *ui;
};

#endif // FEMTRUSS3D_H
