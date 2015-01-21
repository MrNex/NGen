#include <stdlib.h>
#include <stdio.h>

#include "Matrix.h"

///
//Allocates memory for a new matrix
//
//Parameters:
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
//
//Returns:
//	Pointer to new matrix
Matrix* Matrix_Allocate(const int numRows, const int numCols)
{
	Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
	mat->numRows = numRows;
	mat->numColumns = numCols;

	return mat;
}

///
//Initializes a matrices components and sets as identity matrix
//
//Parameters:
//	mat: Matrix to initialize
void Matrix_Initialize(Matrix* mat)
{
	mat->components = (float*)calloc(sizeof(float), mat->numRows * mat->numColumns);
	if(mat->numRows == mat->numColumns)
		Matrix_ToIdentity(mat);
}

///
//Frees a matrix's resources
//
//Parameters:
//	mat: Matrix to free
void Matrix_Free(Matrix* mat)
{
	free(mat->components);
	free(mat);
}


///
//Copies a matrix array
//
//Parameters:
//	dest: The destination of the copy
//	source: The matrix to copy
//	numRows: The number of rows in the matrix being copied
//	numCols: The number of columns in the matrix being copied
void Matrix_CopyArray(float* dest, const float* source, const int numRows, const int numCols)
{
	memcpy(dest, source, sizeof(float) * numRows * numCols);
}
//Checks for errors then calls matrix_CopyArray
void Matrix_Copy(Matrix* dest, const Matrix* src)
{
	if(dest->numRows != src->numRows || dest->numColumns != src->numColumns)
	{
		printf("Matrix_Copy failed! Source and destination of unequal dimensions. Matrix not copied.");
	}
	else
	{
		Matrix_CopyArray(dest->components, src->components, src->numRows, src->numColumns);
	}
}

///
//Transforms a matrix into an nXn Identity matrix
//
//Parameters:
//	mat: The matrix to transform
//	dim: The dimension of the desired identity matrix
void Matrix_ToIdentityArray(float* mat, const int dim)
{
	for(int row = 0; row < dim; row++)
	{
		for(int column = 0; column < dim; column++)
		{
			if( row == column)
			{
				mat[(dim * row) + column] = 1;
			}
			else
			{
				mat[(dim * row) + column] = 0;
			}
		}
	}
}
//Calls Matrix_ToIdentityArray after error checking
void Matrix_ToIdentity(Matrix* mat)
{
	//Make sure matrix is nXn
	if(mat->numRows == mat->numColumns)
	{
		Matrix_ToIdentityArray(mat->components, mat->numRows);
	}
	else
	{
		printf("Matrix_ToIdentity failed! Matrix is not NxN. Matrix not transformed to identity\n");
	}
}

///
//Indexes an array representing amatrix
//
//Parameters:
//	mat: The matrix to index
//	row: the row of the element we wish to index
//	col: The column of the element we wish to index
//	numCols: The number of columns in the matrix
float* Matrix_IndexArray(float* mat, const int row, const int col, const int numCols)
{
	return mat + (row * numCols) + col;
}
//Const correct indexing
float Matrix_GetIndexArray(const float* mat, const int row, const int col, const int numCols)
{
	return mat[(row * numCols) + col];
}
//Performs error checking then calls Matrix_IndexArray and returns null on failure
float* Matrix_Index(Matrix* mat, const int row, const int col)
{
	//Make sure the desired index is valid
	if(row < 0 || col < 0)
	{
		printf("Matrix_Index failed! Index is not valid. Index not founnd. Returning null pointer.\n");
		return 0x0;
	}
	else if(row >= mat->numRows && col >= mat->numColumns)
	{
		printf("Matrix_Index failed! Index is not valid. Index not found, returning null pointer.\n");
		return 0x0;
	}	
	else
	{
		return Matrix_IndexArray(mat->components, row, col, mat->numColumns);		
	}
}
//Performs error checking then calls Matrix_GetIndexArray, returns null on failure
float Matrix_GetIndex(const Matrix* mat, const int row, const int col)
{
		//Make sure the desired index is valid
	if(row < 0 || col < 0)
	{
		printf("Matrix_Index failed! Index is not valid. Index not founnd. Returning null pointer.\n");
		return 0x0;
	}
	else if(row >= mat->numRows && col >= mat->numColumns)
	{
		printf("Matrix_Index failed! Index is not valid. Index not found, returning null pointer.\n");
		return 0x0;
	}	
	else
	{
		return Matrix_GetIndexArray(mat->components, row, col, mat->numColumns);		
	}
}


