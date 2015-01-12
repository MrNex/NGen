#include <math.h>
#include <stdio.h>

#include "Vector.h"

///
//Determines the magnitude of a vector
//
//Parameters:
//	vec: The vector to find the magnitude of
//	dim: the number of components in the vector
float Vector_GetMag(const float* vec, const int dim)
{
	float mag = 0.0f;
	for(int i = 0; i < dim; i++) 
		mag += pow(vec[i], 2.0f);
	return sqrtf(mag);
}

///
//Normalizes a vector
//
//Parameters:
//	vec: the components to normalize
//	dim: the dimension of the vector
void Vector_Normalize(float* vec, const int dim)
{
	float mag = Vector_GetMag(vec, dim);
	for(int i = 0; i < dim; i++)
		vec[i] /= mag;
}

///
//Multiplies a vector by a matrix storing the result in a new vector
//
//Parameters:
//	dest: the destination of the solution
//	mat: The matrix being applied to the vector
//	vec: The vector getting acted upon by the matrix
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
void Vector_Multiply(float* dest, const float* mat, const float* vec, const int numRows, const int numCols)
{
	for(int column = 0; column < numCols; column++)
	{
		float dot = 0.0f;
		for(int row = 0; row < numRows; row++)
		{
			dot += mat[(row * numCols) + column] * vec[row];
		}
		dest[column] = dot;
	}
}

///
//Increments a vector by another vector
//
//Parameters:
//	dest: the vector getting incremented
//	src: The addend vector, or the vector being added to the destination
//	dim: The number of components in the vectors
void Vector_Increment(float* dest, const float* src, const int dim)
{
	for(int column = 0; column < dim; column++)
	{
		dest[column] += src[column];
	}
}

///
//Increments a vector by another vector
//
//Parameters:
//	dest: the vector getting incremented
//	src: The addend vector, or the vector being added to the destination
//	dim: The number of components in the vectors
void Vector_Decrement(float* dest, const float* src, const int dim)
{
	for(int column = 0; column < dim; column++)
	{
		dest[column] -= src[column];
	}
}

///
//Scales a vector by a scalar
//
//Parameters:
//	vec: The vector to be scaled
//	scaleValue: the scale factor
//	dim: the dimension of the vector being scaled
void Vector_Scale(float* vec, const float scaleValue, const int dim)
{
	for(int column = 0; column < dim; column++)
	{
		vec[column] *= scaleValue;
	}
}

///
//Gets the scalar product of a specified vector and a scalar
//
//Parameters:
//	dest: The address to hold the scaled vector product
//	src: the initial vector to be scaled
//	scaleValue: the amount by which to scale the initial vector
//	dim: The number of components in the initial vector
void Vector_GetScalarProduct(float* dest, const float* src, const float scaleValue, const int dim)
{
	for(int column = 0; column < dim; column++)
	{
		dest[column] = src[column] * scaleValue;
	}
}

///
//Adds together two vectors retrieving the sum
//
//Parameters:
//	dest: the destination of the sum vector
//	vec1: the first addend vector
//	vec2: the second addend vector
//	dim: the dimension of the vectors being added
void Vector_Add(float* dest, const float* vec1, const float* vec2, const int dim)
{
	for(int column = 0; column < dim; column++)
	{
		dest[column] = vec1[column] + vec2[column];
	}
}

void Vector_Subtract(float* dest, const float* vec1, const float* vec2, const int dim)
{
	for(int column = 0; column < dim; column++)
	{
		dest[column] = vec1[column] - vec2[column];
	}
}

///
//Prints out the contents of a vector
//
//Parameters:
//	vec: the vector to print
//	dim: the number of components in the vector
void Vector_Print(const float* vec, const int dim)
{
	printf("[%f", vec[0]);
	for(int column = 1; column < dim; column++)
	{
		printf(", %f", vec[column]);
	}
	printf("]\n");
}
