#include "point3dbuffer.h"

#include <iomanip>
#include <cmath>

#define ptol 1.e-3

Point3DBuffer::Point3DBuffer(int buffersize)
{
    count = 0;
    max = buffersize;
    data = new double[3*buffersize];
}

int Point3DBuffer::addPoint(double x, double y, double z)
{
    for(int i=0; i<count; i++)
        if(fabs(x-data[3*i])<ptol) // x
            if(fabs(y-data[3*i+1])<ptol) // y
                if(fabs(z-data[3*i+2])<ptol) // z
                    return i;

    // add point
    data[3*count] = x;
    data[3*count+1] = y;
    data[3*count+2] = z;
    count++;

    return count-1;

}
std::ostream& operator<< (std::ostream& stream, const Point3DBuffer& buffer)
{
    for(int i=0; i<buffer.count; i++)
        stream<<i<<","<<buffer.data[3*i]<<","<<buffer.data[3*i+1]<<","<<buffer.data[3*i+2]<<std::endl;
}
