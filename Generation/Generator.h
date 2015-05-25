#include "../Render/Mesh.h"
#include "../Data/DynamicArray.h"

///
//Generates a cube mesh with specified width/length/height
//
//Parameters:
//	size: The sidelength of the cube
//
//Returns:
//	Pointer to newly allocated & initialized mesh containing a cube
Mesh* Generator_GenerateCubeMesh(float size);

///
//Generates a cylinder mesh to specification
//
//Parameters:
//	radius: The radius of the base of the cylinder
//	height:The height of the cylinder
//	subdivisions: Number of sides of cylinder ( >= 3)
//Returns:
//	Pointer to newly allocated & initialized mesh containing a cylinder
Mesh* Generator_GenerateCylinderMesh(float radius, float height, int subdivisions);

///
//Generates a cone mesh to specification
//Parameters:
//	radius: The radius of the base of the cone
//	height: The height of the cone
//	subdivisions: The number of sides of the cone
//Returns:
//	Pointer to newly allocated & initialized mesh containing a cone
Mesh* Generator_GenerateConeMesh(float radius, float height, int subdivisions);

///
//Generates a tube mesh to specification
//Parameters:
//	outerRadius: The radius of the external part of tube
//	inner radius: The radius of the inner part of tube
//	height: The height of the tube
//	subdivisions: The number of sides of the tube
//Returns:
//	Pointer to newly allocated & initialized mesh containing a tube
Mesh* Generator_GenerateTubeMesh(float outerRadius, float innerRadius, float height, int subdivisions);

///
//Generates a sphere mesh to specification
//Parameters:
//	radius: The radius of the sphere
//	subdivisions: The number of "sides" of the sphere
//Returns:
//	Pointer to newly allocated & initialized mesh containing a sphere
Mesh* Generator_GenerateSphereMesh(float radius, int subdivisions);

///
//Generates a torus mesh to specification
//Parameters:
//	radius: The radius from the center of modelSpace to the center of surrounding tube
//	tubeRadius: The radius of the surrounding tube
//	subdivisions: THe number of "sides" Of the torus
//Returns:
//	Pointer to newly allocated & initialized mesh containing a sphere
Mesh* Generator_GenerateTorusMesh(float radius, float tubeRadius, int subdivisions);

///
//Generates a grid mesh to specification
//Parameters:
//	width: The width of the grid from leftmost side to rightmost side
//	height: The height of the grid from topmost side to bottommost side
//	widthDivisions: The amount of subdivisions along the width of the grid
//	heightDivisions: The amount of subdivisions along the height of the grid
//	depthDivisions: the amount of subdivisons along the depth of the grid
Mesh* Generator_GenerateGridMesh(float width, float height, float depth, unsigned int widthDivisions, unsigned int heightDivisions, unsigned int depthDivisions);


///
//Generates a point grid mesh to specification
//Parameters:
//	width: The width of the grid from leftmost side to rightmost side
//	height: The height of the grid from topmost side to bottommost side
//	widthDivisions: The amount of subdivisions along the width of the grid
//	heightDivisions: The amount of subdivisions along the height of the grid
//	depthDivisions: the amount of subdivisons along the depth of the grid
Mesh* Generator_GeneratePointGridMesh(float width, float height, float depth, unsigned int widthDivisions, unsigned int heightDivisions, unsigned int depthDivisions);
