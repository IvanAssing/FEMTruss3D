#ifndef POINT3DBUFFER_H
#define POINT3DBUFFER_H

#include <ostream>

class Point3DBuffer
{
public:
    double *data;
    int count;
    int max;

public:
    Point3DBuffer(int buffersize);
    int addPoint(double x, double y, double z);

    friend std::ostream& operator<< (std::ostream& stream, const Point3DBuffer& buffer);
};

#endif // POINT3DBUFFER_H
