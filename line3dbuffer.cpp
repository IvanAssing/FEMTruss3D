#include "line3dbuffer.h"

#include <iomanip>

Line3DBuffer::Line3DBuffer(int buffersize)
{
    count = 0;
    max = buffersize;
    data = new int[3*buffersize];
}

int Line3DBuffer::addLine(int p1, int p2, int layer)
{
    for(int i=0; i<count; i++)
        if(p1 == data[3*i]) // p1
            if(p2 == data[3*i+1]) // p2
                return i;

    // add line
    data[3*count] = p1;
    data[3*count+1] = p2;
    data[3*count+2] = layer;
    count++;

    return count-1;
}

std::ostream& operator<< (std::ostream& stream, const Line3DBuffer& buffer)
{
    for(int i=0; i<buffer.count; i++)
        stream<<i<<","<<buffer.data[3*i]<<","<<buffer.data[3*i+1]<<","<<buffer.data[3*i+2]<<std::endl;
}
