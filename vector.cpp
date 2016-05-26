#include "vector.h"

double RadiansToDegrees(double a) {return a * 180.0 / M_PI;}
double DegreesToRadians(double a) {return a * M_PI / 180.0;}

//Vector Functions
double VecMagnitude(Vec &vec)
{
    return sqrt(VecDot(vec, vec));

}

void MakeVector(double x, double y, double z, Vec &v)
{
    v[0] = x;
    v[1] = y;
    v[2] = z;
    
}
void VecCopy(Vec &dest, Vec &source)
{
    dest[0] = source[0];
    dest[1] = source[1];
}
double VecDot(Vec &vec1, Vec &vec2)
{
    return vec1[0] * vec2[0] + vec1[1] * vec2[1];
}

double VecProject(Vec &vec1, Vec &vec2)
{
    return VecDot(vec1, vec2) / VecMagnitude(vec2);
}

double VecAngleBtn(Vec &vec1, Vec &vec2)
{
    return RadiansToDegrees(acos(VecDot(vec1, vec2) / 
        (VecMagnitude(vec1) * VecMagnitude(vec2))));
}

void VecScale(Vec &vec, double scale, Vec &out)
{
    out[0] = vec[0] * scale;
    out[1] = vec[1] * scale;
}

void VecRotate(Vec &vec, double angle, Vec &out)
{
    angle = DegreesToRadians(angle);
    float x = vec[0], y = vec[1];
    out[0] = x * cos(angle) - y * sin(angle);
    out[1] = x * sin(angle) + y * cos(angle);
}
void VecAdd(Vec &a, Vec &b, Vec &out)
{
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
}

void VecBtn(Vec &a, Vec &b, Vec &out)
{
    out[0] = b[0] - a[0];
    out[1] = b[1] - a[1];
    out[2] = b[2] - a[2];
}

void VecNormalize(Vec &v, Vec &out)
{
    double dist = v[0]*v[0]+v[1]*v[1]+v[2]*v[2];
    if (dist==0.0)
        return;
    double len = 1.0 / sqrt(dist);
    out[0] = v[0] * len;
    out[1] = v[1] * len;
    out[2] = v[2] * len;
}
