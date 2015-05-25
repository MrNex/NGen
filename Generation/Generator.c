#include "Generator.h"

#include <math.h>

///
//Generates a cube mesh with specified width/length/height
//
//Parameters:
//	size: The sidelength of the cube
//
//Returns:
//	Pointer to newly allocated & initialized mesh containing a cube
Mesh* Generator_GenerateCubeMesh(float size)
{
	float extent = size/2.0f;

	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	struct Triangle t;

	//Front Face
	//Let a be bottom left
	t.a.x = -extent;
	t.a.y = -extent;
	t.a.z = extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = 0;
	t.a.nz = 1.0f;

	//Let b be bottom right
	t.b.x = extent;
	t.b.y = -extent;
	t.b.z = extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = 0;
	t.b.nz = 1.0f;

	//Let c be top left
	t.c.x = -extent;
	t.c.y = extent;
	t.c.z = extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = 0;
	t.c.nz = 1.0f;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Let a be top left
	t.a.x = -extent;
	t.a.y = extent;
	t.a.z = extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = 0;
	t.a.nz = 1.0f;

	//Let b be bottom right
	t.b.x = extent;
	t.b.y = -extent;
	t.b.z = extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = 0;
	t.b.nz = 1.0f;

	//Let c be top right
	t.c.x = extent;
	t.c.y = extent;
	t.c.z = extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = 0;
	t.c.nz = 1.0f;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Back Face
	//Let a be bottom left
	t.a.x = -extent;
	t.a.y = -extent;
	t.a.z = -extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = 0;
	t.a.nz = -1.0f;

	//Let b be bottom right
	t.b.x = extent;
	t.b.y = -extent;
	t.b.z = -extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = 0;
	t.b.nz = -1.0f;

	//Let c be top left
	t.c.x = -extent;
	t.c.y = extent;
	t.c.z = -extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = 0;
	t.c.nz = -1.0f;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Let a be top left
	t.a.x = -extent;
	t.a.y = extent;
	t.a.z = -extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = 0;
	t.a.nz = -1.0f;

	//Let b be bottom right
	t.b.x = extent;
	t.b.y = -extent;
	t.b.z = -extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = 0;
	t.b.nz = -1.0f;

	//Let c be top right
	t.c.x = extent;
	t.c.y = extent;
	t.c.z = -extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = 0;
	t.c.nz = -1.0f;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Left Face
	//Let a be front bottom
	t.a.x = -extent;
	t.a.y = -extent;
	t.a.z = extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = -1.0f;
	t.a.ny = 0;
	t.a.nz = 0;

	//Let b be front top
	t.b.x = -extent;
	t.b.y = extent;
	t.b.z = extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = -1.0f;
	t.b.ny = 0;
	t.b.nz = 0;

	//Let c be back top
	t.c.x = -extent;
	t.c.y = extent;
	t.c.z = -extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = -1.0f;
	t.c.ny = 0;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Let a be back top
	t.a.x = -extent;
	t.a.y = extent;
	t.a.z = -extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = -1.0f;
	t.a.ny = 0;
	t.a.nz = 0;

	//Let b be back bottom
	t.b.x = -extent;
	t.b.y = -extent;
	t.b.z = -extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = -1.0f;
	t.b.ny = 0;
	t.b.nz = 0;

	//Let c be front bottom
	t.c.x = -extent;
	t.c.y = -extent;
	t.c.z = extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = -1.0f;
	t.c.ny = 0;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Right Face
	//Let a be front bottom
	t.a.x = extent;
	t.a.y = -extent;
	t.a.z = extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 1.0f;
	t.a.ny = 0;
	t.a.nz = 0;

	//Let b be back top
	t.b.x = extent;
	t.b.y = extent;
	t.b.z = -extent; 

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 1.0f;
	t.b.ny = 0;
	t.b.nz = 0;

	//Let c be front top
	t.c.x = extent;
	t.c.y = extent;
	t.c.z = extent;	

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 1.0f;
	t.c.ny = 0;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Let a be back top
	t.a.x = extent;
	t.a.y = extent;
	t.a.z = -extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 1.0f;
	t.a.ny = 0;
	t.a.nz = 0;

	//Let b be back bottom
	t.b.x = extent;
	t.b.y = -extent;
	t.b.z = -extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 1.0f;
	t.b.ny = 0;
	t.b.nz = 0;

	//Let c be front bottom
	t.c.x = extent;
	t.c.y = -extent;
	t.c.z = extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 1.0f;
	t.c.ny = 0;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Bottom Face
	//Let a be front left
	t.a.x = -extent;
	t.a.y = -extent;
	t.a.z = extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = -1.0f;
	t.a.nz = 0;

	//Let b be front right
	t.b.x = extent;
	t.b.y = -extent;
	t.b.z = extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = -1.0f;
	t.b.nz = 0;

	//Let c be back right
	t.c.x = extent;
	t.c.y = -extent;
	t.c.z = -extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = -1.0f;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Let a be back right
	t.a.x = extent;
	t.a.y = -extent;
	t.a.z = -extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = -1.0f;
	t.a.nz = 0;

	//Let b be back left
	t.b.x = -extent;
	t.b.y = -extent;
	t.b.z = -extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = -1.0f;
	t.b.nz = 0;

	//Let c be front left
	t.c.x = -extent;
	t.c.y = -extent;
	t.c.z = extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = -1.0f;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Top Face
	//Let a be front left
	t.a.x = -extent;
	t.a.y = extent;
	t.a.z = extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = 1.0f;
	t.a.nz = 0;

	//Let b be front right
	t.b.x = extent;
	t.b.y = extent;
	t.b.z = extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = 1.0f;
	t.b.nz = 0;

	//Let c be back right
	t.c.x = extent;
	t.c.y = extent;
	t.c.z = -extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = 1.0f;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	//Let a be back right
	t.a.x = extent;
	t.a.y = extent;
	t.a.z = -extent;

	t.a.tx = 0;
	t.a.ty = 0;

	t.a.nx = 0;
	t.a.ny = 1.0f;
	t.a.nz = 0;

	//Let b be back left
	t.b.x = -extent;
	t.b.y = extent;
	t.b.z = -extent;

	t.b.tx = 0;
	t.b.ty = 0;

	t.b.nx = 0;
	t.b.ny = 1.0f;
	t.b.nz = 0;

	//Let c be front left
	t.c.x = -extent;
	t.c.y = extent;
	t.c.z = extent;

	t.c.tx = 0;
	t.c.ty = 0;

	t.c.nx = 0;
	t.c.ny = 1.0f;
	t.c.nz = 0;

	//Copy data into dynamic array
	DynamicArray_Append(triangles, &t);

	Mesh* cube = Mesh_Allocate();
	Mesh_Initialize(cube, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);

	//Free dynamic array after sending mesh a copy of data
	DynamicArray_Free(triangles);

	return cube;
}

