#ifndef ARCBALL_H
#define ARCBALL_H


// standard C/C++
#include <math.h>
#include <cassert>

// Qt includes
#include <QQuaternion>
#include <QVector3D>
#include <QVector2D>

/* The ArcBall class defines tools
* for rotating and viewing
* three-dimensional images.
 *
* When viewing three-dimensional
* objects there are many different
* ways to choose a viewpoint. The
* basic principle is to create a
* sphere around a chosen object and
* then, by choosing a point on that
* sphere, drag that point to a
* different location and have the
* object rotate in a way defined by
* a rotation around the arc-ball.
*
* Rotations are mapped onto a
* quaternion. */

class ArcBall
{
    protected:

        inline QVector3D mapToSphere (QVector2D mouse_coordinate);

    public:

        /* constructor */
        ArcBall ();

        /* destructor */
        ~ArcBall ();

        /* definitions of what to do when
"click" and "drag" are done with
the mouse. */
        void click (const QVector2D &mouse_coordinate);
        void drag (const QVector2D &mouse_coordinate);

        /* Return this quaternion. */
        QQuaternion get_quaternion (const QVector3D &vec_1,
                                    const QVector3D &vec_2);

        QQuaternion
        quaternion,
        q_down,
        mouseQuat;

        double
        invHeight,
        invWidth;

        QVector3D
        vector_begin,
        vector_end;

        double epsilon;

        // Inline and other functions
        inline void set_bounds (const int newWidth,
                                const int newHeight)
        {
            this->invWidth  = 2. / double (newWidth  - 1);
            this->invHeight = 2. / double (newHeight - 1);
        }

};                            // class ArcBall;




#endif // ARCBALL_H
