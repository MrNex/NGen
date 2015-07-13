#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "Matrix.h"

const Vector Vector_ZERO =
{
        3,
        (float*)Vector_ZEROComponents
};

const Vector Vector_E1 =
{
        3,
        (float*)Vector_E1Components
};

const Vector Vector_E2 =
{
        3,
        (float*)Vector_E2Components
};

const Vector Vector_E3 =
{
        3,
        (float*)Vector_E3Components
};


///
//Allocates a Vector
//
//Parameters:
//	dim: the number of components the Vector has
//
//Returns:
//	A pointer to the newly allocated Vector. 
//	Vector is not initialized yet and has no components.
Vector* Vector_Allocate()
{
	Vector* v = (Vector*)malloc(sizeof(Vector));
	v->dimension = 0;
	return v;
}

///
//Initializes a Vector setting all components to 0
//
//Parameters:
//	vec: The Vector to initialize
void Vector_Initialize(Vector* vec, uint16_t dim)
{
	vec->dimension = dim;
	vec->components = (float*)calloc(sizeof(float), vec->dimension);
}

///
//Frees the memory taken by a Vector
//
//Parameters:
//	vec: The Vector which's resources are being freed
void Vector_Free(Vector* vec)
{
	free(vec->components);
	free(vec);
}

///
//Copies a Vector to an array
//
//Parameters:
//	dest: The destination of the copy
//	src: the Vector to copy
//	dim: The dimension of the Vector being copied
void Vector_CopyArray(float* dest, const float* src, const uint16_t dim)
{
	memcpy(dest, src, sizeof(float)* dim);
}
//Checks for errors then calls Vector_CopyArray
void Vector_Copy(Vector* dest, const Vector* src)
{
	if (dest->dimension != src->dimension)
	{
		printf("Vector_Copy failed! Destination and source not of equal dimension. Vector not copied.\n");
	}
	else
	{
		Vector_CopyArray(dest->components, src->components, src->dimension);
	}
}

///
//Sets all components of the Vector to 0
//
//Parameters:
//	vec: The Vector being zeroed
//	dim: The number of components the Vector has
void Vector_ZeroArray(float* vec, const uint16_t dim)
{
	for (uint16_t i = 0; i < dim; i++)
		vec[i] = 0.0f;
}
//Calls Vector_ZeroArray
void Vector_Zero(Vector* vec)
{
	Vector_ZeroArray(vec->components, vec->dimension);
}

///
//Determines the magniude^2 of a vector
//
//Parameters:
//	vec: The vector to find the mag squared of
//	dim: The number of components in the vector
float Vector_GetMagSqFromArray(const float* vec, const uint16_t dim)
{
	float mag = 0.0f;
	for (uint16_t i = 0; i < dim; i++)
		mag += pow(vec[i], 2.0f);
	return mag;
}

float Vector_GetMagSq(const Vector* vec)
{
	return Vector_GetMagSqFromArray(vec->components, vec->dimension);
}

///
//Determines the magnitude of a Vector
//
//Parameters:
//	vec: The Vector to find the magnitude of
float Vector_GetMag(const Vector* vec)
{
	return Vector_GetMagFromArray(vec->components, vec->dimension);
}

///
//Determines the magnitude of a Vector
//
//Parameters:
//	vec: The Vector to find the magnitude of
//	dim: the number of components in the Vector
float Vector_GetMagFromArray(const float* vec, const uint16_t dim)
{
	float mag = Vector_GetMagSqFromArray(vec, dim);
	return sqrtf(mag);
}

///
//Normalizes a Vector
//calls Vector_NormalizeArray
//
//Parameters:
//	vec: the components to normalize
//	dim: the dimension of the Vector
void Vector_Normalize(Vector* vec)
{
	Vector_NormalizeArray(vec->components, vec->dimension);
}

