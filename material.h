#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

class Material
{
public:
    double E;
    double A;
    double I;
    std::string name;

    Material(std::string name, double E, double A, double I = 0.0);
};

#endif // MATERIAL_H
