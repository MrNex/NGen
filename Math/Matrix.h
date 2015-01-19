#include "Vector.h"

typedef struct Matrix
{
	int numRows;
	int numColumns;

	float* components;
}Matrix;

///
//Allocates memory for a new matrix
//
//Parameters:
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
//
//Returns:
//	Pointer to new matrix
Matrix* Matrix_Allocate(const int numRows, const int numCols);

///
//Initializes a matrices components
//
//Parameters:
//	mat: Matrix to initialize
void Matrix_Initialize(Matrix* mat);

///
//Frees a matrix's resources
//
//Parameters:
//	mat: Matrix to free
void Matrix_Free(Matrix* mat);

///
//Copies a matrix array
//
//Parameters:
//	dest: The destination of the copy
//	source: The matrix to copy
//	numRows: The number of rows in the matrix being copied
//	numCols: The number of columns in the matrix being copied
void Matrix_CopyArray(float* dest, const float* source, const int numRows, const int numCols);
//Checks for errors then calls matrix_CopyArray
void Matrix_Copy(Matrix* dest, const Matrix* src);

///
//Transforms a matrix into an nXn Identity matrix
//
//Parameters:
//	mat: The matrix to transform
//	dim: The dimension of the desired identity matrix
void Matrix_ToIdentityArray(float* mat, const int dim);
//Calls Matrix_ToIdentityArray after error checking
void Matrix_ToIdentity(Matrix* mat);

///
//Indexes an array representing amatrix
//
//Parameters:
//	mat: The matrix to index
//	row: the row of the element we wish to index
//	col: The column of the element we wish to index
//	numRows: The numberof rows in the matrix
//	numCols: The number of columns in the matrix
float* Matrix_IndexArray(float* mat, const int row, const int col, const int numCols);
//Const correct indexing
float Matrix_GetIndexArray(const float* mat, const int row, const int col, const int numCols);
//Performs error checking then calls Matrix_IndexArray
float* Matrix_Index(Matrix* mat, const int row, const int col);
//Const correct error checking indexing! Wahooo!
float Matrix_GetIndex(const Matrix* mat, const int row, const int col);

///
//Gets an array representing a row vector of this matrix
//
//Parameters:
//	destination: The destination of the row vector array
//	mat: The matrix to get a row from
//	desiredRow: The row to get
//	numColumns: The number of columns in the source matrix
void Matrix_GetRowVectorArray(float* destination, const float* mat, const int desiredRow, const int numColumns);
//Checks for errors then calls Matrix_GetRowVectorArray
void Matrix_GetRowVector(Vector* destination, const Matrix* mat, const int desiredRow);

///
//Multiplies a matrix onto another, transforming the latter.
//
//Parameters:
//	LHSMatrix: Left hand side matrix (Will not be altered)
//	RHSMatrix: Right hand side matrix (Destination of product)
//	LHSNumRows: Number of rows in the left hand side matrix (Must be equal to the number of columns in the right hand side matrix)
//	LHSNumCols: Number of columns in the left hand side matrix (Must be equal to the number of rows in the right hand side matrix)
void Matrix_TransformMatrixArray(const float* LHSMatrix, float* RHSMatrix, const int LHSNumRows, const int LHSNumCols, const int RHSNumCols);
//Checks for errors then calls Matrix_TransformMatrixArray
void Matrix_TransformMatrix(const Matrix* LHSMatrix, Matrix* RHSMatrix);

///
//Gets the product of a matrix acting upon another matrix
//
//Parameters:
//	destMatrix: The destination of the product matrix
//	LHSMatrix: The left hand side matrix
//	RHSMatrix: The right hand side matrix
//	LHSNumRows: The number of rows in the left hand side matrix
//	LHSNumCols: The number of columns in the left hand side matrix
void Matrix_GetProductMatrixArray(float* destMatrix, const float* LHSMatrix, const float* RHSMatrix, const int LHSNumRows, const int LHSNumCols, const int RHSNumCols);
//Checks for errors then calls GetProductMatrixArray
void Matrix_GetProductMatrix(Matrix* destMatrix, const Matrix* LHSMatrix, const Matrix* RHSMatrix);

///
//Multiplies a matrix onto a vecor, transforming the vector
//
//Parameters:
//	LHSMatrix: The left hand side operand, the matrix
//	RHSVector: The right had side operand and the destination, the vector
//	LHSNumRows: The number of rows in the Left Hand Side matrix
//	LHSNumCols: The number of rows in the Right Hand Side matrix
void Matrix_TransformVectorArray(const float* LHSMatrix, float* RHSVector, const int LHSNumRows, const int LHSNumCols);
//Checks for errors then calls Matrix_TransformVectorArray
void Matrix_TransformVector(const Matrix* LHSMatrix, Vector* RHSVector);


///
//Gets the product of a matrix acting upon a vector
//
//Parameters:
//	destVector: The destination of the product vector
//	LHSMatrix: The left hand side matrix operand
//	RHSVector: The right hand side Vector operand
//	LHSNumRows: The number of rows in the LHS Matrix operand
//	LHSNumCols: The number of columns in the LHS MatrixOperand
void Matrix_GetProductVectorArray(float* destVector, const float* LHSMatrix, const float* RHSVector, const int LHSNumRows, const int LHSNumCols);
//Checks for errors then calls Matrix_GetPRoductVectorArray
void Matrix_GetProductVector(Vector* destVector, const Matrix* LHSMatrix, const Vector* RHSVector);



///
//Prints out a matrix
//
//Parameters:
//	mat: The Matrix to print
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
void Matrix_PrintArray(const float* mat, const int numRows, const int numCols);
//Calls Matrix_PrintArray
void Matrix_Print(const Matrix* mat);
