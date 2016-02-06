#ifndef FRAMEOFREFERENCE_H
#define FRAMEOFREFERENCE_H

#include "../Math/Matrix.h"

#define FrameOfReference_INIT_ON_STACK( frame ) \
	Matrix scale##frame; \
	Matrix rotation##frame; \
	Vector position##frame; \
	Matrix_INIT_ON_STACK( scale##frame , 3, 3); \
	Matrix_INIT_ON_STACK( rotation##frame , 3, 3); \
	Vector_INIT_ON_STACK( position##frame , 3); \
	frame.scale = &(scale##frame); \
	frame.rotation = &(rotation##frame); \
	frame.position = &(position##frame);

typedef struct FrameOfReference
{
	Matrix* scale;
	Matrix* rotation; // Where the 3 columns represent the Right, Up, and Back Vectors,
	Vector* position; // In worldspace	

} FrameOfReference;

///
//Allocates memory for a Frame of Reference
//
//Returns:
//	Pointer to newly allocated uninitialized Frame of Reference
FrameOfReference* FrameOfReference_Allocate(void);

///
//Initializes a Frame of REference
//
//Parameters:
//	FoRef: The frame of reference to initialize
void FrameOfReference_Initialize(FrameOfReference* FoRef);

///
//Initializes a deep copy of a frame of reference
//This means any pointers will point to a Newly Allocated instance of identical memory
//
//Parameters:
//	copy: A pointer to an uninitialized FrameOfreference to initialize as a deep copy
//	original: A pointer to a FrameOfreference to deep copy
void FrameOfReference_InitializeDeepCopy(FrameOfReference* copy, const FrameOfReference* original);

///
//Frees resources allocated for a Frame of Reference
//
//Parameters:
//	FoRef: A pointer to the frame of reference to free
void FrameOfReference_Free(FrameOfReference* FoRef);

///
//Translates the origin of a Frame of Reference
//
//Parameters:
//	FoRef: The frame of reference being translated
//	translation: The 3 component translation Vector
void FrameOfReference_Translate(FrameOfReference* FoRef, const Vector* translation);

///
//Rotates a Frame of Reference
//
//Parameters:
//	FoRef: The frame of reference being rotated
//	axis: The axis being rotated around (Relative to the frame of reference)
//	radians: The amount of radians to rotate by
void FrameOfReference_Rotate(FrameOfReference* FoRef, const Vector* axis, const float radians);

///
//Scales a frame of reference
//
//Parameters:
//	FoRef: The frame of reference being scaled
//	scaleVector: A 3 component Vector containing X, Y, and Z scale factors
void FrameOfReference_Scale(FrameOfReference* FoRef, const Vector* scaleVector);

///
//Constructs a 3x3 rotation matrix to rotate the coordinate system of a frame of reference
//
//PArameters:
//	destination:	The destination of the matrix
//	axis: The axis to rotate around
//	radians: The amound of radians to rotate by
void FrameOfReference_ConstructRotationMatrix(Matrix* destination, const Vector* axis, const float radians);

///
//Compiles the Frame of Reference into a 4x4 matrix
//Where the first 3x3 is a rotation matrix with scale information on the diagnol
//And the 4th column is a Vector 3 holding xyz position info
//
//Parameters:
//	source: THe frame of reference to inscribe into a matrix
//	dest: A pointer to a 4x4 destination matrix
void FrameOfReference_ToMatrix4(FrameOfReference* source, Matrix* dest);

///
//Sets the positio of a FrameOfReference
//
//Parameters:
//  source: The frame of reference to set the position of
//  frame: The position to move the object to
void FrameOfReference_SetPosition(FrameOfReference* frame, const Vector* position);

///
//Sets the rotation of a GObject
//
//Parameters:
//  frame: The frame of reference to set the rotation of
//  position: The rotation to set the frame to
void FrameOfReference_SetRotation(FrameOfReference* frame, Matrix* rotation);

#endif