///
//Gets an array representing a row vector of this matrix
//
//Parameters:
//	destination: The destination of the row vector array
//	mat: The matrix to get a row from
//	desiredRow: The row to get
//	numColumns: The number of columns in the source matrix
void Matrix_GetRowVectorArray(float* destination, const float* mat, const int desiredRow, const int numColumns)
{
	for(int i = 0; i < numColumns; i++)
	{
		destination[i] = Matrix_GetIndexArray(mat, desiredRow, i, numColumns);
	}
}
//Checks for errors then calls Matrix_GetRowVectorArray
void Matrix_GetRowVector(Vector* destination, const Matrix* mat, const int desiredRow)
{
	if(destination->dimension == mat->numColumns)
	{
		if(desiredRow >= 0 && desiredRow < mat->numRows)
		{
			Matrix_GetRowVectorArray(destination->components, mat->components, desiredRow, mat->numColumns);
		}
		else
		{
			printf("Matrix_GetRowVector failed! Invalid desired row! Row vector not retrieved.\n");
		}
	}
	else
	{
		printf("Matrix_GetRowVector failed! Matrix and Vector not of compatible dimension. Row vector not retrieved.\n");
	}
}

///
//Multiplies a matrix onto another, transforming the latter.
//
//Parameters:
//	LHSMatrix: Left hand side matrix (Will not be altered)
//	RHSMatrix: Right hand side matrix (Destination of product)
//	LHSNumRows: Number of rows in the left hand side matrix 
//	LHSNumCols: Number of columns in the left hand side matrix (Must be equal to the number of rows in the right hand side matrix)
void Matrix_TransformMatrixArray(const float* LHSMatrix, float* RHSMatrix, const int LHSNumRows, const int LHSNumCols, const int RHSNumCols)
{
	//Create a copy of the right hand side matrix
	//float* RHSCopy = (float*)malloc(sizeof(float) * LHSNumCols * RHSNumCols);
	float RHSCopy[LHSNumCols * RHSNumCols];
	
	//memcpy(RHSCopy, RHSMatrix, sizeof(float) * LHSNumRows * LHSNumCols);
	Matrix_CopyArray(RHSCopy, RHSMatrix, LHSNumCols, RHSNumCols);

	//Get product of LHSMatrix and RHSCopy and store in RHSMatrix
	Matrix_GetProductMatrixArray(RHSMatrix, LHSMatrix, RHSCopy, LHSNumRows, LHSNumCols, RHSNumCols);
}
//Checks for errors then calls Matrix_TransformMatrixArray
void Matrix_TransformMatrix(const Matrix* LHSMatrix, Matrix* RHSMatrix)
{
	//Make sure the dimensions of LHS and RHS Transpose match

		if(LHSMatrix->numColumns == RHSMatrix->numRows)
		{
			Matrix_TransformMatrixArray(LHSMatrix->components, RHSMatrix->components, LHSMatrix->numRows, LHSMatrix->numColumns, RHSMatrix->numColumns);
			return;
		}
	
	//If code reaches this point, method failed.

	printf("Matrix_TransformMatrix failed! Dimensions of operands are not compatible. Matrix not transformed.\n");
}

///
//Gets the product of a matrix acting upon another matrix
//
//Parameters:
//	destMatrix: The destination of the product matrix
//	LHSMatrix: The left hand side matrix
//	RHSMatrix: The right hand side matrix
//	LHSNumRows: The number of rows in the left hand side matrix
//	LHSNumCols: The number of columns in the left hand side matrix
void Matrix_GetProductMatrixArray(float* destMatrix, const float* LHSMatrix, const float* RHSMatrix, const int LHSNumRows, const int LHSNumCols, const int RHSNumCols)
{
	for(int rows = 0; rows < LHSNumRows; rows++)
	{
		for(int cols = 0; cols < RHSNumCols; cols++)
		{
			*Matrix_IndexArray(destMatrix, rows, cols, RHSNumCols) = 0.0f;
			for(int dot = 0; dot < LHSNumCols; dot++)
			{	
				 *Matrix_IndexArray(destMatrix, rows, cols, RHSNumCols) += Matrix_GetIndexArray(LHSMatrix, rows, dot, LHSNumCols) * Matrix_GetIndexArray(RHSMatrix, dot, cols, RHSNumCols);
			}
		}
	}
}
//Checks for errors then calls GetProductMatrixArray
void Matrix_GetProductMatrix(Matrix* destMatrix, const Matrix* LHSMatrix, const Matrix* RHSMatrix)
{
	//Ensure the LHSMatrix's number of columns is equivilent to the RHSMatrix's number of rows
	if(LHSMatrix->numColumns != RHSMatrix->numRows)
	{
		printf("Matrix_GetProductMatrix Failed! LHSMatrix and RHSMatrix are not of compatible dimensions! Product not retrieved.");
	}
	else if(destMatrix->numRows != LHSMatrix->numRows || destMatrix->numColumns != RHSMatrix->numColumns)
	{
		printf("Matrix_GetPRoductMatrix Failed! destMatrix is not of correct size for product. Product not retrieved.");
	}
	else
	{
		Matrix_GetProductMatrixArray(destMatrix->components, LHSMatrix->components, RHSMatrix->components, LHSMatrix->numRows, LHSMatrix->numColumns, RHSMatrix->numColumns);
	}
}

