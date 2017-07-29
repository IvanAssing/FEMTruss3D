#include "dxfreader.h"
#include <iomanip>

DXFReader::DXFReader()
{
    flog = new std::ofstream("log.txt");
    pbuffer = new Point3DBuffer(100000);
    lbuffer = new Line3DBuffer(100000);

}

bool DXFReader::readfile(char *file)
{
    dxf = new DL_Dxf();
    if (!dxf->in(file, this))
    {
        std::cerr << file << " could not be opened.\n";
        return true;
    }

    (*flog)<<"\n Points:\n";
    (*flog)<<(*pbuffer);
    (*flog)<<"\n\n Lines:\n";
    (*flog)<<(*lbuffer);
    return false;
}


DXFReader::~DXFReader()
{
    if(dxf) delete dxf;
    flog->close();
    if(flog) delete flog;
}

void DXFReader::addPoint(const DL_PointData& data) {
    //    printf("POINT    (%6.3f, %6.3f, %6.3f)\n",
    //           data.x, data.y, data.z);
    //    printAttributes();
}

void DXFReader::addLine(const DL_LineData& data)
{
    int layer = attributes.getColor();
    if(layer>0 && layer<256) // basic color AutoCAD
    {
        int p1 = pbuffer->addPoint(data.x1, data.y1, data.z1);
        int p2 = pbuffer->addPoint(data.x2, data.y2, data.z2);

        lbuffer->addLine(p1, p2, layer);
    }
}


