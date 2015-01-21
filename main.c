#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>


#include "Math/Matrix.h"

#include "Render/Mesh.h"

Mesh* mesh;

///
//Initializes all engine components
void Init(void)
{
	printf("Initializing\n");

	//There currently are no other components to the engine
	
	//Set the background color to fully opaque Red
	//TODO: Move background color setting to the render manager once complete
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);



	//New Vector code
	Vector a;
	Vector b;
	Vector c;

	Vector_INIT_ON_STACK(a, 3);
	Vector_INIT_ON_STACK(b, 3);
	Vector_INIT_ON_STACK(c, 3);

	for(int i = 0; i < a.dimension; i++)
	{
		a.components[i] = 1;
		b.components[i] = 2;
	}

	Vector_PrintTranspose(&a);
	Vector_PrintTranspose(&b);
	Vector_PrintTranspose(&c);

	Vector_Scale(&b, 2.0f);

	Vector_Add(&c, &a, &b);

	Vector_PrintTranspose(&a);
	Vector_PrintTranspose(&b);
	Vector_PrintTranspose(&c);
	
	Vector_Decrement(&a, &c);
	
	Vector_Increment(&c, &a);

	Vector_PrintTranspose(&a);
	Vector_PrintTranspose(&b);
	Vector_PrintTranspose(&c);

	printf("A dot B = %f", Vector_DotProduct(&a, &b));


	//Matrix code
	Matrix m;
	Matrix n;
	Matrix o;
	Matrix p;

	Matrix_INIT_ON_STACK(m, 3, 3);
	Matrix_INIT_ON_STACK(n, 3, 3);
	Matrix_INIT_ON_STACK(p, 3, 2);
	Matrix_INIT_ON_STACK(o, 3, 2);


	Matrix_Print(&m);

	*Matrix_Index(&m, 0, 2) = -3.0f;
	*Matrix_Index(&n, 2, 2) = 2.0f;
	*Matrix_Index(&o, 0, 0) = 5.0f;
	*Matrix_Index(&o, 2, 0) = 1.0f;
	*Matrix_Index(&o, 1, 1) = -22.0f;

	printf("\n\n");
	Matrix_Print(&m);
	printf("\n");
	Matrix_Print(&n);
	printf("\n");
	Matrix_Print(&o);

	Matrix_TransformMatrix(&m, &n);
	Matrix_GetProductMatrix(&p, &n, &o);

	printf("\n\n");
	Matrix_Print(&m);
	printf("\n");
	Matrix_Print(&n);
	printf("\n");
	Matrix_Print(&o);
	printf("\n");
	Matrix_Print(&p);

	Vector d;
	Vector_INIT_ON_STACK(d, 3);

	Matrix_TransformVector(&m, &c);
	Matrix_GetProductVector(&d, &n, &c);

	Vector_PrintTranspose(&c);
	Vector_PrintTranspose(&d);


	struct Vertex va =
	{
		-1.0f,
		-1.0f,
		1.0f
	};

	struct Vertex vb =
	{
		0.0f,
		1.0f,
		1.0f
	};

	struct Vertex vc =
	{
		1.0f,
		-1.0f,
		1.0f
	};

	struct Triangle t = 
	{
		va,
		vb,
		vc
	};

	t.a = va;
	t.b = vb;
	t.c = vc;	
	
	mesh = Mesh_Allocate(1);
	Mesh_Initialize(mesh, &t);


}

///
//Checks for any OpenGL errors and prints error code if found
void CheckGLErrors(void)
{
	//Get the openGL error code
	int error = glGetError();

	//If the error code signifies an error has occurred
	if(error != GL_NO_ERROR)
	{
		//Print the error code
		printf("Error: %d\n", error);
	}
}

///
//Updates engine
//
//Parameters:
//	int val - Unused, needed for valid glutTimerFunc signature
void Update(int val)
{
	//printf("Updating\n");

	//Put update code here
	
	//Start drawing
	glutPostRedisplay();

	//Recall update in 1/60 seconds
	glutTimerFunc(16, Update, 0);
}


///
//Draws the current state of the engine
void Draw(void)
{
	//printf("Drawing\n");

	//Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw code here
	Mesh_Render(mesh);
	
	//Start drawing threads on gpu
	glFlush();
}


///
//Program will begin here
//
//Parameters:
//	int argc - Number of arguments passed from cmd line
//	char* argv[] - Array of C strings of arguments passed from cmd line
int main(int argc, char* argv[])
{
	//Initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(4,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//Window creation
	int win = glutCreateWindow("NGen V1.0");

	glewExperimental = GL_TRUE;
	if(glewInit() != GLEW_OK) { return -1; }
	

	//Check for errors
	CheckGLErrors();

	//Set up callback registration
	glutTimerFunc(16, Update, 0);
	glutDisplayFunc(Draw);

	//Initialize engine
	Init();

	//Start the main loop
	glutMainLoop();

	//When the loop is over, release all memory
	glutDestroyWindow(win);

	
	return 0;
}
