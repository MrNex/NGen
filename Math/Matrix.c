#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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
Matrix* Matrix_Allocate()
{
	Matrix* mat = (Matrix*)malloc(sizeof(Matrix));
	return mat;
}

///
//Initializes a matrices components and sets as identity matrix
//
//Parameters:
//	mat: Matrix to initialize
void Matrix_Initialize(Matrix* mat, const int numRows, const int numCols)
{
	mat->numRows = numRows;
	mat->numColumns = numCols;
	mat->components = (float*)calloc(sizeof(float), mat->numRows * mat->numColumns);
	if (mat->numRows == mat->numColumns)
	{
		Matrix_ToIdentity(mat);
	}
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
	memcpy(dest, source, sizeof(float)* numRows * numCols);
}
//Checks for errors then calls matrix_CopyArray
void Matrix_Copy(Matrix* dest, const Matrix* src)
{
	if (dest->numRows != src->numRows || dest->numColumns != src->numColumns)
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
	for (int row = 0; row < dim; row++)
	{
		for (int column = 0; column < dim; column++)
		{
			if (row == column)
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
	if (mat->numRows == mat->numColumns)
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
	if (row < 0 || col < 0)
	{
		printf("Matrix_Index failed! Index is not valid. Index not founnd. Returning null pointer.\n");
		return 0x0;
	}
	else if (row >= mat->numRows && col >= mat->numColumns)
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
	if (row < 0 || col < 0)
	{
		printf("Matrix_Index failed! Index is not valid. Index not founnd. Returning null pointer.\n");
		return 0x0;
	}
	else if (row >= mat->numRows && col >= mat->numColumns)
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
//Gets the minor matrix corresponding to the specified index
//
//Parameters:
//	dest: The destination matrix
//	mat: The matrix to extract the minor from
//	row: The row of the index to get the corresponding minor of
//	col: The column of the index to get the corresponding minor of
//	numRows: The number of rows in the matrix we are extracting the minor from
//	numColumns: The number of columns in the matrix we are extracting the minor from
void Matrix_GetMinorArray(float* dest, const float* mat, const int row, const int col, const unsigned int numRows, const unsigned int numColumns)
{
	unsigned int destRow = 0, destColumn = 0;
	for(unsigned int srcRow = 0; srcRow < numRows; srcRow++)
	{
		if(srcRow == row) continue;
		for(unsigned int srcColumn = 0; srcColumn < numColumns; srcColumn++)
		{
			if(srcColumn == col)continue;
			*Matrix_IndexArray(dest, destRow, destColumn, numColumns - 1) = Matrix_GetIndexArray(mat, srcRow, srcColumn, numColumns);
			destColumn++;
		}
		destColumn = 0;
		destRow++;
	}
}
//Checks for errors then calls CMatrix_GetMinorArray
void Matrix_GetMinor(Matrix* dest, const Matrix* mat, const int row, const int col)
{
	if(dest->numRows != mat->numRows - 1 || dest->numColumns != mat->numColumns - 1)
	{
		printf("CMatrix_GetMinor failed! Destination matrix is not of proper dimensions. Minor not retrieved.\n");
		return;
	}
	else if(mat->numRows <= 2 || mat->numColumns <= 2)
	{
		printf("CMatrix_GetMinor failed! Matrix is not of proper dimensions to have a minor matrix extracted. Minor not retrieved.\n");
	}
	else
	{
		Matrix_GetMinorArray(dest->components, mat->components, row, col, mat->numRows, mat->numColumns);
	}
}

///
//Gets an array representing a row Vector of this matrix
//
//Parameters:
//	destination: The destination of the row Vector array
//	mat: The matrix to get a row from
//	desiredRow: The row to get
//	numColumns: The number of columns in the source matrix
void Matrix_GetRowVectorArray(float* destination, const float* mat, const int desiredRow, const int numColumns)
{
	for (int i = 0; i < numColumns; i++)
	{
		destination[i] = Matrix_GetIndexArray(mat, desiredRow, i, numColumns);
	}
}

//Checks for errors then calls Matrix_GetRowVectorArray
void Matrix_GetRowVector(Vector* destination, const Matrix* mat, const int desiredRow)
{
	if (destination->dimension == mat->numColumns)
	{
		if (desiredRow >= 0 && desiredRow < mat->numRows)
		{
			Matrix_GetRowVectorArray(destination->components, mat->components, desiredRow, mat->numColumns);
		}
		else
		{
			printf("Matrix_GetRowVector failed! Invalid desired row! Row Vector not retrieved.\n");
		}
	}
	else
	{
		printf("Matrix_GetRowVector failed! Matrix and Vector not of compatible dimension. Row Vector not retrieved.\n");
	}
}

///
//Gets an array representing a column of this Vector
//
//Parameters:
//	destination: The destination of the column Vector array
//	mat: The matrix to get a column from
//	desiredCol: The column to get from the matrix
//	numRows: The number of rows in the matrix
//	numColumns: The number of columns in the matrix
void Matrix_GetColumnVectorArray(float* destination, const float* mat, const int desiredCol, const int numRows, const int numColumns)
{
	for(int i = 0; i < numRows; i++)
	{
		destination[i] = Matrix_GetIndexArray(mat, i, desiredCol, numColumns);
	}
}
//Checks for errors then calls Matrix_GetColumnVectorArray
void Matrix_GetColumnVector(Vector* destination, const Matrix* mat, const int desiredCol)
{
	if (destination->dimension == mat->numRows)
	{
		if (desiredCol >= 0 && desiredCol < mat->numColumns)
		{
			Matrix_GetColumnVectorArray(destination->components, mat->components, desiredCol, mat->numRows, mat->numColumns);
		}
		else
		{
			printf("Matrix_GetColumnVector failed! Invalid desired Column! Column Vector not retrieved.\n");
		}
	}
	else
	{
		printf("Matrix_GetRowVector failed! Matrix and Vector not of compatible dimension. Column Vector not retrieved.\n");
	}
}

///
//Slices a row of a matrix storing the contents in a Vector
//
//Parameters:
//	destination: The destination array to hold the row contents
//	mat: The matrix to slice
//	desiredRow: The row of the matrix to slice
//	sliceStart: The index to begin slicing the row (inclusive)
//	sliceRange: The amount of indices to slice
//	numColumns:	The number of columns in the matrix
void Matrix_SliceRowArray(float* destination, const float* mat, const int desiredRow, const int sliceStart, const int sliceRange, const int numColumns)
{
	//The ugliest line of code written by me so far this year.
	memcpy(destination, Matrix_IndexArray((float*)mat, desiredRow, sliceStart, numColumns), sliceRange * sizeof(float));
}
//Checks for errors then calls Matrix_SliceRowArray
void Matrix_SliceRow(Vector* destination, const Matrix* mat, const int desiredRow, const int sliceStart, const int sliceRange)
{
	if(desiredRow >= 0 && desiredRow < mat->numRows)
	{
		if(sliceStart + sliceRange <= mat->numColumns)
		{
			if(destination->dimension >= sliceRange)
			{
				Matrix_SliceRowArray(destination->components, mat->components, desiredRow, sliceStart, sliceRange, mat->numColumns);
			}
			else
			{
				printf("Matrix_SliceRow Failed! Slice is outside bounds of destination Vector. Slice not retrieved.\n");
			}
		}
		else
		{
			printf("Matrix_SliceRow Failed! Slice is outside bounds of matrix. Slice not retrieved.\n");
		}
	}
	else
	{
		printf("Matrix_SliceRow failed! Row is outside bounds of matrix. Slice not retrieved.\n");
	}
}

///
//Slices a row of a matrix storing the contents in a Vector
//
//Parameters:
//	destination: The destination array to hold the row contents
//	mat: The matrix to slice
//	desiredColumn: The row of the matrix to slice
//	sliceStart: The index to begin slicing the row (inclusive)
//	sliceRange: The amount of indices to slice
//	numColumns:	The number of columns in the matrix
void Matrix_SliceColumnArray(float* destination, const float* mat, const int desiredColumn, const int sliceStart, const int sliceRange, const int numColumns)
{
	for(int i = 0; i < sliceRange; i++)
		destination[i] = Matrix_GetIndexArray(mat, sliceStart + i, desiredColumn, numColumns);
}
//Checks for errors then calls Matrix_SliceColumnArray
void Matrix_SliceColumn(Vector* destination, const Matrix* mat, const int desiredColumn, const int sliceStart, const int sliceRange)
{
	if(desiredColumn > 0 && desiredColumn < mat->numColumns)
	{
		if(sliceStart + sliceRange < mat->numRows)
		{
			if(destination->dimension >= sliceRange)
			{
				Matrix_SliceRowArray(destination->components, mat->components, desiredColumn, sliceStart, sliceRange, mat->numColumns);
			}
			else
			{
				printf("Matrix_SliceColumn Failed! Slice is outside bounds of destination Vector. Slice not retrieved.\n");
			}
		}
		else
		{
			printf("Matrix_SliceColumn Failed! Slice is outside bounds of matrix. Slice not retrieved.\n");
		}
	}
	else
	{
		printf("Matrix_SliceColumn failed! Column is outside bounds of matrix. Slice not retrieved.\n");
	}
}

///
//Scales a matrix by a scalar
//
//Parameters:
//	matrix: A pointer to the array of floats representing the matrix to scale
//	numRows: The number of rows in the matrix
//	numCols: The number of columns in the matrix
//	scalarValue: The value by which to scale the matrix
void Matrix_ScaleArray(float* matrix, const unsigned int numRows, const unsigned int numColumns, const float scalarValue)
{
	for(int i = 0; i < numRows * numColumns; i++)
	{
		matrix[i] *= scalarValue;
	}
}
//Calls Matrix_ScaleArray
void Matrix_Scale(Matrix* matrix, const float scalarValue)
{
	Matrix_ScaleArray(matrix->components, matrix->numRows, matrix->numColumns, scalarValue);
}

///
//Calculates the determinate of a matrix in array form.
//
//Parameters:
//	mat: The matrix to calculate the determinate of
//	numColumns: The number of columns in the matrix
//	numRows: The number of rows in the matrix
float Matrix_GetDeterminateArray(const float* mat, const int numRows, const int numColumns)
{
	float determinate = 0.0f;
	if(numRows > 2 && numColumns > 2)
	{
		for(int i = 0; i < numColumns; i++)
		{
			float coefficient = powf(-1.0, (float)(i + 2)) * Matrix_GetIndexArray(mat, 0, i, numColumns);
			float minorDeterminate = 0.0f;

			float* minor = (float*)malloc(sizeof(float) * (numRows - 1) * (numColumns - 1));
			Matrix_GetMinorArray(minor, mat, 0, i, numRows, numColumns);
			minorDeterminate = Matrix_GetDeterminateArray(minor, numRows - 1, numColumns - 1);
			free(minor);



			determinate += coefficient * minorDeterminate;
		}
	}
	else
	{
		determinate = (Matrix_GetIndexArray(mat, 0, 0, numColumns) * Matrix_GetIndexArray(mat, 1, 1, numColumns)) - (Matrix_GetIndexArray(mat, 0, 1, numColumns) * Matrix_GetIndexArray(mat, 1, 0, numColumns));
	}

	return determinate;
}
//Checks for errors then calls CMatrix_GetDeterminateArray
float Matrix_GetDeterminate(const Matrix* mat)
{
	//Make sure matrix is nxn, else return 0
	if(mat->numRows != mat->numColumns)
	{
		return 0;
	}
	else
	{
		return Matrix_GetDeterminateArray(mat->components, mat->numRows, mat->numColumns);
	}
}

///
//Trasposes a matrix in array form
//Matrix must be NxN
//
//Parameters:
//	mat: The matrix to transpose in array form
//	numRows: The number of rows in the matrix
//	numColumns: the number of columns in the matrix
void Matrix_TransposeArray(float* mat, const int numRows, const int numColumns)
{
	int middleRow = numRows % 2 == 0 ? numRows / 2 : numRows / 2 + 1;
	for(int i = 0; i < middleRow; i++)
	{
		for(int j = i + 1; j < numColumns; j++)
		{
			float temp = *Matrix_IndexArray(mat, i, j, numColumns);
			*Matrix_IndexArray(mat, i, j, numColumns) = Matrix_GetIndexArray(mat, j, i, numColumns);
			*Matrix_IndexArray(mat, j, i, numColumns) = temp;
		}
	}
}
//Checks for errors then calls Matrix_TransposeArray
void Matrix_Transpose(Matrix* mat)
{
	if(mat->numRows != mat->numColumns)
	{
		printf("Matrix_Transpose Failed! Matrix is not NxN! Matrix was not transposed! Consider using Matrix_GetTranspose!\n");
		return;
	}
	Matrix_TransposeArray(mat->components, mat->numRows, mat->numColumns);
}


///
//Finds the transpose of a matrix array and stores it in a given array
//
//Parameters:
//	dest: A pointer to an array of floats as the destinaton of the transpose matrix
//	matrix: A pointer to an array of floats representing the matrix to transpose
//	numRows: the number of rows in the matrix
//	numColumns: The number of columns in the matrix
void Matrix_GetTransposeArray(float* dest,const float* matrix, const int numRows, const int numColumns)
{
	for(int i = 0; i < numRows; i++)
	{
		for(int j = 0; j < numColumns; j++)
		{
			*Matrix_IndexArray(dest, j, i, numRows) = Matrix_GetIndexArray(matrix, i, j, numColumns);
		}
	}
}
//Checks for errors, then calls Matrix_GetTransposeArray
void Matrix_GetTranspose(Matrix* dest, Matrix* src)
{
	if(dest->numRows != src->numColumns || dest->numColumns != src->numRows)
	{
		printf("Matrix_GetTranspose failed! Destiantion matrix is ot of proper dimensions! Transpose not found!\n");
		return;
	}

	Matrix_GetTransposeArray(dest->components, src->components, src->numRows, src->numColumns);
}

///
//Calculates the inverse of a matrix in array form.
//
//Parameters:
//	dest: A pointer to an array of floats to store the inverse of the components
//	matrix: A pointer to an array of floats containing the components of the matrix to invert
//	numRows: The number of rows in the matrix being inverted
//	numCols: The number of columns in the matrix being inverted
void Matrix_GetInverseArray(float* dest, const float* matrix, const unsigned int numRows, const unsigned int numCols)
{
	if(numRows > 2 && numCols > 2)
	{
		//Create an array of floats to hold the minor matrices
		float* minor = (float*)malloc(sizeof(float) * (numRows - 1) * (numCols - 1));

		//Set the dimensions of the current minor
		//No minor was taken yet, dimensions match the dimensions of the matrix
		unsigned int currentNumRows = numRows;
		unsigned int currentNumColumns = numCols;

		for(int i = 0; i < numRows; i++)
		{
			for(int j = 0; j < numCols; j++)
			{

				//Get the minor of the 0, 0 index
				Matrix_GetMinorArray(minor, matrix, i, j, numRows, numCols);

				//Find the determinate of the minor
				float minorDet = Matrix_GetDeterminateArray(minor, numRows - 1, numCols - 1);

				//The j,i th component of the adjoint matrix is (-1)^(i*j) * the i,jth minor
				*Matrix_IndexArray(dest, j, i, numCols) = pow(-1.0f, i+j) * minorDet;

			}
		}

		//Free the memory made for the minor matrix
		free(minor);
	}
	else
	{
		//Swap the components on the diagonal
		*Matrix_IndexArray(dest, 0, 0, numCols) = Matrix_GetIndexArray(matrix, 1, 1, numCols);
		*Matrix_IndexArray(dest, 1, 1, numCols) = Matrix_GetIndexArray(matrix, 0, 0, numCols);

		//Negate the off diagonals
		*Matrix_IndexArray(dest, 0, 1, numCols) = -1.0f * Matrix_GetIndexArray(matrix, 0, 1, numCols);
		*Matrix_IndexArray(dest, 1, 0, numCols) = -1.0f * Matrix_GetIndexArray(matrix, 1, 0, numCols);
	}

	//Scale the adjoint by 1/det(matrix)
	Matrix_ScaleArray(dest, numRows, numCols, 1.0f/Matrix_GetDeterminateArray(matrix, numRows, numCols));

}
//Checks for errors, then calls Matrix_GetInverseArray
void Matrix_GetInverse(Matrix* dest, const Matrix* matrix)
{
	if(dest->numRows != matrix->numRows || dest->numColumns != matrix->numColumns)
	{
		printf("Matrix_GetInverse failed! Dimensions of destination and input matrices do not match! Inverse not found!\n");
		return;
	}
	else if(Matrix_GetDeterminate(matrix) == 0)
	{
		printf("Matrix_GetInverse failed! Matrix is not invertible! Inverse not found!\n");
		return;
	}
	else
	{
		Matrix_GetInverseArray(dest->components, matrix->components, matrix->numRows, matrix->numColumns);
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
	float* RHSCopy = (float*)malloc(sizeof(float) * LHSNumCols * RHSNumCols);
	//float RHSCopy[LHSNumCols * RHSNumCols];

	Matrix_CopyArray(RHSCopy, RHSMatrix, LHSNumCols, RHSNumCols);

	//Get product of LHSMatrix and RHSCopy and store in RHSMatrix
	Matrix_GetProductMatrixArray(RHSMatrix, LHSMatrix, RHSCopy, LHSNumRows, LHSNumCols, RHSNumCols);

	//Never forget
	free(RHSCopy);
}
//Checks for errors then calls Matrix_TransformMatrixArray
void Matrix_TransformMatrix(const Matrix* LHSMatrix, Matrix* RHSMatrix)
{
	//Make sure the dimensions of LHS and RHS Transpose match

	if (LHSMatrix->numColumns == RHSMatrix->numRows)
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
	for (int rows = 0; rows < LHSNumRows; rows++)
	{
		for (int cols = 0; cols < RHSNumCols; cols++)
		{
			*Matrix_IndexArray(destMatrix, rows, cols, RHSNumCols) = 0.0f;
			for (int dot = 0; dot < LHSNumCols; dot++)
			{
				float increment = Matrix_GetIndexArray(LHSMatrix, rows, dot, LHSNumCols) * Matrix_GetIndexArray(RHSMatrix, dot, cols, RHSNumCols);
				*Matrix_IndexArray(destMatrix, rows, cols, RHSNumCols) += increment;
			}
		}
	}
}
//Checks for errors then calls GetProductMatrixArray
void Matrix_GetProductMatrix(Matrix* destMatrix, const Matrix* LHSMatrix, const Matrix* RHSMatrix)
{
	//Ensure the LHSMatrix's number of columns is equivilent to the RHSMatrix's number of rows
	if (LHSMatrix->numColumns != RHSMatrix->numRows)
	{
		printf("Matrix_GetProductMatrix Failed! LHSMatrix and RHSMatrix are not of compatible dimensions! Product not retrieved.");
	}
	else if (destMatrix->numRows != LHSMatrix->numRows || destMatrix->numColumns != RHSMatrix->numColumns)
	{
		printf("Matrix_GetPRoductMatrix Failed! destMatrix is not of correct size for product. Product not retrieved.");
	}
	else
	{
		Matrix_GetProductMatrixArray(destMatrix->components, LHSMatrix->components, RHSMatrix->components, LHSMatrix->numRows, LHSMatrix->numColumns, RHSMatrix->numColumns);
	}
}

///
//Multiplies a matrix onto a vecor, transforming the Vector
//
//Parameters:
//	LHSMatrix: The left hand side operand, the matrix
//	RHSVector: The right had side operand and the destination, the Vector
//	LHSNumRows: The number of rows in the Left Hand Side matrix
//	LHSNumCols: The number of rows in the Right Hand Side matrix
void Matrix_TransformVectorArray(const float* LHSMatrix, float* RHSVector, const int LHSNumRows, const int LHSNumCols)
{
	//float RHSCopy[LHSNumCols];
	float* RHSCopy = (float*)malloc(sizeof(float)* LHSNumCols);

	Vector_CopyArray(RHSCopy, RHSVector, LHSNumCols);

	Matrix_GetProductVectorArray(RHSVector, LHSMatrix, RHSCopy, LHSNumRows, LHSNumCols);

	free(RHSCopy);
}
//Checks for errors then calls Matrix_TransformVectorArray
void Matrix_TransformVector(const Matrix* LHSMatrix, Vector* RHSVector)
{
	if (LHSMatrix->numColumns != RHSVector->dimension)
	{
		printf("Matrix_TransformVector failed! Operands are of incompatible sizes. Vector not transformed.\n");
	}
	else if (LHSMatrix->numRows != LHSMatrix->numColumns)
	{
		printf("Matrix_TransformVector failed! Matrix must be square to tranform vetor. Vector not transformed.\n");
	}
	else
	{
		Matrix_TransformVectorArray(LHSMatrix->components, RHSVector->components, LHSMatrix->numRows, LHSMatrix->numColumns);
	}
}

///
//Gets the product of a matrix acting upon a Vector
//
//Parameters:
//	destVector: The destination of the product Vector
//	LHSMatrix: The left hand side matrix operand
//	RHSVector: The right hand side Vector operand
//	LHSNumRows: The number of rows in the LHS Matrix operand
//	LHSNumCols: The number of columns in the LHS MatrixOperand
void Matrix_GetProductVectorArray(float* destVector, const float* LHSMatrix, const float* RHSVector, const int LHSNumRows, const int LHSNumCols)
{
	for (int row = 0; row < LHSNumRows; row++)
	{
		destVector[row] = 0;
		for (int col = 0; col < LHSNumCols; col++)
		{
			destVector[row] += Matrix_GetIndexArray(LHSMatrix, row, col, LHSNumCols) * RHSVector[col];
		}
	}
}
//Checks for errors then calls Matrix_GetPRoductVectorArray
void Matrix_GetProductVector(Vector* destVector, const Matrix* LHSMatrix, const Vector* RHSVector)
{
	if (destVector->dimension != LHSMatrix->numRows)
	{
		printf("Matrix_GetProductVector failed! Destination is not the proper size. Product Vector not retrieved\n");
	}
	else if (LHSMatrix->numColumns != RHSVector->dimension)
	{
		printf("Matrix_GetProductVector failed! Operands are of incompatible size. Product Vector not retrieved\n");
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
	for (int row = 0; row < numRows; row++)
	{
		for (int col = 0; col < numCols; col++)
		{
			printf("%f ", Matrix_GetIndexArray(mat, row, col, numCols));
		}
		if (row < numRows - 1)
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