///
//Generates a cylinder mesh to specification
//
//Parameters:
//	radius: The radius of the base of the cylinder
//	height:The height of the cylinder
//	subdivisions: Number of sides of cylinder ( >= 3)
//Returns:
//	Pointer to newly allocated & initialized mesh containing a cylinder
Mesh* Generator_GenerateCylinderMesh(float radius, float height, int subdivisions)
{
	struct Triangle t;
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	if(subdivisions < 3)
	{
		subdivisions = 3;
	}

	float extent = height / 2.0f;

	float stepSize =  (3.14159f * 2.0f) / ((float)subdivisions);


	//Set up vertex values that won't be changing in loop
	//A normals
	t.a.nx = 0;
	t.a.ny = -1.0f;
	t.a.nz = 0;
	//A Texture coords
	t.a.tx = 0;
	t.a.ty = 0;
	//A height
	t.a.y = -extent;

	//B normals
	t.b.nx = 0;
	t.b.ny = -1.0f;
	t.b.nz = 0;
	//B texture coords
	t.b.tx = 0;
	t.b.ty = 0;
	//B height
	t.b.y = -extent;

	//C normals
	t.c.nx = 0;
	t.c.ny = -1.0f;
	t.c.nz = 0;
	//C texture coords
	t.c.tx = 0;
	t.c.ty = 0;
	//C position
	t.c.x = 0.0f;
	t.c.y = -extent;
	t.c.z = 0.0f;

	//When constructing bottom & top faces, 
	//point A in the triangle will be the current edge vertex
	//point B will be the next edge vertex
	//Point C will be the triangle center

	//Bottom face
	for(int i = 0; i < subdivisions; i++)
	{
		t.a.x = cos(stepSize * i) * radius;
		t.a.z = sin(stepSize * i) * radius;

		t.b.x = cos(stepSize * (i + 1)) * radius;
		t.b.z = sin(stepSize * (i + 1)) * radius;

		//Append triangle to array
		DynamicArray_Append(triangles, &t);
	}

	//Set up non changing values of vertices for top face
	t.a.ny = 1.0f;
	t.b.ny = 1.0f;
	t.c.ny = 1.0f;

	t.a.y = extent;
	t.b.y = extent;
	t.c.y = extent;

	//Top face
	for(int i = 0; i < subdivisions; i++)
	{
		t.a.x = cos(stepSize * i) * radius;
		t.a.z = sin(stepSize * i) * radius;

		t.b.x = cos(stepSize * (i+1)) * radius;
		t.b.z = sin(stepSize * (i+1)) * radius;

		//Append triangle copy to array
		DynamicArray_Append(triangles, &t);
	}

	//These are for figuring out normals
	Vector u;
	Vector_INIT_ON_STACK(u, 3);
	Vector v;
	Vector_INIT_ON_STACK(v, 3);
	Vector w;
	Vector_INIT_ON_STACK(w, 3);


	//Index i and i + subdivisions are corresponding triangles for every i < subdivisions.
	//Connect the top and bottom faces
	for(int i = 0; i < subdivisions; i++)
	{
		//First triangle making up quad
		t.a = ((struct Triangle*)DynamicArray_Index(triangles, i))->a;
		t.b = ((struct Triangle*)DynamicArray_Index(triangles, i))->b;
		t.c = ((struct Triangle*)DynamicArray_Index(triangles, i + subdivisions))->a;

		//Determine the normal of the face
		u.components[0] = t.b.x - t.a.x;
		u.components[1] = t.b.y - t.a.y;
		u.components[2] = t.b.z - t.a.z;

		v.components[0] = t.c.x - t.a.x;
		v.components[1] = t.c.y - t.a.y;
		v.components[2] = t.c.z - t.a.z;

		Vector_CrossProduct(&w, &v, &u);

		//Assign normals
		t.c.nx = t.b.nx = t.a.nx = w.components[0];
		t.c.ny = t.b.ny = t.a.ny = w.components[1];
		t.c.nz = t.b.nz = t.a.nz = w.components[2];

		//append copy of triangle to array
		DynamicArray_Append(triangles, &t);

		//Second triangle making up quad
		t.a = ((struct Triangle*)DynamicArray_Index(triangles, i + subdivisions))->a;
		t.b = ((struct Triangle*)DynamicArray_Index(triangles, i))->b;
		t.c = ((struct Triangle*)DynamicArray_Index(triangles, i + subdivisions))->b;

		//Determine the normal of the face
		u.components[0] = t.b.x - t.a.x;
		u.components[1] = t.b.y - t.a.y;
		u.components[2] = t.b.z - t.a.z;

		v.components[0] = t.c.x - t.a.x;
		v.components[1] = t.c.y - t.a.y;
		v.components[2] = t.c.z - t.a.z;

		Vector_CrossProduct(&w, &v, &u);

		//Assign normals
		t.c.nx = t.b.nx = t.a.nx = w.components[0];
		t.c.ny = t.b.ny = t.a.ny = w.components[1];
		t.c.nz = t.b.nz = t.a.nz = w.components[2];

		//append copy of triangle to array
		DynamicArray_Append(triangles, &t);
	}

	Mesh* cylinder = Mesh_Allocate();
	Mesh_Initialize(cylinder, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);

	//Free dynamic array after mesh has copy of data
	DynamicArray_Free(triangles);

	return cylinder;
}

