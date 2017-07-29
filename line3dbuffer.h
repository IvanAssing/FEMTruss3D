#ifndef LINE3DBUFFER_H
#define LINE3DBUFFER_H

#include <ostream>

class Line3DBuffer
{
public:
    int *data;
    int count;
    int max;

public:
    Line3DBuffer(int buffersize);
    int addLine(int p1, int p2, int layer);

    friend std::ostream& operator<< (std::ostream& stream, const Line3DBuffer& buffer);
};

#endif // LINE3DBUFFER_H
