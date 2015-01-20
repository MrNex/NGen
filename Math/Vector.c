#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "Vector.h"

///
//Allocates a vector
//
//Parameters:
//	dim: the number of components the vector has
//
//Returns:
//	A pointer to the newly allocated vector. 
//	Vector is not initialized yet and has no components.
Vector* Vector_Allocate(int dim)
{
	Vector* v = (Vector*)malloc(sizeof(Vector));
	v->dimension = dim;
	return v;
}

///
//Initializes a vector setting all components to 0
//
//Parameters:
//	vec: The vector to initialize
void Vector_Initialize(Vector* vec)
{
	vec->components = (float*)calloc(sizeof(float), vec->dimension);
}

///
//Frees the memory taken by a vector
//
//Parameters:
//	vec: The Vector which's resources are being freed
void Vector_Free(Vector* vec)
{
	free(vec->components);
	free(vec);
}

///
//Copies a vector to an array
//
//Parameters:
//	dest: The destination of the copy
//	src: the vector to copy
//	dim: The dimension of the vector being copied
void Vector_CopyArray(float* dest, const float* src, const int dim)
{
	memcpy(dest, src, sizeof(float) * dim);
}
//Checks for errors then calls Vector_CopyArray
void Vector_Copy(Vector* dest, const Vector* src)
{
	if(dest->dimension != src->dimension)
	{
		printf("Vector_Copy failed! Destination and source not of equal dimension. Vector not copied.\n");
	}
	else
	{
		Vector_CopyArray(dest->components, src->components, src->dimension);
	}
}

///
//Sets all components of the vector to 0
//
//Parameters:
//	vec: The vector being zeroed
//	dim: The number of components the vector has
void Vector_ZeroArray(float* vec, const int dim)
{
	for(int i = 0; i < dim; i++)
		vec[i] = 0;
}
//Calls Vector_ZeroArray
void Vector_Zero(Vector* vec)
{
	Vector_ZeroArray(vec->components, vec->dimension);
}

///
//Determines the magnitude of a vector
//
//Parameters:
//	vec: The vector to find the magnitude of
float Vector_GetMag(const Vector* vec)
{
	return Vector_GetMagFromArray(vec->components, vec->dimension);
}

///
//Determines the magnitude of a vector
//
//Parameters:
//	vec: The vector to find the magnitude of
//	dim: the number of components in the vector
float Vector_GetMagFromArray(const float* vec, const int dim)
{
	float mag = 0.0f;
	for(int i = 0; i < dim; i++) 
		mag += pow(vec[i], 2.0f);
	return sqrtf(mag);
}

///
//Normalizes a vector
//calls Vector_NormalizeArray
//
//Parameters:
//	vec: the components to normalize
//	dim: the dimension of the vector
void Vector_Normalize(Vector* vec)
{
	Vector_NormalizeArray(vec->components, vec->dimension);
}

///
//Normalizes a vector
//
//Parameters:
//	vec: the components to normalize
//	dim: the dimension of the vector
void Vector_NormalizeArray(float* vec, const int dim)
{
	float mag = Vector_GetMagFromArray(vec, dim);
	for(int i = 0; i < dim; i++)
		vec[i] /= mag;
}

///
//Gets the dot product of two arrays
//
//Parameters:
//	vec1: Vector being dotted
//	vec2: Other vector being dotted
//	dim: Dimension of the two vectors
//
//Returns:
//	Float representing value of the dot product
float Vector_DotProductArray(const float* vec1, const float* vec2, const int dim)
{
	float dotProd = 0.0f;

	for(int i = 0; i < dim; i++)
		dotProd += vec1[i] * vec2[i];

	return dotProd;
}
//Checks for errors then calls Vector_DotProductArray
//Returns 0 on error
float Vector_DotProduct(const Vector* vec1, const Vector* vec2)
{
	if(vec1->dimension != vec2->dimension)
	{
		printf("Vector_DotProduct failed. Operands are not of equal dimension. Returning 0.");
		return 0.0f;
	}
	else
		return Vector_DotProductArray(vec1->components, vec2->components, vec1->dimension);
}

///
//Increments a vector by another vector
//
//Parameters:
//	dest: the vector getting incremented
//	src: The addend vector, or the vector being added to the destination
//	dim: The number of components in the vectors
void Vector_IncrementArray(float* dest, const float* src, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		dest[row] += src[row];
	}
}
//Checks for errors, then calls Vector_IncrementArray
void Vector_Increment(Vector* dest, const Vector* src)
{
	if(dest->dimension == src->dimension)
	{
		Vector_IncrementArray(dest->components, src->components, dest->dimension);
	}
	else
	{
		printf("Vector_Imcrement Failed. Destination and source of unequal dimensions. Sum not computed\n");
	}

}