///
//Generates a cone mesh to specification
//Parameters:
//	radius: The radius of the base of the cone
//	height: The height of the cone
//	subdivisions: The number of sides of the cone
//Returns:
//	Pointer to newly allocated & initialized mesh containing a cone
Mesh* Generator_GenerateConeMesh(float radius, float height, int subdivisions)
{
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	Triangle t;

	if(subdivisions < 3) 
	{
		subdivisions = 3;
	}

	float stepSize = (3.14159f * 2.0f) / subdivisions;
	float extent = height / 2.0f;

	//Set up triangle attributes which will not be changing for the base
	t.a.nx = t.b.nx = t.c.nx = 0.0f;
	t.a.ny = t.b.ny = t.c.ny = -1.0f;
	t.a.nz = t.b.nz = t.c.nz = 0.0f;

	t.a.y = t.b.y = t.c.y = -extent;

	t.a.tx = t.b.tx = t.c.tx = 0.0f;
	t.a.ty = t.b.ty = t.c.ty = 0.0f;

	//Set point c at center of bottom face
	t.c.x = 0.0f;
	t.c.z = 0.0f;

	//Create base
	//Let point a be the current edge vertex,
	//Point b will be the next edge vertex,
	//Point c will be the center of bottom face
	for(int i = 0; i < subdivisions; i++)
	{
		t.a.x = cos(stepSize * i) * radius;
		t.a.z = sin(stepSize * i) * radius;

		t.b.x = cos(stepSize * (i + 1)) * radius;
		t.b.z = sin(stepSize * (i + 1)) * radius;

		DynamicArray_Append(triangles, &t);


	}

	//Connect base to upper point
	t.c.y = extent;

	//These vectors are for computing normals
	Vector u, v, w;
	Vector_INIT_ON_STACK(u, 3);
	Vector_INIT_ON_STACK(v, 3);
	Vector_INIT_ON_STACK(w, 3);


	for(int i = 0; i < subdivisions; i++)
	{
		t.a = ((struct Triangle*)DynamicArray_Index(triangles, i))->a;
		t.b = ((struct Triangle*)DynamicArray_Index(triangles, i))->b;

		u.components[0] = t.b.x - t.a.x;
		u.components[1] = t.b.y - t.a.y;
		u.components[2] = t.b.z - t.a.z;

		v.components[0] = t.c.x - t.a.x;
		v.components[1] = t.c.y - t.a.y;
		v.components[2] = t.c.z - t.a.z;

		//Compute normals
		Vector_CrossProduct(&w, &v, &u);

		//Assign normals
		t.c.nx = t.b.nx = t.a.nx = w.components[0];
		t.c.ny = t.b.ny = t.a.ny = w.components[1];
		t.c.nz = t.b.nz = t.a.nz = w.components[2];

		DynamicArray_Append(triangles, &t);
	}

	//Create mesh
	Mesh* cone = Mesh_Allocate();
	Mesh_Initialize(cone, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);

	//Free memory from dynamic array now that mesh has copy of triangles
	DynamicArray_Free(triangles);

	return cone;
}

