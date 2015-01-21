#include <windows.h>

#include <GL/glew.h>
#include <GL/freeglut.h>

struct Vertex
{
	float x, y, z;		//Position
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
} Mesh;

///
//Allocates memory for a new Mesh
//
//Parameters:
//	numTriangles: The number of triangles the mesh should contain
//
//Returns:
//	Pointer to a new mesh
Mesh* Mesh_Allocate(unsigned int numTriangles);

///
//Initializes a mesh
//
//Parameters:
//	m: The mesh to initialize
//	tris: An array of triangles representing the mesh
void Mesh_Initialize(Mesh* m, struct Triangle* tris);

///
//Generates Vertex buffer & array objects for a mesh
//
//Parameters:
//	m: The mesh to generate VBO & VAO for
static void GenerateBuffers(Mesh* m);

///
//Frees a mesh from memory
//
//PArmeters:
//	m: The mesh to free
void Mesh_Free(Mesh* m);

///
//Renders a mesh
//
//Parameters:
//	m: The mesh to render
void Mesh_Render(Mesh* m);
