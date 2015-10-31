#include <GL/glew.h>
#include <GL/freeglut.h>

#include "FrameOfReference.h"

typedef struct Camera
{
	Matrix* translationMatrix;
	Matrix* rotationMatrix;
	Matrix* projectionMatrix;

	float aspectX, aspectY;

	float nearPlane, farPlane;
	float leftPlane, rightPlane;
	float topPlane, bottomPlane;
} Camera;

///
//Allocates memory for a camera
//
//Returns:
//	Pointer to newly allocated camera
Camera* Camera_Allocate();

///
//Initializes a camera
//
//Parameters:
//	cam: The camera to initialize
void Camera_Initialize(Camera* cam);

///
//Frees resources being taken by a camera
//
//Parameters:
//	cam: The camera which we are deleting
void Camera_Free(Camera* cam);

///
//Recalculates the perspective matrix based on the camera's attributes
//
//Parameters:
//	cam: The camera which has a perspective matrix we want to refresh.
void Camera_RefreshPerspectiveMatrix(Camera* cam);

///
//Translates the camera
//
//Parameters:
//	cam: The camera to translate
//	translation: The Vector to translate by (NON CONST)
void Camera_Translate(Camera* cam, Vector* translation);

//
// Set the position of the camera
// Parameters:
//  cam: The camera object
//  translation: The vector position
void Camera_SetPosition(Camera* cam, Vector* translation);

///
//Rotates the camera
//
//Parameters:
//	Cam: The camera to rotate
//	axis: The axis to rotate around
//	radians: The angle in radians to rotate by
void Camera_Rotate(Camera* cam, Vector* axis, const float radians);

///
//Increments the pitch of the camera by a certain angle in radians
//
//Parameters:
//	cam: A pointer to the camera having it's pitch changed
//	radians: The radians to alter the pitch by
void Camera_ChangePitch(Camera* cam, const float radians);

///
//Increments the yaw of the camera by a certain angle in radians
//
//Parameters:
//	cam: A pointer to the camera having it's yaw changed
//	radians: The radians to alter the yaw by
void Camera_ChangeYaw(Camera* cam, const float radians);

///
//Compiles the camera into a 4x4 view matrix containing a translation to camera view
//Where the first 3x3 is a rotation matrix with scale information on the diagnol
//And the 4th column is a Vector 3 holding Rx(x)+Ry(y)+Rz(z)
//
//Parameters:
//	source: THe camera to inscribe into a matrix
//	dest: A pointer to a 4x4 destination matrix
void Camera_ToMatrix4(Camera* source, Matrix* dest);


