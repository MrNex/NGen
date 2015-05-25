#include "Mesh.h"

#include <stdio.h>
#include <math.h>
#include <string.h>

///
//Allocates memory for a new Mesh
//
//Parameters:
//	numTriangles: The number of triangles the mesh should contain
//
//Returns:
//	Pointer to a new mesh
Mesh* Mesh_Allocate()
{
	Mesh* m = (Mesh*)malloc(sizeof(Mesh));
	m->VAO = 0;
	m->VBO = 0;
	m->numTriangles = 0;
	m->triangles = 0;
	m->primitive = GL_TRIANGLES;
	return m;
}

///
//Initializes a mesh
//
//Parameters:
//	m: The mesh to initialize
//	tris: An array of triangles representing the mesh
//	numTriangles: the amount of triangles
//	usagePattern: The usage pattern of the mesh's data (GL_STATIC_DRAW | GL_STREAM_DRAW | GL_DYNAMIC_DRAW)
void Mesh_Initialize(Mesh* m, struct Triangle* tris, unsigned int numTriangles, GLenum usagePattern)
{
	m->numTriangles = numTriangles;
	m->triangles = (struct Triangle*)malloc(sizeof(struct Triangle) * m->numTriangles);

	memcpy(m->triangles, tris, sizeof(struct Triangle) * m->numTriangles);

	GenerateBuffers(m, usagePattern);
}

