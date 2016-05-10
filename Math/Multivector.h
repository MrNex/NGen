#ifndef MULTIVECTOR_H
#define MULTIVECTOR_H
#include <stdlib.h>

float* Multivector_Allocate(size_t dim);

float Multivector_DetermineProductTermSign(size_t basisA, size_t gradeA, size_t basisB, size_t gradeB, size_t dimension);

void Multivector_GetProduct(float* dest, size_t dimDest, float* lhs, size_t dimLhs, float* rhs, size_t dimRhs);


#endif
