#include "Multivector.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

float* Multivector_Allocate(size_t dim)
{
	return calloc(sizeof(float), (1 << dim));
}


float Multivector_DetermineProductTermSign(size_t basisA, size_t gradeA, size_t basisB, size_t gradeB, size_t dimension)
{
	size_t totalBlades = gradeA + gradeB;
	size_t* orderedDimensions = malloc(sizeof(size_t) * totalBlades);
	size_t indexA = 0;
	size_t indexB = 0;
	for(size_t i = 0; i < dimension; i++)
	{
		if(basisA & (1 << i)) orderedDimensions[indexA++] = i;
		if(basisB & (1 << i)) orderedDimensions[gradeA + indexB++] = i;
	}

	int power = 0;

	size_t i = 0;
	size_t j = gradeA;
	size_t offset = 0;

	while((i < gradeA + offset) && (j < (gradeB + gradeA)))
	{
		if(orderedDimensions[i] == orderedDimensions[j])
		{
			power += (j - i) - 1;

			size_t save = orderedDimensions[j];
			memmove(orderedDimensions + i + 2, orderedDimensions + i + 1, (j - i) - 1);
			orderedDimensions[j] = save;

			i += 2;
			++j;
			++offset;
		}
		else if(orderedDimensions[j] < orderedDimensions[i])
		{
			power += (j - i);
			size_t save = orderedDimensions[i];
			memmove(orderedDimensions + i, orderedDimensions + i + 1, j - i);
			orderedDimensions[j] = save;

			++i;
			++j;
			++offset;
		}
		else
		{
			++i;
		}
	}

	if(power > 0)
	{
		return powf(-1.0f, (float)power);
	}
	return 1.0f;
}

static unsigned int Multivector_GetBasisKBladeGrade(size_t blade)
{
		static const unsigned int lookupTable[] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4};
		unsigned int sum = 0;
		for(size_t i = 0; i < 4; i++)
		{
			sum += lookupTable[blade & 0x0F] + lookupTable[(blade >> (4 * i)) & 0x0F];
		}
		return sum;
}

void Multivector_GetProduct(float* dest, size_t dimDest, float* lhs, size_t dimLhs, float* rhs, size_t dimRhs)
{
	size_t destNumComps = (1 << dimDest);
	if(dimLhs <= dimRhs)
	{
		size_t opNumComps = (1 << dimLhs);
		for(size_t i = 0; i < destNumComps; i++)
		{
			dest[i] = 0.0f;

			for(size_t j = 0; j < opNumComps; j++)
			{
				size_t gradeA = Multivector_GetBasisKBladeGrade(j);
				size_t gradeB = Multivector_GetBasisKBladeGrade(i ^ j);
				float sign = Multivector_DetermineProductTermSign(j, gradeA, i ^ j, gradeB, dimDest);
				float scalar = lhs[j] * rhs[i ^ j];
				dest[i] += sign * scalar;
			}
		}
	}
	else
	{

		size_t opNumComps = (1 << dimRhs);

		for(size_t i = 0; i < destNumComps; i++)
		{
			dest[i] = 0.0f;

			for(size_t j = 0; j < opNumComps; j++)
			{
				size_t gradeA = Multivector_GetBasisKBladeGrade(i ^ j);
				size_t gradeB = Multivector_GetBasisKBladeGrade(j);
				float sign = Multivector_DetermineProductTermSign(i ^ j, gradeA, j, gradeB, dimDest);
				float scalar = lhs[i ^ j] * rhs[j];
				dest[i] += sign * scalar;
			}
		}
	}
}

/*
	float* a = Multivector_Allocate(2);
	float* b = Multivector_Allocate(2);
	float* c = Multivector_Allocate(2);
	float* d = Multivector_Allocate(2);
	float* e = Multivector_Allocate(2);

	float hAngle = 3.14159 / 100.0f;
	a[0] = cosf(hAngle);
	a[3] = -sinf(hAngle);
	
	c[0] = cosf(hAngle);
	c[3] = sinf(hAngle);

	b[1] = 1.0f;
	b[2] = 0.0f;

	for(int i = 0; i < 2000; i++)
	{
		Multivector_GetProduct(d, 2, a, 2, b, 2);
		Multivector_GetProduct(e, 2, d, 2, c, 2);
		b[1] = e[1];
		b[2] = e[2];

		printf("%f\t%f\n", b[1], b[2]);
		d[0] = e[0] = d[1] = e[1] = d[2] = e[2] = d[3] = e[3] = 0.0f;
	}

	return 0;

}
*/
