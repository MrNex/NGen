#include "Mesh.h"

#include <stdio.h>
#include <string.h>

///
//Allocates memory for a new Mesh
//
//Parameters:
//	numTriangles: The number of triangles the mesh should contain
//
//Returns:
//	Pointer to a new mesh
Mesh* Mesh_Allocate(unsigned int numTriangles)
{
	Mesh* m = (Mesh*)malloc(sizeof(Mesh));
	m->VAO = 0;
	m->VBO = 0;
	m->numTriangles = numTriangles;
	m->triangles = 0;

	return m;
}

///
//Initializes a mesh
//
//Parameters:
//	m: The mesh to initialize
//	tris: An array of triangles representing the mesh
void Mesh_Initialize(Mesh* m, struct Triangle* tris)
{
	m->triangles = (struct Triangle*)malloc(sizeof(struct Triangle) * m->numTriangles);

	memcpy(m->triangles, tris, sizeof(struct Triangle) * m->numTriangles);

	GenerateBuffers(m);
}

///
//Generates Vertex buffer & array objects for a mesh
//
//Parameters:
//	m: The mesh to generate VBO & VAO for
static void GenerateBuffers(Mesh* m)
{
	printf("Generating buffers\n");
	glGenVertexArrays(1, &m->VAO);
	printf("VAO: %d\n", m->VAO);
	glBindVertexArray(m->VAO);

	printf("VAO: %d\n", m->VAO);

	glGenBuffers(1, &m->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m->VBO);

	printf("VAO: %d\nVBO: %d\n", m->VAO, m->VBO);

	glBufferData(
	/*Type*/	GL_ARRAY_BUFFER,
	/*Size*/	sizeof(struct Triangle) * m->numTriangles,
	/*Data*/	m->triangles,
	/*Changes?*/	GL_STATIC_DRAW
	);

	//Position Attribute
	glVertexAttribPointer(
	/*Attr. Index*/	0,
	/*Num Element*/	3,
	/*Type*/	GL_FLOAT,
	/*Normalize?*/	GL_FALSE,
	/*Stride*/	sizeof(struct Vertex),
	/*Offset*/	(void*)0
	);

	glEnableVertexAttribArray(0);

	printf("VAO: %d\nVBO: %d\n", m->VAO, m->VBO);
}

///
//Frees a mesh from memory
//
//PArmeters:
//	m: The mesh to free
void Mesh_Free(Mesh* m)
{
	glDeleteBuffers(1, &m->VBO);
	glDeleteVertexArrays(1, &m->VAO);
	free(m->triangles);
	free(m);
}

///
//Renders a mesh
//
//Parameters:
//	m:The mesh to render
void Mesh_Render(Mesh* m)
{
	glBindVertexArray(m->VAO);

	glDrawArrays(
	/*Primitive*/	GL_TRIANGLES,
	/*Offset*/	0,
	/*numVertices*/	m->numTriangles * 3
	);
}


