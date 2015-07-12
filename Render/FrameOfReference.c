#include "FrameOfReference.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

///
//Allocates memory for a Frame of Reference
//
//Returns:
//	Pointer to newly allocated uninitialized Frame of Reference
FrameOfReference* FrameOfReference_Allocate(void)
{
	FrameOfReference* FoRef = (FrameOfReference*)malloc(sizeof(FrameOfReference));
	return FoRef;
}

///
//Initializes a Frame of Reference
//
//Parameters:
//	FoRef: The frame of reference to initialize
void FrameOfReference_Initialize(FrameOfReference* FoRef)
{
	FoRef->scale = Matrix_Allocate();
	Matrix_Initialize(FoRef->scale, 3, 3);

	FoRef->rotation = Matrix_Allocate();
	Matrix_Initialize(FoRef->rotation, 3, 3);

	FoRef->position = Vector_Allocate();
	Vector_Initialize(FoRef->position, 3);

}

///
//Frees resources allocated for a Frame of Reference
//
//Parameters:
//	FoRef: A pointer to the frame of reference being freed
void FrameOfReference_Free(FrameOfReference* FoRef)
{
	Vector_Free(FoRef->position);
	Matrix_Free(FoRef->scale);
	Matrix_Free(FoRef->rotation);
	free(FoRef);
}

///
//Translates the origin of a Frame of Reference
//
//Parameters:
//	FoRef: The frame of reference being translated
//	translation: The 3 component translation Vector
void FrameOfReference_Translate(FrameOfReference* FoRef, const Vector* translation)
{

	Vector_Increment(FoRef->position, translation);
}

///
//Rotates a Frame of Reference
//
//Parameters:
//	FoRef: The frame of reference being rotated
//	axis: The axis being rotated around (Relative to the frame of reference) (Will be normalized).
//	radians: The amount of radians to rotate by
void FrameOfReference_Rotate(FrameOfReference* FoRef, const Vector* axis, const float radians)
{

	Vector copyOfAxis;
	Vector_INIT_ON_STACK(copyOfAxis, 3);
	Vector_Copy(&copyOfAxis, axis);

	Vector_Normalize(&copyOfAxis);

	Matrix rotMat;
	Matrix_INIT_ON_STACK(rotMat, 3, 3);

	//Construct a rotation matrix based on rodriguez' formula
	FrameOfReference_ConstructRotationMatrix(&rotMat, &copyOfAxis, radians);
	Matrix_TransformMatrix(&rotMat, FoRef->rotation);
}

///
//Scales a frame of reference
//
//Parameters:
//	FoRef: The frame of reference being scaled
//	scaleVector: A 3 component Vector containing X, Y, and Z scale factors
void FrameOfReference_Scale(FrameOfReference* FoRef, const Vector* scaleVector)
{
	for (int i = 0; i < 3; i++)
	{
		*Matrix_Index(FoRef->scale, i, i) *= scaleVector->components[i];
	}
}


///
//Constructs a 3x3 rotation matrix to rotate the coordinate system of a frame of reference
//
//PArameters:
//	destination: The destination of the matrix
//	axis: The axis to rotate around
//	radians: The amound of radians to rotate by
void FrameOfReference_ConstructRotationMatrix(Matrix* destination, const Vector* axis, const float radians)
{
	//Row 1
	*Matrix_Index(destination, 0, 0) = cosf(radians) + powf(axis->components[0], 2.0f) * (1.0f - cosf(radians));
	*Matrix_Index(destination, 0, 1) = axis->components[0] * axis->components[1] * (1.0f - cosf(radians)) - axis->components[2] * sinf(radians);
	*Matrix_Index(destination, 0, 2) = axis->components[0] * axis->components[2] * (1.0f - cosf(radians)) + axis->components[1] * sinf(radians);

	//Row 2
	*Matrix_Index(destination, 1, 0) = axis->components[0] * axis->components[1] * (1.0f - cosf(radians)) + axis->components[2] * sinf(radians);
	*Matrix_Index(destination, 1, 1) = cosf(radians) + powf(axis->components[1], 2.0f) * (1.0f - cosf(radians));
	*Matrix_Index(destination, 1, 2) = axis->components[1] * axis->components[2] * (1.0f - cosf(radians)) - axis->components[0] * sinf(radians);

	//Row 3
	*Matrix_Index(destination, 2, 0) = axis->components[0] * axis->components[2] * (1.0f - cosf(radians)) - axis->components[1] * sinf(radians);
	*Matrix_Index(destination, 2, 1) = axis->components[1] * axis->components[2] * (1.0f - cosf(radians)) + axis->components[0] * sinf(radians);
	*Matrix_Index(destination, 2, 2) = cosf(radians) + powf(axis->components[2], 2.0f) * (1.0f - cosf(radians));
}

///
//Compiles the Frame of Reference into a 4x4 matrix
//Where the first 3x3 is a rotation matrix with scale information on the diagnol
//And the 4th column is a Vector 3 holding xyz position info
//
//Parameters:
//	source: THe frame of reference to inscribe into a matrix
//	dest: A pointer to a 4x4 destination matrix
void FrameOfReference_ToMatrix4(FrameOfReference* source, Matrix* dest)
{
	Matrix temp;
	Matrix_INIT_ON_STACK(temp, 3, 3);

	Matrix_GetProductMatrix(&temp, source->rotation, source->scale);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			*Matrix_Index(dest, i, j) = Matrix_GetIndex(&temp, i, j);
		}	
		*Matrix_Index(dest, i, 3) = source->position->components[i];
	}
}

///
//Sets the position of a frame of reference
//
//Parameters:
//      frame: The frame of reference to set the position of
//      position: The new position
void FrameOfReference_SetPosition(FrameOfReference* frame, const Vector* position)
{
        Vector_Copy(frame->position, position);
}

///
//Sets the rotation of a frame of reference in world space
//
//Parameters:
//      frame: The game object to set the rotation of
//      rotation: The matrix to set the rotation to
void FrameOfReference_SetRotation(FrameOfReference* frame, Matrix* rotation)
{
	Matrix_Copy(frame->rotation, rotation);
}

