#ifndef GRAPHICWINDOW_H
#define GRAPHICWINDOW_H

#include <QGLWidget>
#include <QMouseEvent>
#include <QCursor>

#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

#include "truss3d.h"
#include "arcball.h"
#include "dxfreader.h"

class GraphicWindow : public QGLWidget
{
        Q_OBJECT
    public:
        explicit GraphicWindow(QWidget *parent = 0);

        double xmax, xmin, ymax, ymin, panX, panY;
        bool isMousePress;

        Truss3D *mesh;

        DXFReader *reader;

        QVector3D mousePos; //arcball
        QVector2D mousePt; //arcball
        QPoint lastPos; //arcball


        QVector2D mousePt2; //test

        float* transformM;
        double screenW, screenH;

        ArcBall *arcb;

        GLdouble model_view[16];
        GLdouble projection[16];
        GLint viewport[4];

        double phiRot, thetaRot, psiRot;

        QVector3D getMousePos (int x, int y);
        QVector2D normalizeMouse(QPoint qp);

        void euler2matr ();
        void quat2matr (QQuaternion q);
        QVector3D quat2euler (QQuaternion q);
        QQuaternion quatfromEuler ();

        QOpenGLVertexArrayObject *vao;
        QOpenGLBuffer vbo;
        QOpenGLBuffer vbo2, vbo3;

        QOpenGLShaderProgram program;

        QMatrix4x4 projection2;
        QVector2D mousePressPosition;


    signals:

    public slots:

        void initializeGL();
        void resizeGL(int width, int height);
        void paintGL();

        void wheelEvent(QWheelEvent *event);
        void mousePressEvent(QMouseEvent *event);
        void mouseMoveEvent(QMouseEvent *event);
        void mouseReleaseEvent(QMouseEvent *event);
        void mouseDoubleClickEvent(QMouseEvent *event);

};

#endif // GRAPHICWINDOW_H