///
//Multiplies a matrix onto a vecor, transforming the vector
//
//Parameters:
//	LHSMatrix: The left hand side operand, the matrix
//	RHSVector: The right had side operand and the destination, the vector
//	LHSNumRows: The number of rows in the Left Hand Side matrix
//	LHSNumCols: The number of rows in the Right Hand Side matrix
void Matrix_TransformVectorArray(const float* LHSMatrix, float* RHSVector, const int LHSNumRows, const int LHSNumCols)
{
	float RHSCopy[LHSNumCols];
	Vector_CopyArray(RHSCopy, RHSVector, LHSNumCols);

	Matrix_GetProductVectorArray(RHSVector, LHSMatrix, RHSCopy, LHSNumRows, LHSNumCols);
}
//Checks for errors then calls Matrix_TransformVectorArray
void Matrix_TransformVector(const Matrix* LHSMatrix, Vector* RHSVector)
{
	if(LHSMatrix->numColumns != RHSVector->dimension)
	{
		printf("Matrix_TransformVector failed! Operands are of incompatible sizes. Vector not transformed.\n");
	}
	else if(LHSMatrix->numRows != LHSMatrix->numColumns)
	{
		printf("Matrix_TransformVector failed! Matrix must be square to tranform vetor. Vector not transformed.\n");
	}
	else
	{
		Matrix_TransformVectorArray(LHSMatrix->components, RHSVector->components, LHSMatrix->numRows, LHSMatrix->numColumns);
	}
}

///
//Gets the product of a matrix acting upon a vector
//
//Parameters:
//	destVector: The destination of the product vector
//	LHSMatrix: The left hand side matrix operand
//	RHSVector: The right hand side Vector operand
//	LHSNumRows: The number of rows in the LHS Matrix operand
//	LHSNumCols: The number of columns in the LHS MatrixOperand
void Matrix_GetProductVectorArray(float* destVector, const float* LHSMatrix, const float* RHSVector, const int LHSNumRows, const int LHSNumCols)
{
	for(int row = 0; row < LHSNumRows; row++)
	{
		destVector[row] = 0;
		for(int col = 0; col < LHSNumCols; col++)
		{
			destVector[row] += Matrix_GetIndexArray(LHSMatrix, row, col, LHSNumCols) * RHSVector[col];
		}	
	}
}
//Checks for errors then calls Matrix_GetPRoductVectorArray
void Matrix_GetProductVector(Vector* destVector, const Matrix* LHSMatrix, const Vector* RHSVector)
{
	if(destVector->dimension != LHSMatrix->numRows)
	{
		printf("Matrix_GetProductVector failed! Destination is not the proper size. Product vector not retrieved\n");
	}
	else if(LHSMatrix->numColumns != RHSVector->dimension)
	{
		printf("Matrix_GetProductVector failed! Operands are of incompatible size. Product vector not retrieved\n");
	}
	else
	{
		Matrix_GetProductVectorArray(destVector->components, LHSMatrix->components, RHSVector->components, LHSMatrix->numRows, LHSMatrix->numColumns);
	}
}

///
//Prints out a matrix
//
//Parameters:
//	mat: The Matrix to print
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
void Matrix_PrintArray(const float* mat, const int numRows, const int numCols)
{
	printf("[ ");
	for(int row = 0; row < numRows; row++)
	{
		for(int col = 0; col < numCols; col++)
		{
			printf("%f ", Matrix_GetIndexArray(mat, row, col, numCols));
		}
		if(row < numRows - 1)
			printf("\n  ");
		else
			printf("]\n");
	}
}
//Calls Matrix_PrintArray
void Matrix_Print(const Matrix* mat)
{
	Matrix_PrintArray(mat->components, mat->numRows, mat->numColumns);
}
