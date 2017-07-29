#include "graphicwindow.h"

#include <cmath>
#include <iomanip>
#include <string>
#include <cstdio>

#include <QImage>
#include <QString>
#include <QImageWriter>
#include <QDateTime>
#include <QElapsedTimer>



#include <GL/gl.h>

#include <GL/glu.h>
#include <FTGL/FTGLPixmapFont.h>

#include "dxfreader.h"

static const double deg2rad    =  0.01745329251994329576;
static const double pi_2       =  1.57079632679489661922;
static const double rad2deg    = 57.29577951308232087721;


GraphicWindow::GraphicWindow(QWidget *parent) :
    QGLWidget(parent)
{
    double size = 100.;

    xmin = -size;
    ymin = -size;
    xmax = +size;
    ymax = +size;

    reader = new DXFReader;
    //reader->readfile("../DXFReader/cube2.dxf");
    reader->readfile("../DXFReader/cobertura plana_v4.dxf");
    //reader->readfile("../DXFReader/cobertura arco_v2.dxf");

    /*
    mesh = new Truss3D(reader);

    //mesh->evalStiffnessMatrix();
    //mesh->solve();
*/

    QElapsedTimer timer;

    timer.start();

    mesh = new Truss3D("../DXFReader/cobertura plana_v4.ft3d");

    std::cerr<<"\n :nodes = "<<mesh->nNodes;
    std::cerr<<"\n :elements = "<<mesh->nElements;

    mesh->evalStiffnessMatrix();
    mesh->solve();



    std::cerr<<"\n :timing = "<<timer.elapsed()/1000<<" s";



    this->phiRot           = 0.;
    this->thetaRot         = 0.;
    this->psiRot           = 0.;
    this->mousePos         = QVector3D(0., 0., 0.);

    arcb       = new ArcBall();
    transformM = new float[16];

    for (unsigned int i=1; i<15; ++i)
        transformM[i] = 0.;

    for (unsigned int i=0; i<16; i+=5)
        transformM[i] = 1.;
}


