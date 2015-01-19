///
//A vector consists of an array of components,
//And a dimension, or number of components.
typedef struct Vector
{
	int dimension;
	float* components;
}Vector;

///
//Allocates a vector
//
//Parameters:
//	dim: the number of components the vector has
//
//Returns:
//	A pointer to the newly allocated vector. 
//	Vector is not initialized yet and has no components.
Vector* Vector_Allocate(int dim);

///
//Initializes a vector
//
//Parameters:
//	vec: The vector to initialize
void Vector_Initialize(Vector* vec);

///
//Frees the memory taken by a vector
//
//Parameters:
//	vec: The Vector which's resources are being freed
void Vector_Free(Vector* vec);

///
//Copies a vector to an array
//
//Parameters:
//	dest: The destination of the copy
//	src: the vector to copy
//	dim: The dimension of the vector being copied
void Vector_CopyArray(float* dest, const float* src, const int dim);
//Checks for errors then calls Vector_CopyArray
void Vector_Copy(Vector* dest, const Vector* src);


///
//Determines the magnitude of a vector
//
//Parameters:
//	vec: The vector to find the magnitude of
//	dim: the number of components in the vector
float Vector_GetMagFromArray(const float* vec, const int dim);
float Vector_GetMag(const Vector* vec);

///
//Normalizes a vector
//
//Parameters:
//	vec: the components to normalize
//	dim: the dimension of the vector
void Vector_NormalizeArray(float* vec, const int dim);
void Vector_Normalize(Vector* vec);

///
//Multiplies a vector by a matrix storing the result in a new vector
//
//Parameters:
//	dest: the destination of the solution
//	mat: The matrix being applied to the vector
//	vec: The vector getting acted upon by the matrix
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
//void Vector_Multiply(float* dest, const float* mat, const float* vec, const int numRows, const int numCols);

///
//Increments a vector by another vector
//
//Parameters:
//	dest: the vector getting incremented
//	src: The addend vector, or the vector being added to the destination
//	dim: The number of components in the vectors
void Vector_IncrementArray(float* dest, const float* src, const int dim);
void Vector_Increment(Vector* dest, const Vector* src);
void Vector_DecrementArray(float* dest, const float* src, const int dim);
void Vector_Decrement(Vector* dest, const Vector* src);

///
//Scales a vector by a scalar
//
//Parameters:
//	vec: The vector to be scaled
//	scaleValue: the scale factor
//	dim: the dimension of the vector being scaled
void Vector_ScaleArray(float* vec, const float scaleValue, const int dim);
void Vector_Scale(Vector* vec, const float scaleValue);

///
//Gets the scalar product of a specified vector and a scalar
//
//Parameters:
//	dest: The address to hold the scaled vector product
//	src: the initial vector to be scaled
//	scaleValue: the amount by which to scale the initial vector
//	dim: The number of components in the initial vector
void Vector_GetScalarProductFromArray(float* dest, const float* src, const float scaleValue, const int dim);
void Vector_GetScalarProduct(Vector* dest, const Vector* src, const float scaleValue);

///
//Adds together two vectors retrieving the sum
//
//Parameters:
//	dest: the destination of the sum vector
//	vec1: the first addend vector
//	vec2: the second addend vector
//	dim: the dimension of the vectors being added
void Vector_AddArray(float* dest, const float* vec1, const float* vec2, const int dim);
void Vector_Add(Vector* dest, const Vector* vec1, const Vector* vec2);
void Vector_SubtractArray(float* dest, const float* vec1, const float* vec2, const int dim);
void Vector_Subtract(Vector* dest, const Vector* vec1, const Vector* vec2);

///
//Prints out the contents of a vector
//
//Parameters:
//	vec: the vector to print
//	dim: the number of components in the vector
void Vector_PrintTransposeArray(const float* vec, const int dim);
void Vector_PrintTranspose(const Vector* vec);
void Vector_PrintArray(const float* vec, const int dim);
void Vector_Print(const Vector* vec);