///
//Normalizes a Vector
//
//Parameters:
//	vec: the components to normalize
//	dim: the dimension of the Vector
void Vector_NormalizeArray(float* vec, const uint16_t dim)
{
	float mag = Vector_GetMagFromArray(vec, dim);
	if(mag != 0.0f)
	{
		for (int i = 0; i < dim; i++)
		vec[i] /= mag;
	}
}

///
//Gets the dot product of two arrays
//
//Parameters:
//	vec1: Vector being dotted
//	vec2: Other Vector being dotted
//	dim: Dimension of the two Vectors
//
//Returns:
//	Float representing value of the dot product
float Vector_DotProductArray(const float* vec1, const float* vec2, const uint16_t dim)
{
	float dotProd = 0.0f;

	for (int i = 0; i < dim; i++)
		dotProd += (vec1[i] * vec2[i]);

	return dotProd;
}
//Checks for errors then calls Vector_DotProductArray
//Returns 0 on error
float Vector_DotProduct(const Vector* vec1, const Vector* vec2)
{
	if (vec1->dimension != vec2->dimension)
	{
		printf("Vector_DotProduct failed. Operands are not of equal dimension. Returning 0.");
		return 0.0f;
	}
	else
	{
		return Vector_DotProductArray(vec1->components, vec2->components, vec1->dimension);
	}
}

///
//Gets a vector perpendicular to all other given vectors within a certain subspace.
//
//PArameters:
//	dest: The destination of the cross product result
//	dim: The dimension of the resulting vector
//	vectors: an array of dim - 1 vectors of dimension dim
void Vector_CrossProductArray(float* dest, const uint16_t dim, float** vectors)
{
	//Construct Crossproduct Matrix
	float* crossMatrix = (float*)malloc(sizeof(float) * dim * dim);
	for(uint16_t i = 0; i < dim - 1; i++)
	{
		for(unsigned int j = 0; j < dim; j++)
		{
			*Matrix_IndexArray(crossMatrix, i + 1, j, dim) = vectors[i][j];
		}
	}
	for(unsigned int i = 0; i < dim; i++)
	{
		float* minor = (float*) malloc(sizeof(float) * (dim - 1) * (dim - 1));
		Matrix_GetMinorArray(minor, crossMatrix, 0, i, dim, dim);
		dest[i] = powf(-1.0f, (float)(i + 2)) * Matrix_GetDeterminateArray(minor, dim - 1, dim - 1);
		free(minor);
	}
	free(crossMatrix);
}
///
//Checks for errors then calls CVector_CrossProductArray
//
//Parameters:
//	dest: The destination of cross product result
//	...: dest->dim - 1 vectors of dimension dest->dim
void Vector_CrossProduct(Vector* dest, ...)
{
	int argc = dest->dimension - 1;
	va_list argsList;
	va_start (argsList, dest);	//Initialize arguments list

	float** vectors = (float**)malloc(sizeof(float*) * (argc));
	for(int i = 0; i < argc; i++)
	{
		Vector* v = va_arg(argsList, Vector*);
		vectors[i] = v->components;
	}

	Vector_CrossProductArray(dest->components, dest->dimension, vectors);
	va_end(argsList);
	free(vectors);
}

///
//Calculates the angle between two equally dimensioned arrays
//
//Parameters:
//	vec1: The first vector to find the angle between
//	vec2: The second vector to find the angle between
//	dim: the dimension of the two vectors
//
//returns:
//	Angle in radians
float Vector_GetAngleArray(const float* vec1, const float* vec2, const uint16_t dim)
{
	float dotProduct = Vector_DotProductArray(vec1, vec2, dim);
	return 	acosf(dotProduct / (Vector_GetMagFromArray(vec1, dim) * Vector_GetMagFromArray(vec2, dim)));
}
//Checks for errors then calls CVector_GetAngleArray
float Vector_GetAngle(const Vector* vec1, const Vector* vec2)
{
	if(vec1->dimension == vec2->dimension)
		return Vector_GetAngleArray(vec1->components, vec2->components, vec1->dimension);
	else
	{
		printf("CVector_GetAngle failed! Vectors are not of equal dimension! Returning 0.\n");
		return 0.0f;
	}
}