///
//Generates a tube mesh to specification
//Parameters:
//	outerRadius: The radius of the external part of tube
//	inner radius: The radius of the inner part of tube
//	height: The height of the tube
//	subdivisions: The number of sides of the tube
//Returns:
//	Pointer to newly allocated & initialized mesh containing a tube
Mesh* Generator_GenerateTubeMesh(float outerRadius, float innerRadius, float height, int subdivisions)
{
	struct Triangle t;
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	if(subdivisions < 3) 
	{
		subdivisions = 3;
	}

	float stepSize = (3.14159f * 2.0f) / subdivisions;
	float extent = height / 2.0f;

	//Set up triangle attributes which will not be changing for bottom base
	//Normals
	t.a.nx = t.b.nx = t.c.nx = 0.0f;
	t.a.ny = t.b.ny = t.c.ny = -1.0f;
	t.a.nz = t.b.nz = t.c.nz = 0.0f;

	//y pos
	t.a.y = t.b.y = t.c.y = -extent;

	//texture coordinates
	t.a.tx = t.b.tx = t.c.tx = 0.0f;
	t.a.ty = t.b.ty = t.c.ty = 0.0f;

	//Create bottom base
	for(int i = 0; i < subdivisions; i++)
	{

		//Let A be the current point on the outer radius
		//Let B be the current point on the inner radius
		//Let C be the next point on the outer radius

		t.a.x = cos(stepSize * i) * outerRadius;
		t.a.z = sin(stepSize * i) * outerRadius;

		t.b.x = cos(stepSize * i) * innerRadius;
		t.b.z = sin(stepSize * i) * innerRadius;

		t.c.x = cos(stepSize * (i + 1)) * outerRadius;
		t.c.z = sin(stepSize * (i + 1)) * outerRadius;

		DynamicArray_Append(triangles, &t);



		//Let A be the current point on the inner radius
		//Let B be the current point on the outer radius
		//Let C be the next point on the inner radius

		t.a.x = cos(stepSize * i) * innerRadius;
		t.a.z = sin(stepSize * i) * innerRadius;

		t.b.x = cos(stepSize * (i+1)) * outerRadius;
		t.b.z = sin(stepSize * (i+1)) * outerRadius;

		t.c.x = cos(stepSize * (i+1)) * innerRadius;
		t.c.z = sin(stepSize * (i+1)) * innerRadius;

		DynamicArray_Append(triangles, &t);

	}

	//Set up triangle attributes which will not change for the top base
	//Normals
	t.a.nx = t.b.nx = t.c.nx = 0.0f;
	t.a.ny = t.b.ny = t.c.ny = 1.0f;
	t.a.nz = t.b.nz = t.c.nz = 0.0f;

	//y pos
	t.a.y = t.b.y = t.c.y = extent;

	//Create top base
	for(int i = 0; i < subdivisions; i++)
	{

		//Let A be the current point on the outer radius
		//Let B be the current point on the inner radius
		//Let C be the next point on the outer radius

		t.a.x = cos(stepSize * i) * outerRadius;
		t.a.z = sin(stepSize * i) * outerRadius;

		t.b.x = cos(stepSize * i) * innerRadius;
		t.b.z = sin(stepSize * i) * innerRadius;

		t.c.x = cos(stepSize * (i + 1)) * outerRadius;
		t.c.z = sin(stepSize * (i + 1)) * outerRadius;

		DynamicArray_Append(triangles, &t);

		//Let A be the current point on the inner radius
		//Let B be the next point on the outer radius
		//Let C be the next point on the inner radius

		t.a.x = cos(stepSize * i) * innerRadius;
		t.a.z = sin(stepSize * i) * innerRadius;

		t.b.x = cos(stepSize * (i+1)) * outerRadius;
		t.b.z = sin(stepSize * (i+1)) * outerRadius;

		t.c.x = cos(stepSize * (i+1)) * innerRadius;
		t.c.z = sin(stepSize * (i+1)) * innerRadius;

		DynamicArray_Append(triangles, &t);
	}

	//Create outer sides

	//These are for computing normals
	Vector u, v, w;
	Vector_INIT_ON_STACK(u, 3);
	Vector_INIT_ON_STACK(v, 3);
	Vector_INIT_ON_STACK(w, 3);

	//triangles[i] and triangles[i + subdivisions] should be corresponding triangles
	//If i % 2 == 0 then points A and C on triangle are on outer perimeter and b is on inner
	//Else points A and C are on inner perimeter and b is on outer
	for(int i = 0; i < subdivisions * 2; i++)
	{

		t.a = ((struct Triangle*)DynamicArray_Index(triangles, i))->a;
		t.b = ((struct Triangle*)DynamicArray_Index(triangles, i + (subdivisions * 2)))->a;
		t.c = ((struct Triangle*)DynamicArray_Index(triangles, i))->c;

		//Get vectors in plane
		u.components[0] = t.b.x - t.a.x;
		u.components[1] = t.b.y - t.a.y;
		u.components[2] = t.b.z - t.a.z;

		v.components[0] = t.c.x - t.a.x;
		v.components[1] = t.c.y - t.a.y;
		v.components[2] = t.c.z - t.a.z;

		//If outer face
		if(i%2 == 0)
		{
			Vector_CrossProduct(&w, &u, &v);
		}
		//Else inner face
		else
		{
			Vector_CrossProduct(&w, &v, &u);
		}
		//Assign normals
		t.a.nx = t.b.nx = t.c.nx = w.components[0];
		t.a.ny = t.b.ny = t.c.ny = w.components[1];
		t.a.nz = t.b.nz = t.c.nz = w.components[2];

		//Add triangle
		DynamicArray_Append(triangles, &t);

		//Get other triangle in quad
		t.a = ((struct Triangle*)DynamicArray_Index(triangles, i))->c;
		t.b = ((struct Triangle*)DynamicArray_Index(triangles, i + (subdivisions*2)))->c;
		t.c = ((struct Triangle*)DynamicArray_Index(triangles, i + (subdivisions*2)))->a;

		//Compute normals
		//Get vectors in plane
		u.components[0] = t.b.x - t.a.x;
		u.components[1] = t.b.y - t.a.y;
		u.components[2] = t.b.z - t.a.z;

		v.components[0] = t.c.x - t.a.x;
		v.components[1] = t.c.y - t.a.y;
		v.components[2] = t.c.z - t.a.z;

		//If outer face
		if(i % 2 == 0)
		{
			Vector_CrossProduct(&w, &v, &u);
		}
		//Else inner face
		else
		{
			Vector_CrossProduct(&w, &u, &v);
		}

		//Assign normals
		t.a.nx = t.b.nx = t.c.nx = w.components[0];
		t.a.ny = t.b.ny = t.c.ny = w.components[1];
		t.a.nz = t.b.nz = t.c.nz = w.components[2];

		//Add triangle
		DynamicArray_Append(triangles, &t);

	}

	Mesh* tube = Mesh_Allocate();
	Mesh_Initialize(tube, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);

	//Free memory from dynamic array now that mesh has copy of triangles
	DynamicArray_Free(triangles);

	return tube;
}