void GraphicWindow::initializeGL()
{
    glShadeModel(GL_SMOOTH);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClearDepth(1.0f);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


    vao = new QOpenGLVertexArrayObject(this);
    vao->create();
    vao->bind();

    GLfloat *points = new GLfloat[3*2*mesh->nElements];
    //    GLuint *indices = new GLuint[2*2*mesh->nElements];

    for(int i=0; i<mesh->nElements; i++)
    {
        points [6*i+0] = static_cast<GLfloat>(mesh->elements[i]->node1->coordinates[0]+
                mesh->u(3*mesh->elements[i]->node1->index+0));
        points [6*i+1] = static_cast<GLfloat>(mesh->elements[i]->node1->coordinates[1]+
                mesh->u(3*mesh->elements[i]->node1->index+1));
        points [6*i+2] = static_cast<GLfloat>(mesh->elements[i]->node1->coordinates[2]+
                mesh->u(3*mesh->elements[i]->node1->index+2));

        points [6*i+3] = static_cast<GLfloat>(mesh->elements[i]->node1->coordinates[0]+
                mesh->u(3*mesh->elements[i]->node2->index+0));
        points [6*i+4] = static_cast<GLfloat>(mesh->elements[i]->node1->coordinates[1]+
                mesh->u(3*mesh->elements[i]->node2->index+1));
        points [6*i+5] = static_cast<GLfloat>(mesh->elements[i]->node1->coordinates[2]+
                mesh->u(3*mesh->elements[i]->node2->index+2));


        //        indices [4*i] = static_cast<GLuint>(mesh->elements[i]->node1->index);
        //        indices [4*i+1] = static_cast<GLuint>(reader->lbuffer->data[3*i+1]);

    }



    //    for(int i=0; i<reader->lbuffer->count; i++)
    //    {
    //        indices [2*i] = static_cast<GLuint>(reader->lbuffer->data[3*i]);
    //        indices [2*i+1] = static_cast<GLuint>(reader->lbuffer->data[3*i+1]);
    //    }

    double T0, T1, T2, T3, T4, Tn, R, G, B;
    mesh->stresslimits(T0, T4);

    //    if(mesh->nNodes)
    //        getMaxMin(mesh->stress, mesh->nElements, T4, T0);

    T0 = -10000;
    T4 =  10000;

    std::cerr<<"\n min= "<<T0<<" max= "<<T4;

    T2 = (T0+T4)/2.;
    T1 = (T0+T2)/2.;
    T3 = (T2+T4)/2.;

    GLfloat *colors = new GLfloat[4*mesh->nElements];


    for(int i=0; i<mesh->nElements; i++){

        Tn = mesh->stress(i);
        R = Tn<T2?  0. : (Tn>T3? 1. : (Tn-T2)/(T3-T2));
        B = Tn>T2?  0. : (Tn<T1? 1. : (T2-Tn)/(T2-T1));
        G = Tn<T1? (Tn-T0)/(T1-T0) : Tn>T3 ? (T4-Tn)/(T4-T3) : 1.;

        colors[4*i] = R;
        colors[4*i+1] = G;
        colors[4*i+2] = B;
        colors[4*i+3] = 0.8;

    }




    // Create a Vector Buffer Object that will store the vertices on video memory
    vbo.create();
    vbo.setUsagePattern(QOpenGLBuffer::StreamDraw);
    vbo.bind();
    vbo.allocate(points, mesh->nElements*3*2*sizeof(GL_FLOAT));

    //    vbo3.create();
    //    vbo3.setUsagePattern(QOpenGLBuffer::StreamDraw);
    //    vbo3.bind();
    //    vbo3.allocate(indices, reader->lbuffer->count*2*sizeof(GLuint));

    vbo2.create();
    vbo2.setUsagePattern(QOpenGLBuffer::StreamDraw);
    vbo2.bind();
    vbo2.allocate(colors, mesh->nElements*4*sizeof(GLfloat));






    program.addShaderFromSourceFile(QOpenGLShader::Vertex, "../FEMTruss3D/vertexshader.vert");
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, "../FEMTruss3D/fragmentshader.frag");

    program.link();
    program.bind();

    vbo.bind();
    int vertexLocation = program.attributeLocation("position");
    program.enableAttributeArray(vertexLocation);
    program.setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 3, 0);

    vbo2.bind();
    int vertexColor = program.attributeLocation("color");
    program.enableAttributeArray(vertexColor);
    program.setAttributeBuffer(vertexColor, GL_FLOAT, 0, 4, 0);

    //program.attributeLocation("qt_ModelViewProjectionMatrix");

    //GLuint shaderProgram = create_program("shaders/vert.shader", "shaders/frag.shader");



    //    // Get the location of the attributes that enters in the vertex shader
    //    GLint position_attribute = QOpenGLFunctions_4_5_Core::glGetAttribLocation(program, "position");

    //    // Specify how the data for position can be accessed
    //    QOpenGLFunctions_4_5_Core::glVertexAttribPointer(position_attribute, 2, GL_FLOAT, GL_FALSE, 0, 0);

    //    // Enable the attribute
    //    QOpenGLFunctions_4_5_Core::glEnableVertexAttribArray(position_attribute);


    //*/
}

void GraphicWindow::resizeGL(int width, int height)
{

    double width_ = static_cast<double>(this->width());
    double height_ = static_cast<double>(this->height());

    if (width_ > height_)
    {
        height_ = height_?height_:1;
        double correction = 0.5 * ( width_/ height_ * (ymax-ymin) - (xmax-xmin) );
        xmin   -= correction;
        xmax +=correction;
    }
    else
    {
        width_ = width_?width_:1;
        double correction = 0.5 * ( height_ / width_ * (xmax-xmin) - (ymax-ymin) );
        ymin   -= correction;
        ymax  += correction;
    }

    glViewport( 0, 0, (GLint)width, (GLint)height );

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(xmin, xmax, ymin, ymax, 1000.0, -1000.0);
    //gluPerspective(60, (float)width/height, 0.1, 50000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    screenW = double(width);
    screenH = double(height);

    arcb->set_bounds (width, height);


    // Reset projection
    projection2.setToIdentity();

    // Set perspective projection
    projection2.ortho(xmin, xmax, ymin, ymax, 1000.0, -1000.0);

}

