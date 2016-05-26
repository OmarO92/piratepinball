
#ifndef _VECTOR_H_

#define _VECTOR_H_

#include<cmath>

struct Vec
{
    double vec[3];
    double& operator[] (const int index) { return this->vec[index]; };

};


double RadiansToDegrees(double);
double DegreesToRadians(double); 

void MakeVector(double, double, double, Vec&);
void VecNormalize(Vec &, Vec &);
double VecDot(Vec &, Vec &);
double VecProject(Vec &, Vec &);
double VecAngleBtn(Vec &, Vec &);
void VecScale(Vec &, double scale, Vec &);
void VecRotate(Vec &, double angle, Vec &);
void VecAdd(Vec &, Vec &, Vec &);
double VecMagnitude(Vec &);
void VecBtn(Vec &, Vec &, Vec &);
void VecCopy(Vec&, Vec&);
#endif