///
//Generates a sphere mesh to specification
//Parameters:
//	radius: The radius of the sphere
//	subdivisions: The number of "sides" of the sphere
//Returns:
//	Pointer to newly allocated & initialized mesh containing a sphere
Mesh* Generator_GenerateSphereMesh(float radius, int subdivisions)
{
	struct Triangle t;
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	if(subdivisions < 3)
	{
		subdivisions = 3;
	}

	float stepSize = (3.14159f * 2.0f) / subdivisions;

	//For computing normals
	Vector u, v, w;
	Vector_INIT_ON_STACK(u, 3);
	Vector_INIT_ON_STACK(v, 3);
	Vector_INIT_ON_STACK(w, 3);

	//Set triangle attributes which will not change
	//Texture coordinates
	t.a.tx = t.a.ty = t.b.tx = t.b.ty = t.c.tx = t.c.ty = 0.0f;	//They are all 0.

	int lim = subdivisions %2 == 0 ? subdivisions / 2 : (subdivisions / 2) + 1;

	//Rotate around Y
	for(int i = 0; i < lim; i++)
	{
		//Rotate points around Z
		for(int j = 0; j < subdivisions; j++)
		{
			t.a.x = radius * cos(i * stepSize) * cos(j * stepSize);
			t.a.y = radius * sin(j * stepSize);
			t.a.z = -radius * sin(i * stepSize) * cos(j * stepSize);

			t.b.x = radius * cos((i+1) * stepSize) * cos(j * stepSize);
			t.b.y = radius * sin(j * stepSize);
			t.b.z = -radius * sin((i+1) * stepSize) * cos(j * stepSize);

			t.c.x = radius * cos((i) * stepSize) * cos((j+1) * stepSize);
			t.c.y = radius * sin((j+1) * stepSize);
			t.c.z = -radius * sin((i) * stepSize) * cos((j+1) * stepSize);

			//Sphere normals = radial vector (Thank god)
			t.a.nx = t.a.x;
			t.a.ny = t.a.y;
			t.a.nz = t.a.z;

			t.b.nx = t.b.x;
			t.b.ny = t.b.y;
			t.b.nz = t.b.z;

			t.c.nx = t.c.x;
			t.c.ny = t.c.y;
			t.c.nz = t.c.z;

			//Add triangle
			DynamicArray_Append(triangles, &t);

			t.a.x = radius * cos((i) * stepSize) * cos((j+1) * stepSize);
			t.a.y = radius * sin((j+1) * stepSize);
			t.a.z = -radius * sin((i) * stepSize) * cos((j+1) * stepSize);

			t.b.x = radius * cos((i+1) * stepSize) * cos(j * stepSize);
			t.b.y = radius * sin(j * stepSize);
			t.b.z = -radius * sin((i+1) * stepSize) * cos(j * stepSize);

			t.c.x = radius * cos((i + 1) * stepSize) * cos((j+1) * stepSize);
			t.c.y = radius * sin((j+1) * stepSize);
			t.c.z = -radius * sin((i + 1) * stepSize) * cos((j+1) * stepSize);

			//Sphere normals = radial vector (thank god)
			t.a.nx = t.a.x;
			t.a.ny = t.a.y;
			t.a.nz = t.a.z;

			t.b.nx = t.b.x;
			t.b.ny = t.b.y;
			t.b.nz = t.b.z;

			t.c.nx = t.c.x;
			t.c.ny = t.c.y;
			t.c.nz = t.c.z;

			//Add triangle
			DynamicArray_Append(triangles, &t);
		}
	}

	Mesh* sphere = Mesh_Allocate();
	Mesh_Initialize(sphere, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);

	//Free memory from dynamic array now that mesh has copy of triangles
	DynamicArray_Free(triangles);

	return sphere;
}