void GraphicWindow::wheelEvent(QWheelEvent *event)
{
    double zoom_factor = -event->delta()/120*0.2;


    double X_opengl = event->x()/static_cast<double>(this->width())*(xmax - xmin)+xmin;
    double Y_opengl  = (this->height()-event->y())/static_cast<double>(this->height())*(ymax - ymin)+ymin;

    xmin -= (X_opengl-xmin)*zoom_factor;
    xmax += (xmax-X_opengl)*zoom_factor;

    ymin -= (Y_opengl-ymin)*zoom_factor;
    ymax += (ymax-Y_opengl)*zoom_factor;


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(xmin, xmax, ymin, ymax, 1000.0, -1000.0);

    // Reset projection
    projection2.setToIdentity();

    // Set perspective projection
    projection2.ortho(xmin, xmax, ymin, ymax, 1000.0, -1000.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    this->repaint();

}

void GraphicWindow::mousePressEvent(QMouseEvent *event)
{
    isMousePress = true;
    panX = event->x();
    panY = event->y();

    lastPos = event->pos();
    mousePt = normalizeMouse(lastPos);

    mousePt2 = mousePt;


    if (event->buttons() == Qt::RightButton)
    {
        mousePos = getMousePos(event->x(), event->y());

        // Update start vector and prepare
        // For dragging
        arcb->click (mousePt);
    }

    // Save mouse press position
    mousePressPosition = QVector2D(event->localPos());


}

void GraphicWindow::mouseReleaseEvent(QMouseEvent *event)
{
    isMousePress = false;
}

void GraphicWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    QDateTime now = QDateTime::currentDateTime();

    QString filename = QString("snapshot-")
            + now.toString("yyyyMMddhhmmsszzz") + QString(".png");

    this->updateGL();

    this->grabFrameBuffer(true).save(filename, "PNG", 100);

}

void GraphicWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        double X_opengl = (-event->x()+panX)/static_cast<double>(this->width())*(xmax - xmin);
        double Y_opengl  = (event->y()-panY)/static_cast<double>(this->height())*(ymax - ymin);

        xmax += X_opengl;
        xmin += X_opengl;

        ymax += Y_opengl;
        ymin += Y_opengl;

        panX = event->x();
        panY = event->y();


        glOrtho(xmin, xmax, ymin, ymax, 10.0, -10.0);

        // Reset projection
        projection2.setToIdentity();

        // Set perspective projection
        projection2.ortho(xmin, xmax, ymin, ymax, 1000.0, -1000.0);

        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        this->repaint();

    }

    lastPos = event->pos();
    mousePt = normalizeMouse(lastPos);

    if (event->buttons() & Qt::RightButton)
    {
        arcb->drag(mousePt);

        quat2matr(arcb->quaternion);
        QVector3D result = quat2euler(arcb->quaternion)*rad2deg;

        if ( thetaRot!=result.x() )
        {
            thetaRot = result.x();
            if ( thetaRot<-180. ) thetaRot += 360.;
            if ( thetaRot>180. ) thetaRot -= 360.;
            //emit SIG_thetaRotationChanged(int(thetaRot));
        }
        if ( phiRot!=result.z() )
        {
            phiRot = result.z();
            if ( phiRot<-90. ) phiRot += 180.;
            if ( phiRot>90. ) phiRot -= 180.;
            //emit SIG_phiRotationChanged(int(phiRot));
        }
        if ( psiRot!=result.y() )
        {
            psiRot = result.y();
            if ( psiRot<-180. ) psiRot += 180.;
            if ( psiRot>180. ) psiRot -= 180.;
            //emit SIG_psiRotationChanged(int(psiRot));
        }




        updateGL();

    }
}

void processHits (GLint hits, GLuint buffer[])
{
    unsigned int i, j;
    GLuint names, *ptr;

    printf ("hits = %d\n", hits);
    ptr = (GLuint *) buffer;
    for (i = 0; i < hits; i++) { /*  for each hit  */
        names = *ptr;
        printf (" number of names for hit = %d\n", names); ptr++;
        printf("  z1 is %g;", (float) *ptr/0x7fffffff); ptr++;
        printf(" z2 is %g\n", (float) *ptr/0x7fffffff); ptr++;
        printf ("   the name is ");
        for (j = 0; j < names; j++) {     /*  for each name */
            printf ("%d ", *ptr); ptr++;
        }
        printf ("\n");
    }
}


