#ifndef MESH
#define MESH

//#include <windows.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "../Math/Matrix.h"


struct Vertex
{
	float x, y, z;		//Position
	float tx, ty;		//Texture coordinates
	float nx, ny, nz;	//Normal
};

struct Triangle
{
	struct Vertex a, b, c;
};

typedef struct Mesh
{
	GLuint VAO;
	GLuint VBO;
	unsigned int numTriangles;
	struct Triangle* triangles;
	GLenum primitive;
	GLenum usagePattern;
} Mesh;

///
//Allocates memory for a new Mesh
//
//Parameters:
//	numTriangles: The number of triangles the mesh should contain
//
//Returns:
//	Pointer to a new mesh
Mesh* Mesh_Allocate();

///
//Initializes a mesh
//
//Parameters:
//	m: The mesh to initialize
//	tris: An array of triangles representing the mesh
//	numTriangles: the amount of triangles
//	usagePattern: The usage pattern of the mesh's data (GL_STATIC_DRAW | GL_STREAM_DRAW | GL_DYNAMIC_DRAW)
void Mesh_Initialize(Mesh* m, struct Triangle* tris, unsigned int numTriangles, GLenum usagePattern);



///
//Frees a mesh from memory
//
//PArmeters:
//	m: The mesh to free
void Mesh_Free(Mesh* m);

///
//Calculates the centroid of a CLOSED mesh. A closed mesh
//constitutes a mesh which is composed of outward facing faces. This is to say that
//in order to view any face from the inside out you would first need to clip through a face facing you.
//
//Parameters:
//	dest: A pointer to the vector to store the centroid
//	mesh: The mesh to calculate the centroid of
void Mesh_CalculateCentroid(Vector* dest, const Mesh* mesh);

///
//Calculates the maximum dimensions of a meshes model space
//That is the furthest X, Y, and Z values of any point from the centroid of the mesh
//
//Parameters:
//	dest: A pointer to The vector (of dimension 3) to store the maximum dimensions in
//	mesh: A pointer to The mesh to calculate the maximum dimensions of
//	centroid: A pointer to a vector containing the center of a mesh
void Mesh_CalculateMaxDimensions(Vector* dest, const Mesh* mesh, const Vector* centroid);

///
//Renders a mesh
//
//Parameters:
//	m: The mesh to render
void Mesh_Render(Mesh* m, GLenum renderMode);




#endif