///
//Generates a torus mesh to specification
//Parameters:
//	radius: The radius from the center of modelSpace to the center of surrounding tube
//	tubeRadius: The radius of the surrounding tube
//	subdivisions: THe number of "sides" Of the torus
//Returns:
//	Pointer to newly allocated & initialized mesh containing a sphere
Mesh* Generator_GenerateTorusMesh(float radius, float tubeRadius, int subdivisions)
{
	struct Triangle t;
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	float stepSize =  (3.14159f * 2.0f)/subdivisions;

	//Set up vertex attributes which will not be changing
	t.a.tx = t.a.ty = t.b.tx = t.b.ty = t.c.tx = t.c.ty = 0.0f;	//All texture coordinates are 0

	//For computing normals
	Vector u, v, w;
	Vector_INIT_ON_STACK(u, 3);
	Vector_INIT_ON_STACK(v, 3);
	Vector_INIT_ON_STACK(w, 3);

	for(int i = 0; i < subdivisions; i++)
	{
		for(int j = 0; j < subdivisions; j++)
		{
			t.a.x = tubeRadius * cos(i * stepSize) * cos(j * stepSize) + radius * cos(i * stepSize);
			t.a.y = tubeRadius * sin(j * stepSize);
			t.a.z = -tubeRadius * sin(i * stepSize) * cos(j * stepSize) - radius * sin(i * stepSize);

			//i + 1
			t.b.x = tubeRadius * cos((i+1) * stepSize) * cos(j * stepSize) + radius * cos((i+1) * stepSize);
			t.b.y = tubeRadius * sin(j * stepSize);
			t.b.z = -tubeRadius * sin((i+1) * stepSize) * cos(j * stepSize) - radius * sin((i+1) * stepSize);

			//j + 1
			t.c.x = tubeRadius * cos(i * stepSize) * cos((j+1) * stepSize) + radius * cos(i * stepSize);
			t.c.y = tubeRadius * sin((j+1) * stepSize);
			t.c.z = -tubeRadius * sin(i * stepSize) * cos((j+1) * stepSize) - radius * sin(i * stepSize);

			//Compute normals

			//Get vectors in plane
			u.components[0] = t.b.x - t.a.x;
			u.components[1] = t.b.y - t.a.y;
			u.components[2] = t.b.z - t.a.z;

			v.components[0] = t.c.x - t.a.x;
			v.components[1] = t.c.y - t.a.y;
			v.components[2] = t.c.z - t.a.z;

			Vector_CrossProduct(&w, &u, &v);

			//Assign normals
			t.a.nx = t.b.nx = t.c.nx = w.components[0];
			t.a.ny = t.b.ny = t.c.ny = w.components[1];
			t.a.nz = t.b.nz = t.c.nz = w.components[2];


			//Add triangle
			DynamicArray_Append(triangles, &t);

			//j+1
			t.a.x = tubeRadius * cos(i * stepSize) * cos((j+1) * stepSize) + radius * cos(i * stepSize);
			t.a.y = tubeRadius * sin((j+1) * stepSize);
			t.a.z = -tubeRadius * sin(i * stepSize) * cos((j+1) * stepSize) - radius * sin(i * stepSize);

			//i+1
			t.b.x = tubeRadius * cos((i+1) * stepSize) * cos(j * stepSize) + radius * cos((i+1) * stepSize);
			t.b.y = tubeRadius * sin(j * stepSize);
			t.b.z = -tubeRadius * sin((i+1) * stepSize) * cos(j * stepSize) - radius * sin((i+1) * stepSize);
			//j+1 and i + 1
			t.c.x = tubeRadius * cos((i+1) * stepSize) * cos((j+1) * stepSize) + radius * cos((i+1) * stepSize);
			t.c.y = tubeRadius * sin((j+1) * stepSize);
			t.c.z = -tubeRadius * sin((i+1) * stepSize) * cos((j+1) * stepSize) - radius * sin((i+1) * stepSize);

			//Compute normals

			//Get vectors in plane
			u.components[0] = t.b.x - t.a.x;
			u.components[1] = t.b.y - t.a.y;
			u.components[2] = t.b.z - t.a.z;

			v.components[0] = t.c.x - t.a.x;
			v.components[1] = t.c.y - t.a.y;
			v.components[2] = t.c.z - t.a.z;

			Vector_CrossProduct(&w, &u, &v);

			//Assign normals
			t.a.nx = t.b.nx = t.c.nx = w.components[0];
			t.a.ny = t.b.ny = t.c.ny = w.components[1];
			t.a.nz = t.b.nz = t.c.nz = w.components[2];

			//Add triangle
			DynamicArray_Append(triangles, &t);
		}
	}

	Mesh* torus = Mesh_Allocate();
	Mesh_Initialize(torus, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);

	//Free memory from dynamic array now that mesh has copy of triangles
	DynamicArray_Free(triangles);

	return torus;
}

