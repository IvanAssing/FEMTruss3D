#ifndef DXFREADER_H
#define DXFREADER_H

#include <dl_dxf.h>
#include <dl_creationadapter.h>
#include <fstream>

#include "point3dbuffer.h"
#include "line3dbuffer.h"

class DXFReader : public DL_CreationAdapter
{
private:
    DL_Dxf *dxf;
    std::ofstream *flog;


public:

    Point3DBuffer *pbuffer;
    Line3DBuffer *lbuffer;


    DXFReader();

    bool readfile(char *file);

    virtual void addPoint(const DL_PointData& data);
    virtual void addLine(const DL_LineData& data);

protected:
    ~DXFReader();
};

#endif // DXFREADER_H