void GraphicWindow::paintGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //projection2.translate(mousePt2.x(), mousePt2.y(), 0.0);

    //std::cerr<<"\n"<<mousePt2.x()<<"  "<<mousePt2.y();

    glGetDoublev (GL_MODELVIEW_MATRIX, model_view);
    glGetDoublev (GL_PROJECTION_MATRIX, projection);
    glGetIntegerv (GL_VIEWPORT, viewport);

    //glTranslatef(0.,1.,0.);




    glMultMatrixf (transformM);

    //glTranslated(-mousePt.x(), -mousePt.y(), 0.0);


    //glTranslatef(0.,-1,0.);


    //    glColor4d(1.0, 1.0, 1.0, 1.0);
    //    for(int i=0; i<mesh->nElements; i++)
    //    {
    //        glBegin(GL_LINES);
    //        glVertex3d(mesh->elements[i]->node1->coordinates[0],
    //                mesh->elements[i]->node1->coordinates[1],
    //                mesh->elements[i]->node1->coordinates[2]);
    //        glVertex3d(mesh->elements[i]->node2->coordinates[0],
    //                mesh->elements[i]->node2->coordinates[1],
    //                mesh->elements[i]->node2->coordinates[2]);
    //        glEnd();
    //    }

    //    glColor4d(1.0, 0.0, 0.0, 1.0);
    //    glPointSize(5.f);
    //    for(int i=0; i<mesh->nNodes; i++)
    //    {
    //        glBegin(GL_POINTS);
    //        glVertex3d(mesh->nodes[i]->coordinates[0],
    //                mesh->nodes[i]->coordinates[1],
    //                mesh->nodes[i]->coordinates[2]);
    //        glEnd();
    //    }

    //    GLuint selectBuf[1024];
    //    GLint hits;

    //    glSelectBuffer (1024, selectBuf);
    //    (void) glRenderMode (GL_SELECT);

    //    glInitNames();
    //    glPushName(0);


    //    glColor4d(1.0, 1.0, 1.0, 1.0);
    //    for(int i=0; i<mesh->nElements; i++)
    //    {
    //        glLoadName(i+1);
    //        mesh->elements[i]->draw();
    //    }


    //    hits = glRenderMode (GL_RENDER);
    //    processHits (hits, selectBuf);



    /*
    FTGLPolygonFont font("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    FTPoint coord(10, 10, 10);

        if(font.Error())
            qDebug()<<"erro no carregamento da fonte";



    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glColor4f(1, 0, 0, 1);
    font.FaceSize(8);
    font.Render("abc", -1 , coord);

    glPopAttrib();
*/

    //glLoadIdentity();

    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QMatrix4x4 mat(transformM);

    program.setUniformValue("mvp_matrix", projection2*mat);



    vao->bind();

    //    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);


    //    glDrawArrays(GL_POINTS, 0, reader->pbuffer->count);

    //glDrawArrays(GL_LINES, 0, mesh->nElements);

        GLuint indices[2*mesh->nElements];

        for(int i=0; i<2*mesh->nElements; i++)
        {
            indices [2*i] = 2*i;//static_cast<GLuint>(reader->lbuffer->data[3*i]);
            indices [2*i+1] = 2*i+1;//static_cast<GLuint>(reader->lbuffer->data[3*i+1]);
        }

        //glDrawArrays(GL_POINTS, 0, mesh->nElements);


    glDrawElements(GL_LINES, mesh->nElements, GL_UNSIGNED_INT, &indices[0]);


    /*
    FTGLPolygonFont font("/usr/share/fonts/truetype/freefont/FreeSans.ttf");


    if(font.Error())
        qDebug()<<"erro no carregamento da fonte";



    glPushAttrib(GL_ALL_ATTRIB_BITS);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glLoadIdentity();

    glColor4f(1, 0, 0, 1);
    font.FaceSize(1);

    char text[50];
    for(int i=0; i<1; i++)
    {
        glPopMatrix();
        glTranslated(mesh->nodes[i]->coordinates[0], mesh->nodes[i]->coordinates[1], mesh->nodes[i]->coordinates[2]);
        //FTPoint coord(mesh->nodes[i]->coordinates[0], mesh->nodes[i]->coordinates[1], mesh->nodes[i]->coordinates[2]);
        FTPoint coord(10,10,10);
        std::sprintf(text, "%d", i);
        font.Render(text, -1 , coord);
        glPushMatrix();
    }

    glPopAttrib();

*/
}


QVector3D GraphicWindow::getMousePos (int x, int y)
{
    GLfloat winX, winY, winZ;
    GLdouble posX, posY, posZ;
    winX = float(x);
    winY = float(viewport[3]) - float(y);

    winZ = 100.;

    gluUnProject( winX, winY, winZ, model_view, projection, viewport,
                  &posX, &posY, &posZ);

    //        qWarning(" +++ GetMousePos  mouse position:  (%g, %g, %g) (%g, %g, %g)",
    //                 winX, winY, winZ, posX, posY, posZ);
    return QVector3D(posX, posY, posZ);
}


QVector2D GraphicWindow::normalizeMouse(QPoint qp)
{
    QVector2D res = QVector2D(arcb->invWidth * (double(qp.x()) - 0.5*screenW),
                              arcb->invHeight * (0.5*screenH - double(qp.y())));

    return res;
}