///
//Increments a Vector by another Vector
//
//Parameters:
//	dest: the Vector getting incremented
//	src: The addend Vector, or the Vector being added to the destination
//	dim: The number of components in the Vectors
void Vector_IncrementArray(float* dest, const float* src, const uint16_t dim)
{
	for (int row = 0; row < dim; row++)
	{
		dest[row] += src[row];
	}
}
//Checks for errors, then calls Vector_IncrementArray
void Vector_Increment(Vector* dest, const Vector* src)
{
	if (dest->dimension == src->dimension)
	{
		Vector_IncrementArray(dest->components, src->components, dest->dimension);
	}
	else
	{
		printf("Vector_Imcrement Failed. Destination and source of unequal dimensions. Sum not computed\n");
	}

}



///
//Increments a Vector by another Vector
//
//Parameters:
//	dest: the Vector getting incremented
//	src: The addend Vector, or the Vector being added to the destination
//	dim: The number of components in the Vectors
void Vector_DecrementArray(float* dest, const float* src, const uint16_t dim)
{
	for (int row = 0; row < dim; row++)
	{
		dest[row] -= src[row];
	}
}
//Checks for errors, then calls Vector_DecrementArray
void Vector_Decrement(Vector* dest, const Vector* src)
{
	if (dest->dimension == src->dimension)
	{
		Vector_DecrementArray(dest->components, src->components, dest->dimension);
	}
	else
	{
		printf("Vector_Decrement Failed. Destination and source of unequal dimensions. Difference not computed.\n");
	}
}