///
//Increments a vector by another vector
//
//Parameters:
//	dest: the vector getting incremented
//	src: The addend vector, or the vector being added to the destination
//	dim: The number of components in the vectors
void Vector_DecrementArray(float* dest, const float* src, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		dest[row] -= src[row];
	}
}
//Checks for errors, then calls Vector_DecrementArray
void Vector_Decrement(Vector* dest, const Vector* src)
{
	if(dest->dimension == src->dimension)
	{
		Vector_DecrementArray(dest->components, src->components, dest->dimension);
	}
	else
	{
		printf("Vector_Decrement Failed. Destination and source of unequal dimensions. Difference not computed.\n");
	}
}


///
//Scales a vector by a scalar
//
//Parameters:
//	vec: The vector to be scaled
//	scaleValue: the scale factor
//	dim: the dimension of the vector being scaled
void Vector_ScaleArray(float* vec, const float scaleValue, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		vec[row] *= scaleValue;
	}
}
//Calls Vector_ScaleArray
void Vector_Scale(Vector* vec, const float scaleValue)
{
	Vector_ScaleArray(vec->components, scaleValue, vec->dimension);
}

///
//Gets the scalar product of a specified vector and a scalar
//
//Parameters:
//	dest: The address to hold the scaled vector product
//	src: the initial vector to be scaled
//	scaleValue: the amount by which to scale the initial vector
//	dim: The number of components in the initial vector
void Vector_GetScalarProductFromArray(float* dest, const float* src, const float scaleValue, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		dest[row] = src[row] * scaleValue;
	}
}
//Checks for errors then calls Vector_GetScalarProductFromArray
void Vector_GetScalarProduct(Vector* dest, const Vector* src, const float scaleValue)
{
	if(dest->dimension == src->dimension)
	{
		Vector_GetScalarProductFromArray(dest->components, src->components, scaleValue, dest->dimension);
	}
	else
	{
		printf("Vector_GetScalarProduct failed. Destination and source of unequal dimension. Scalar product not computed.\n");
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
void Vector_AddArray(float* dest, const float* vec1, const float* vec2, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		dest[row] = vec1[row] + vec2[row];
	}
}
//Checks for errors then calls Vector_AddArray
void Vector_Add(Vector* dest, const Vector* vec1, const Vector* vec2)
{
	if(dest->dimension == vec1->dimension && dest->dimension == vec2->dimension)
	{
		Vector_AddArray(dest->components, vec1->components, vec2->components, dest->dimension);	
	}
	else
	{
		printf("Vector_Add Failed. Destination and addends of unequal dimension. Sum not computed.\n");
	}
}

///
//Subtracts two vectors retrieving the difference
//
//Parameters:
//	dest: the destination of the sum vector
//	vec1: the first addend vector
//	vec2: the second addend vector
//	dim: the dimension of the vectors being added
void Vector_SubtractArray(float* dest, const float* vec1, const float* vec2, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		dest[row] = vec1[row] - vec2[row];
	}
}
//Checks for errors then calls Vector_SubtractArray
void Vector_Subtract(Vector* dest, const Vector* vec1, const Vector* vec2)
{
	if(dest->dimension == vec1->dimension && dest->dimension == vec2->dimension)
	{
		Vector_SubtractArray(dest->components, vec1->components, vec2->components, dest->dimension);
	}
	else
	{
		printf("Vector_Subtract failed. Destination, minuend, and subtractend of unequal dimension. Difference not computed.\n");
	}
}

///
//Prints out the contents of a vector in a transpose format
//
//Parameters:
//	vec: the vector to print
//	dim: the number of components in the vector
void Vector_PrintTransposeArray(const float* vec, const int dim)
{
	printf("[%f", vec[0]);
	for(int row = 1; row < dim; row++)
	{
		printf(", %f", vec[row]);
	}
	printf("]\n");
}

//Calls Vector_PrintTransposeArray
void Vector_PrintTranspose(const Vector* vec)
{
	Vector_PrintTransposeArray(vec->components, vec->dimension);
}

///
//Prints out the contents of a vector
//
//Parameters:
//	vec: the vector to print
//	dim: the number of components in the vector
void Vector_PrintArray(const float* vec, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		printf("%f\n", vec[row]);
	}
}
//Calls Vector_PrintArray
void Vector_Print(const Vector* vec)
{
	Vector_PrintArray(vec->components, vec->dimension);
}