void GraphicWindow::euler2matr ()
{
    // Assume the angles are in passed in
    // in radians.
    double ch = cos (thetaRot*deg2rad); // heading
    double sh = sin (thetaRot*deg2rad);
    double ca = cos (psiRot  *deg2rad);   // attitude
    double sa = sin (psiRot  *deg2rad);
    double cb = cos (phiRot  *deg2rad);   // bank
    double sb = sin (phiRot  *deg2rad);

    transformM[0]  = ch*ca;
    transformM[1]  = sh*sb - ch*sa*cb;
    transformM[2]  = ch*sa*sb + sh*cb;
    transformM[3]  = 0.;
    transformM[4]  = sa;
    transformM[5]  = ca*cb;
    transformM[6]  = -ca*sb;
    transformM[7]  = 0.;
    transformM[8]  = -sh*ca;
    transformM[9]  = sh*sa*cb + ch*sb;
    transformM[10] = -sh*sa*sb + ch*cb;
    transformM[11] = 0.;
    transformM[12] = 0.;
    transformM[13] = 0.;
    transformM[14] = 0.;
    transformM[15] = 1.;
}

void GraphicWindow::quat2matr (QQuaternion q)
{
    double sqw = q.scalar()*q.scalar();
    double sqx = q.x()*q.x();
    double sqy = q.y()*q.y();
    double sqz = q.z()*q.z();

    double invs = 1 / (sqx + sqy + sqz + sqw);
    transformM[0] = (sqx - sqy - sqz + sqw)*invs;
    transformM[5] = (-sqx + sqy - sqz + sqw)*invs;
    transformM[10] = (-sqx - sqy + sqz + sqw)*invs;

    double tmp1 = q.x()*q.y();
    double tmp2 = q.z()*q.scalar();
    transformM[4] = 2.*(tmp1 + tmp2)*invs;
    transformM[1] = 2.*(tmp1 - tmp2)*invs;
    tmp1 = q.x()*q.z();
    tmp2 = q.y()*q.scalar();
    transformM[8] = 2.*(tmp1 - tmp2)*invs;
    transformM[2] = 2.*(tmp1 + tmp2)*invs;
    tmp1 = q.y()*q.z();
    tmp2 = q.x()*q.scalar();
    transformM[9] = 2.*(tmp1 + tmp2)*invs;
    transformM[6] = 2.*(tmp1 - tmp2)*invs;
    transformM[3] = transformM[7] = transformM[11] =
            transformM[12] = transformM[13] = transformM[14] = 0.;
    transformM[15] = 1.;
}


QVector3D GraphicWindow::quat2euler (QQuaternion q)
{
    double head, att, b;
    double test = q.x()*q.y() + q.z()*q.scalar();
    if (test > 0.499)
    { // singularity at north pole
        head = 2.*atan2(q.x(), q.scalar());
        att = pi_2;
        b = 0.;
        return QVector3D(head, att, b);
    }
    if (test < -0.499)
    { // singularity at south pole
        head = -2.*atan2(q.x(), q.scalar());
        att = - pi_2;
        b = 0.;
        return QVector3D(head, att, b);
    }
    double sqx = q.x()*q.x();
    double sqy = q.y()*q.y();
    double sqz = q.z()*q.z();
    head = atan2(2.*q.y()*q.scalar()-2.*q.x()*q.z(), 1. - 2.*sqy - 2.*sqz);
    att = asin(2.*test);
    b = atan2(2.*q.x()*q.scalar()-2.*q.y()*q.z(), 1. - 2.*sqx - 2.*sqz);
    return QVector3D(head, att, b);
}

QQuaternion GraphicWindow::quatfromEuler ()
{
    //   QQuaternion res;
    double heading2  = 0.5 * deg2rad * thetaRot;
    double attitude2 = 0.5 * deg2rad * psiRot;
    double bank2     = 0.5 * deg2rad * phiRot;

    double c1   = cos (heading2);
    double s1   = sin (heading2);
    double c2   = cos (attitude2);
    double s2   = sin (attitude2);
    double c3   = cos (bank2);
    double s3   = sin (bank2);
    double c1c2 = c1*c2;
    double s1s2 = s1*s2;
    return QQuaternion(c1c2*c3 - s1s2*s3,
                       c1c2*s3 + s1s2*c3, s1*c2*c3 + c1*s2*s3, c1*s2*c3 - s1*c2*s3);
}