///
//Generates a grid mesh to specification
//Parameters:
//	width: The width of the grid from leftmost side to rightmost side
//	height: The height of the grid from topmost side to bottommost side
//	widthDivisions: The amount of subdivisions along the width of the grid
//	heightDivisions: The amount of subdivisions along the height of the grid
//	depthDivisions: the amount of subdivisons along the depth of the grid
Mesh* Generator_GenerateGridMesh(float width, float height, float depth, unsigned int widthDivisions, unsigned int heightDivisions, unsigned int depthDivisions)
{
	struct Triangle t;
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	float wStepSize = width / widthDivisions;	//Step size along width of grid
	float hStepSize = height / heightDivisions;	//Step size along height of grid
	float dStepSize = depth / depthDivisions;	//Step size along depth of grid

	float widthStart = -width/2.0f;
	float heightStart = -height/2.0f;
	float depthStart = -depth/2.0f;

	//Set up vertex attributes which will not be changing
	t.a.tx = t.a.ty = t.b.ty = t.b.ty = t.c.tx = t.c.ty = 0.0f;

	for(int k = 0; k < depthDivisions; k++)
	{
		for(int j = 0; j < heightDivisions; j++)
		{
			for(int i = 0; i < widthDivisions; i++)
			{
				//Square in XY plane of current depth level
				//Set normals pointing down Z axis or -Z axis depending on depth level
				t.a.nx = t.a.ny = t.b.nx = t.b.ny = t.c.nx = t.c.ny = 0.0f;
				if(k < depthDivisions / 2)
				{
					t.a.nz = t.b.nz = t.c.nz = -1.0f;
				}
				else
				{
					t.a.nz = t.b.nz = t.c.nz = 1.0f;
				}

				//Triangle 1
				t.a.x = widthStart + wStepSize * i;
				t.a.y = heightStart + hStepSize * j;
				t.a.z = depthStart + dStepSize * k;

				t.b.x = widthStart + wStepSize * (i+1);
				t.b.y = heightStart + hStepSize * j;
				t.b.z = depthStart + dStepSize * k;

				t.c.x = widthStart + wStepSize * i;
				t.c.y = heightStart + hStepSize * (j+1);
				t.c.z = depthStart + dStepSize * k;
				//Add triangle
				DynamicArray_Append(triangles, &t);

				//Triangle 2
				t.a.x = widthStart + wStepSize * i;
				t.a.y = heightStart + hStepSize * (j+1);
				t.a.z = depthStart + dStepSize * k;

				t.b.x = widthStart + wStepSize * (i+1);
				t.b.y = heightStart + hStepSize * j;
				t.b.z = depthStart + dStepSize * k;

				t.c.x = widthStart + wStepSize * (i+1);
				t.c.y = heightStart + hStepSize * (j+1);
				t.c.z = depthStart + dStepSize * k;
				//Add triangle
				DynamicArray_Append(triangles, &t);

				//Square in YZ plane at current width level
				//Set normals pointing down X axis or -X axis depending on width level
				t.a.nz = t.a.ny = t.b.nz = t.b.ny = t.c.nz = t.c.ny = 0.0f;
				if(i < widthDivisions / 2)
				{
					t.a.nx = t.b.nx = t.c.nx = -1.0f;
				}
				else
				{
					t.a.nx = t.b.nx = t.c.nx = 1.0f;
				}
				//Triangle 1
				t.a.x = widthStart + wStepSize * i;
				t.a.y = heightStart + hStepSize * j;
				t.a.z = depthStart + dStepSize * k;

				t.b.x = widthStart + wStepSize * i;
				t.b.y = heightStart + hStepSize * j;
				t.b.z = depthStart + dStepSize * (k+1);

				t.c.x = widthStart + wStepSize * i;
				t.c.y = heightStart + hStepSize * (j+1);
				t.c.z = depthStart + dStepSize * k;
				//Add triangle
				DynamicArray_Append(triangles, &t);

				//Triangle 2
				t.a.x = widthStart + wStepSize * i;
				t.a.y = heightStart + hStepSize * (j+1);
				t.a.z = depthStart + dStepSize * k;

				t.b.x = widthStart + wStepSize * i;
				t.b.y = heightStart + hStepSize * j;
				t.b.z = depthStart + dStepSize * (k+1);

				t.c.x = widthStart + wStepSize * i;
				t.c.y = heightStart + hStepSize * (j+1);
				t.c.z = depthStart + dStepSize * (k+1);
				//Add triangle
				DynamicArray_Append(triangles, &t);

				//Square in XZ plane at current height level
				//Set normals pointing down Y axis or -Y axis depending on height level
				t.a.nx = t.a.nz = t.b.nx = t.b.nz = t.c.nx = t.c.nz = 0.0f;
				if(j < heightDivisions / 2)
				{
					t.a.ny = t.b.ny = t.c.ny = -1.0f;
				}
				else
				{
					t.a.ny = t.b.ny = t.c.ny = 1.0f;
				}
				//Triangle 1
				t.a.x = widthStart + wStepSize * i;
				t.a.y = heightStart + hStepSize * j;
				t.a.z = depthStart + dStepSize * k;

				t.b.x = widthStart + wStepSize * i;
				t.b.y = heightStart + hStepSize * j;
				t.b.z = depthStart + dStepSize * (k+1);

				t.c.x = widthStart + wStepSize * (i+1);
				t.c.y = heightStart + hStepSize * j;
				t.c.z = depthStart + dStepSize * k;
				//Add triangle
				DynamicArray_Append(triangles, &t);

				//Triangle 2
				t.a.x = widthStart + wStepSize * (i+1);
				t.a.y = heightStart + hStepSize * j;
				t.a.z = depthStart + dStepSize * k;

				t.b.x = widthStart + wStepSize * i;
				t.b.y = heightStart + hStepSize * j;
				t.b.z = depthStart + dStepSize * (k+1);

				t.c.x = widthStart + wStepSize * (i+1);
				t.c.y = heightStart + hStepSize * j;
				t.c.z = depthStart + dStepSize * (k+1);
				//Add triangle
				DynamicArray_Append(triangles, &t);
			}

			//Create the final triangle in the YZ plane to finish the row
			//Set normals pointing down X
			t.a.nz = t.a.ny = t.b.nz = t.b.ny = t.c.nz = t.c.ny = 0.0f;
			t.a.nx = t.b.nx = t.c.nx = 1.0f;
			//Triangle 1
			t.a.x = widthStart + wStepSize * widthDivisions;
			t.a.y = heightStart + hStepSize * j;
			t.a.z = depthStart + dStepSize * k;

			t.b.x = widthStart + wStepSize * widthDivisions;
			t.b.y = heightStart + hStepSize * j;
			t.b.z = depthStart + dStepSize * (k+1);

			t.c.x = widthStart + wStepSize * widthDivisions;
			t.c.y = heightStart + hStepSize * (j+1);
			t.c.z = depthStart + dStepSize * k;
			//Add triangle
			DynamicArray_Append(triangles, &t);

			//Triangle 2
			t.a.x = widthStart + wStepSize * widthDivisions;
			t.a.y = heightStart + hStepSize * (j+1);
			t.a.z = depthStart + dStepSize * k;

			t.b.x = widthStart + wStepSize * widthDivisions;
			t.b.y = heightStart + hStepSize * j;
			t.b.z = depthStart + dStepSize * (k+1);

			t.c.x = widthStart + wStepSize * widthDivisions;
			t.c.y = heightStart + hStepSize * (j+1);
			t.c.z = depthStart + dStepSize * (k+1);
			//Add triangle
			DynamicArray_Append(triangles, &t);
		}

		//Create the row of triangles in the XZ plane to complete the top side
		for(int i = 0; i < widthDivisions; i++)
		{
			//Square in XZ plane at max height level
			//Set normals pointing down Y 
			t.a.nx = t.a.nz = t.b.nx = t.b.nz = t.c.nx = t.c.nz = 0.0f;
			t.a.ny = t.b.ny = t.c.ny = 1.0f;
			//Triangle 1
			t.a.x = widthStart + wStepSize * i;
			t.a.y = heightStart + hStepSize * heightDivisions;
			t.a.z = depthStart + dStepSize * k;

			t.b.x = widthStart + wStepSize * i;
			t.b.y = heightStart + hStepSize * heightDivisions;
			t.b.z = depthStart + dStepSize * (k+1);

			t.c.x = widthStart + wStepSize * (i+1);
			t.c.y = heightStart + hStepSize * heightDivisions;
			t.c.z = depthStart + dStepSize * k;
			//Add triangle
			DynamicArray_Append(triangles, &t);

			//Triangle 2
			t.a.x = widthStart + wStepSize * (i+1);
			t.a.y = heightStart + hStepSize * heightDivisions;
			t.a.z = depthStart + dStepSize * k;

			t.b.x = widthStart + wStepSize * i;
			t.b.y = heightStart + hStepSize * heightDivisions;
			t.b.z = depthStart + dStepSize * (k+1);

			t.c.x = widthStart + wStepSize * (i+1);
			t.c.y = heightStart + hStepSize * heightDivisions;
			t.c.z = depthStart + dStepSize * (k+1);
			//Add triangle
			DynamicArray_Append(triangles, &t);
		}
	}

	//Create final face of squares in the XY plane at max depth
	for(int j = 0; j < heightDivisions; j++)
	{
		for(int i = 0; i < widthDivisions; i++)
		{
			//Set normals pointing down Z axis 
			t.a.nx = t.a.ny = t.b.nx = t.b.ny = t.c.nx = t.c.ny = 0.0f;
			t.a.nz = t.b.nz = t.c.nz = 1.0f;
			//Triangle 1
			t.a.x = widthStart + wStepSize * i;
			t.a.y = heightStart + hStepSize * j;
			t.a.z = depthStart + dStepSize * depthDivisions;

			t.b.x = widthStart + wStepSize * (i+1);
			t.b.y = heightStart + hStepSize * j;
			t.b.z = depthStart + dStepSize * depthDivisions;

			t.c.x = widthStart + wStepSize * i;
			t.c.y = heightStart + hStepSize * (j+1);
			t.c.z = depthStart + dStepSize * depthDivisions;
			//Add triangle
			DynamicArray_Append(triangles, &t);

			//Triangle 2
			t.a.x = widthStart + wStepSize * i;
			t.a.y = heightStart + hStepSize * (j+1);
			t.a.z = depthStart + dStepSize * depthDivisions;

			t.b.x = widthStart + wStepSize * (i+1);
			t.b.y = heightStart + hStepSize * j;
			t.b.z = depthStart + dStepSize * depthDivisions;

			t.c.x = widthStart + wStepSize * (i+1);
			t.c.y = heightStart + hStepSize * (j+1);
			t.c.z = depthStart + dStepSize * depthDivisions;
			//Add triangle
			DynamicArray_Append(triangles, &t);
		}
	}

	Mesh* grid = Mesh_Allocate();
	Mesh_Initialize(grid, (struct Triangle*)triangles->data, triangles->size, GL_DYNAMIC_DRAW);

	//Free triangles now that mesh has a copy
	DynamicArray_Free(triangles);

	return grid;

}


