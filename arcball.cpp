#include "arcball.h"

/* Constructor. Create an empty
(null) ArcBall by initialisation
of certain variables. */
ArcBall::ArcBall ()
    :
      epsilon      (1.e-8),
      vector_begin (QVector3D (0., 0., 0.)),
      vector_end   (QVector3D (0., 0., 0.)),
      mouseQuat    (QQuaternion (1., 0., 0., 0.)),
      q_down       (QQuaternion (1., 0., 0., 0.)),
      quaternion   (QQuaternion (1., 0., 0., 0.))
{}

ArcBall::~ArcBall ()
{}

void ArcBall::click (const QVector2D &mouse_coordinate)
{
    this->vector_begin  = mapToSphere (mouse_coordinate);
    this->q_down    = this->quaternion;
    this->mouseQuat = QQuaternion (1., 0., 0., 0.);
    //this->mouseQuat = QQuaternion (1., 1., 1., 0.);
}

void ArcBall::drag (const QVector2D &mouse_coordinate)
{
    this->vector_end     = mapToSphere (mouse_coordinate);
    this->mouseQuat  = get_quaternion (this->vector_begin, this->vector_end);
    this->quaternion = this->mouseQuat * this->q_down;
}

QQuaternion ArcBall::get_quaternion (const QVector3D &vec_1,
                                     const QVector3D &vec_2)
{
    QVector3D perpendicular_vector = QVector3D::crossProduct (vec_2, vec_1);

    return (perpendicular_vector.x ()*perpendicular_vector.x () +
            perpendicular_vector.y ()*perpendicular_vector.y () +
            perpendicular_vector.z ()*perpendicular_vector.z () >
            this->epsilon)
            ?
                QQuaternion (QVector3D::dotProduct (vec_1, vec_2),
                             perpendicular_vector.x (),
                             perpendicular_vector.y (),
                             perpendicular_vector.z ()
                             ).normalized ()
              :
                QQuaternion (1., 0., 0., 0.);
}


QVector3D ArcBall::mapToSphere (QVector2D mouse_coordinate)
{
    const double length =
            mouse_coordinate.x ()*mouse_coordinate.x () +
            mouse_coordinate.y ()*mouse_coordinate.y ();

    // If the length comes back as a
    // negative number we are in trouble,
    // check that and bail out if
    // length!>=0.
    assert (length>0);

    // The mouse_coordinate is in 2d (ie,
    // it is on the screen), byt we need
    // a 3d object. We therefore compute
    // the "mouse_position" on the
    // surface of a sphere.
    QVector3D mouse_position;

    if (length>1.96)
    {
        double l2_norm = sqrt (length);
        mouse_position = QVector3D (mouse_coordinate.x ()/l2_norm,
                                    mouse_coordinate.y ()/l2_norm, 0.);
    }
    else
    {
        mouse_position = QVector3D (mouse_coordinate.x (),
                                    mouse_coordinate.y (),
                                    sqrt (1.96-length));
    }

    return mouse_position;
}