///
//Scales a Vector by a scalar
//
//Parameters:
//	vec: The Vector to be scaled
//	scaleValue: the scale factor
//	dim: the dimension of the Vector being scaled
void Vector_ScaleArray(float* vec, const float scaleValue, const uint16_t dim)
{
	for (uint16_t row = 0; row < dim; row++)
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
//Gets the scalar product of a specified Vector and a scalar
//
//Parameters:
//	dest: The address to hold the scaled Vector product
//	src: the initial Vector to be scaled
//	scaleValue: the amount by which to scale the initial Vector
//	dim: The number of components in the initial Vector
void Vector_GetScalarProductFromArray(float* dest, const float* src, const float scaleValue, const uint16_t dim)
{
	for (uint16_t row = 0; row < dim; row++)
	{
		dest[row] = src[row] * scaleValue;
	}
}
//Checks for errors then calls Vector_GetScalarProductFromArray
void Vector_GetScalarProduct(Vector* dest, const Vector* src, const float scaleValue)
{
	if (dest->dimension == src->dimension)
	{
		Vector_GetScalarProductFromArray(dest->components, src->components, scaleValue, dest->dimension);
	}
	else
	{
		printf("Vector_GetScalarProduct failed. Destination and source of unequal dimension. Scalar product not computed.\n");
	}
}

///
//Adds together two Vectors retrieving the sum
//
//Parameters:
//	dest: the destination of the sum Vector
//	vec1: the first addend Vector
//	vec2: the second addend Vector
//	dim: the dimension of the Vectors being added
void Vector_AddArray(float* dest, const float* vec1, const float* vec2, const uint16_t dim)
{
	for (uint16_t row = 0; row < dim; row++)
	{
		dest[row] = vec1[row] + vec2[row];
	}
}
//Checks for errors then calls Vector_AddArray
void Vector_Add(Vector* dest, const Vector* vec1, const Vector* vec2)
{
	if (dest->dimension == vec1->dimension && dest->dimension == vec2->dimension)
	{
		Vector_AddArray(dest->components, vec1->components, vec2->components, dest->dimension);
	}
	else
	{
		printf("Vector_Add Failed. Destination and addends of unequal dimension. Sum not computed.\n");
	}
}

///
//Subtracts two Vectors retrieving the difference
//
//Parameters:
//	dest: the destination of the sum Vector
//	vec1: the first addend Vector
//	vec2: the second addend Vector
//	dim: the dimension of the Vectors being added
void Vector_SubtractArray(float* dest, const float* vec1, const float* vec2, const uint16_t dim)
{
	for (uint16_t row = 0; row < dim; row++)
	{
		dest[row] = vec1[row] - vec2[row];
	}
}
//Checks for errors then calls Vector_SubtractArray
void Vector_Subtract(Vector* dest, const Vector* vec1, const Vector* vec2)
{
	if (dest->dimension == vec1->dimension && dest->dimension == vec2->dimension)
	{
		Vector_SubtractArray(dest->components, vec1->components, vec2->components, dest->dimension);
	}
	else
	{
		printf("Vector_Subtract failed. Destination, minuend, and subtractend of unequal dimension. Difference not computed.\n");
	}
}

///
//Projects vec1 onto vec2 and stores the result in the destination
//
//Parameters:
//	dest: the destination of the projected vector
//	vec1: The vector being projected
//	vec2: The vector representing the projection axis
//	dim: the dimension of the vectors
void Vector_GetProjectionArray(float* dest, const float* vec1, const float* vec2, const uint16_t dim)
{
	float numerator = Vector_DotProductArray(vec1, vec2, dim);
	float denominator = Vector_GetMagSqFromArray(vec2, dim);
	
	//Return 0 vector
	if(numerator == 0.0f || denominator == 0.0f)
	{
		for(int i = 0; i < dim; i++)
		{
			dest[i] = 0.0f;
		}
	}
	else
	{
		Vector_GetScalarProductFromArray(dest, vec2, numerator / denominator, dim);
	}
}
//Calls Vector_ProjectArray
void Vector_GetProjection(Vector* dest, const Vector* vec1, const Vector* vec2)
{
	Vector_GetProjectionArray(dest->components, vec1->components, vec2->components, dest->dimension);
}

///
//Projects vec1 onto vec2 storing the result in vec1
//
//Parameters:
//	vec1: The vector being projected
//	vec2: The vector representing the projection axis
//	dim: The dimension of the vectors
void Vector_ProjectArray(float* vec1, const float* vec2, const uint16_t dim)
{
	float numerator = Vector_DotProductArray(vec1, vec2, dim);
	float denominator = Vector_GetMagSqFromArray(vec2, dim);
	
	//Return 0 vector
	if(numerator == 0.0f || denominator == 0.0f)
	{
		for(int i = 0; i < dim; i++)
		{
			vec1[i] = 0.0f;
		}
	}
	else
	{
		Vector_GetScalarProductFromArray(vec1, vec2, numerator / denominator, dim);
	}
}
//Calls Vector_ProjectArray
void Vector_Project(Vector* vec1, const Vector* vec2)
{
	Vector_ProjectArray(vec1->components, vec2->components, vec1->dimension);
}

///
//Prints out the contents of a Vector in a transpose format
//
//Parameters:
//	vec: the Vector to print
//	dim: the number of components in the Vector
void Vector_PrintTransposeArray(const float* vec, const uint16_t dim)
{
	printf("[%f", vec[0]);
	for (int row = 1; row < dim; row++)
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
//Prints out the contents of a Vector
//
//Parameters:
//	vec: the Vector to print
//	dim: the number of components in the Vector
void Vector_PrintArray(const float* vec, const uint16_t dim)
{
	for (int row = 0; row < dim; row++)
	{
		printf("%f\n", vec[row]);
	}
}
//Calls Vector_PrintArray
void Vector_Print(const Vector* vec)
{
	Vector_PrintArray(vec->components, vec->dimension);
}