///
//Generates Vertex buffer & array objects for a mesh
//
//Parameters:
//	m: The mesh to generate VBO & VAO for
//	usagePatter: The usage pattern of the mesh's data (GL_STATIC_DRAW | GL_STREAM_DRAW | GL_DYNAMIC_DRAW)
static void GenerateBuffers(Mesh* m, GLenum usagePattern)
{
	printf("Generating buffers\n");
	glGenVertexArrays(1, &m->VAO);
	glBindVertexArray(m->VAO);

	glGenBuffers(1, &m->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m->VBO);

	m->usagePattern = usagePattern;

	glBufferData(
		/*Type*/	GL_ARRAY_BUFFER,
		/*Size*/	sizeof(struct Triangle) * m->numTriangles,
		/*Data*/	m->triangles,
		/*Changes?*/m->usagePattern
		);

	//Position Attribute
	glVertexAttribPointer(
		/*Attr. Index*/	0,
		/*Num Element*/	3,
		/*Type*/		GL_FLOAT,
		/*Normalize?*/	GL_FALSE,
		/*Stride*/		sizeof(struct Vertex),
		/*Offset*/		(void*)0
		);

	//Texture coordinate attribute
	glVertexAttribPointer(
		/*Attr. Index*/	1,
		/*Num Element*/	2,
		/*Type*/		GL_FLOAT,
		/*Normalize?*/	GL_FALSE,
		/*Stride*/		sizeof(struct Vertex),
		/*Offset*/		(void*)(3 * sizeof(float))
		);

	//Texture coordinate attribute
	glVertexAttribPointer(
		/*Attr. Index*/	2,
		/*Num Element*/	3,
		/*Type*/		GL_FLOAT,
		/*Normalize?*/	GL_FALSE,
		/*Stride*/		sizeof(struct Vertex),
		/*Offset*/		(void*)(5 * sizeof(float))
		);

	glEnableVertexAttribArray(0);	//Position
	glEnableVertexAttribArray(1);	//Texture
	glEnableVertexAttribArray(2);	//Normal

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
//Calculates the centroid of a CLOSED mesh. A closed mesh
//constitutes a mesh which is composed of outward facing faces. This is to say that
//in order to view any face from the inside out you would first need to clip through a face facing you.
//
//Parameters:
//	dest: A pointer to the vector to store the centroid
//	mesh: The mesh to calculate the centroid of
void Mesh_CalculateCentroid(Vector* dest, const Mesh* mesh)
{
	//Set dest as 0
	Vector_Copy(dest, &Vector_ZERO);

	//Add up all vertices in each triangle
	for(unsigned int i = 0; i < mesh->numTriangles; i++)
	{
		dest->components[0] += mesh->triangles[i].a.x;
		dest->components[1] += mesh->triangles[i].a.y;
		dest->components[2] += mesh->triangles[i].a.z;

		dest->components[0] += mesh->triangles[i].b.x;
		dest->components[1] += mesh->triangles[i].b.y;
		dest->components[2] += mesh->triangles[i].b.z;

		dest->components[0] += mesh->triangles[i].c.x;
		dest->components[1] += mesh->triangles[i].c.y;
		dest->components[2] += mesh->triangles[i].c.z;
	}

	Vector_PrintTranspose(dest);

	//Divide by the number of vertices
	Vector_Scale(dest, 1.0f/(mesh->numTriangles * 3.0f));
}

///
//Calculates the maximum dimensions of a meshes model space
//That is the furthest X, Y, and Z values of any point from the centroid of the mesh
//
//Parameters:
//	dest: A pointer to The vector (of dimension 3) to store the maximum dimensions in
//	mesh: A pointer to The mesh to calculate the maximum dimensions of
//	centroid: A pointer to a vector containing the center of a mesh
void Mesh_CalculateMaxDimensions(Vector* dest, const Mesh* mesh, const Vector* centroid)
{
	float xDist = 0.0f;
	float yDist = 0.0f;
	float zDist = 0.0f;

	Vector_Copy(dest, &Vector_ZERO);

	//For each triangle
	for(unsigned int i = 0; i < mesh->numTriangles; i++)
	{
		//Get the |distance| of each vertex A's dimension from the centroid
		xDist = fabs(mesh->triangles[i].a.x - centroid->components[0]);
		yDist = fabs(mesh->triangles[i].a.y - centroid->components[1]);
		zDist = fabs(mesh->triangles[i].a.z - centroid->components[2]);

		//if it's greater than the current dimension, set it as the current dimension
		if(xDist > dest->components[0]) dest->components[0] = xDist;
		if(yDist > dest->components[1]) dest->components[1] = yDist;
		if(zDist > dest->components[2]) dest->components[2] = zDist;

		//Get the |distance| of each vertex B's dimension from the centroid
		xDist = fabs(mesh->triangles[i].b.x - centroid->components[0]);
		yDist = fabs(mesh->triangles[i].b.y - centroid->components[1]);
		zDist = fabs(mesh->triangles[i].b.z - centroid->components[2]);

		//if it's greater than the current dimension, set it as the current dimension
		if(xDist > dest->components[0]) dest->components[0] = xDist;
		if(yDist > dest->components[1]) dest->components[1] = yDist;
		if(zDist > dest->components[2]) dest->components[2] = zDist;

		//Get the |distance| of each vertex C's dimension from the centroid
		xDist = fabs(mesh->triangles[i].c.x - centroid->components[0]);
		yDist = fabs(mesh->triangles[i].c.y - centroid->components[1]);
		zDist = fabs(mesh->triangles[i].c.z - centroid->components[2]);

		//if it's greater than the current dimension, set it as the current dimension
		if(xDist > dest->components[0]) dest->components[0] = xDist;
		if(yDist > dest->components[1]) dest->components[1] = yDist;
		if(zDist > dest->components[2]) dest->components[2] = zDist;
	}

	//The centroid to each vertex location represents 1/2 the actual AABB dimension, we must scale the dimensions by 2
	Vector_Scale(dest, 2.0f);
}

///
//Renders a mesh
//
//Parameters:
//	m:The mesh to render
void Mesh_Render(Mesh* m, GLenum renderMode)
{
	glBindVertexArray(m->VAO);

	if(m->usagePattern == GL_DYNAMIC_DRAW)
	{
		GLvoid* memory = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
		memcpy(memory, m->triangles, m->numTriangles * 3 * sizeof(struct Vertex));
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	glDrawArrays(
		/*Primitive*/	renderMode,
		/*Offset*/		0,
		/*numVertices*/	m->numTriangles * 3
		);


}