///
//Generates a point grid mesh to specification
//Parameters:
//	width: The width of the grid from leftmost side to rightmost side
//	height: The height of the grid from topmost side to bottommost side
//	widthDivisions: The amount of subdivisions along the width of the grid
//	heightDivisions: The amount of subdivisions along the height of the grid
//	depthDivisions: the amount of subdivisons along the depth of the grid
Mesh* Generator_GeneratePointGridMesh(float width, float height, float depth, unsigned int widthDivisions, unsigned int heightDivisions, unsigned int depthDivisions)
{
	struct Triangle t;
	DynamicArray* triangles = DynamicArray_Allocate();
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	float wStepSize = width / widthDivisions;	//Step size along width of grid
	float hStepSize = height / heightDivisions;	//Step size along height of grid
	float dStepSize = depth / depthDivisions;	//Step size along depth of grid

	float widthStart = -width/2.0f;
	float heightStart = -height/2.0f;
	float depthStart = -depth/2.0f;

	//Set up vertex attributes which will not be changing
	t.a.tx = t.a.ty = t.b.ty = t.b.ty = t.c.tx = t.c.ty = 0.0f;
	t.a.nz = t.b.nz = t.c.nz = 1.0f;

	int triSize = 0;

	for(int k = 0; k < depthDivisions; k++)
	{
		for(int j = 0; j < heightDivisions; j++)
		{
			for(int i = 0; i < widthDivisions; i++)
			{
				//Get current vertex
				Vertex* memLoc = &(t.a);
				Vertex* current = (memLoc + triSize);
				current->x = widthStart + i * wStepSize;
				current->y = heightStart + j * hStepSize;
				current->z = depthStart + k * dStepSize;
				triSize++;

				if(triSize == 3)
				{
					DynamicArray_Append(triangles, &t);
					triSize = 0;
				}


			}
		}
	}

	while(triSize != 0 && triSize != 3)
	{
		//Get current vertex
		Vertex* memLoc = &(t.a);
		Vertex* prev = (Vertex*)(memLoc + (triSize - 1));
		Vertex* current = (memLoc + triSize);

		current->x = prev->x;
		current->y = prev->y;
		current->z = prev->z;

		triSize++;
	}
	DynamicArray_Append(triangles, &t);


	Mesh* grid = Mesh_Allocate();
	Mesh_Initialize(grid, (struct Triangle*)triangles->data, triangles->size, GL_DYNAMIC_DRAW);

	//Free triangles now that mesh has a copy
	DynamicArray_Free(triangles);

	return grid;
}
