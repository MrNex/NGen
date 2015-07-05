#include "Loader.h"
#include <stdio.h>
#include <stdlib.h>

#include "../Data/DynamicArray.h"

///
//Loads a text file into memory
//
//Parameters:
//	fPath: The filepath of the file to load
//
//Returns:
//	Null terminated character array containing contents of file
char* Loader_LoadTextFile(const char* fPath)
{
	char* fileContents = 0;

	//Open file in binary mode
	FILE* fileP;
	fileP = fopen(fPath, "rb");


	if (fileP != NULL)
	{
		//Calculate length
		int length;
		fseek(fileP, 0, SEEK_END);
		length = ftell(fileP);

		//Put file pointer back at start of file
		rewind(fileP);

		int size = (sizeof(char)* length) + 1;

		//Allocate space for entire file and null terminator in file contents
		fileContents = (char*)malloc(size);

		//Read file into array
		//fileInputStream.read(fileContents, length);
		fread(fileContents, sizeof(char), length, fileP);

		//Add null terminator
		fileContents[length] = '\0';



		fclose(fileP);
	}

	//Return file contents
	return fileContents;
}


///
//Loads and parses a .OBJ file constructing and returning
//the Mesh it defines
//
//TODO: Put in code for parsing materials and groups
//
//Parameters:
//	fPath: The filepath of the .obj file to load
//
//Returns:
//	A pointer to a newly allocated Mesh following the specifications of the given .OBJ file
Mesh* Loader_LoadOBJFile(const char* fPath)
{
	//Open file in read mode
	FILE* fp = fopen(fPath, "r");

	//MAke sure file is open
	if (fp == NULL)
	{
		printf("Error opening file %s.\n", fPath);
		return NULL;
	}

	//File opened correctly, begin parsing
	//Create struct to wrap 3 component attributes
	struct VertexAttr3
	{
		GLfloat x, y, z;
	};
	//Create struct to wrap 2 component attributes
	struct VertexAttr2
	{
		GLfloat x, y;
	};

	DynamicArray* vertices = DynamicArray_Allocate();		//Vertex coordinates
	DynamicArray* texCoords = DynamicArray_Allocate();		//Texture coordinates
	DynamicArray* normals = DynamicArray_Allocate();		//Normal Vectors
	DynamicArray* triangles = DynamicArray_Allocate();		//Complete Triangles

	DynamicArray_Initialize(vertices, sizeof(struct VertexAttr3));
	DynamicArray_Initialize(texCoords, sizeof(struct VertexAttr2));
	DynamicArray_Initialize(normals, sizeof(struct VertexAttr3));
	DynamicArray_Initialize(triangles, sizeof(struct Triangle));

	//Create a Cstring to hold each line of text
	char type[3];
	//char line[100];

	//Loop until we reach the end of the file
	while (!feof(fp))
	{
		//Get first (max 3) letters indicating information we are parsing
		//if(fscanf_s(fp, "%s", type, 3) == EOF) break;	//Break if we read past the end of the file (less than 3 characters were left)

		if(fscanf(fp, "%s", type) == EOF) break;

		//If we are reading Vertices, Texture Coordinates, or Normals
		if (type[0] == 'v')
		{
			//Normals
			if (type[1] == 'n')
			{
				struct VertexAttr3 vn;
				fscanf(fp, " %f %f %f", &vn.x, &vn.y, &vn.z);
				DynamicArray_Append(normals, &vn);
			}
			//Texture coordinates
			else if (type[1] == 't')
			{
				struct VertexAttr2 vt;
				fscanf(fp, " %f %f", &vt.x, &vt.y);
				DynamicArray_Append(texCoords, &vt);
			}
			//Vertices
			else
			{
				struct VertexAttr3 v;
				fscanf(fp, " %f %f %f", &v.x, &v.y, &v.z);
				DynamicArray_Append(vertices, &v);
			}
		}
		//Else if we are reading Face information
		else if (type[0] == 'f')
		{
			int indices[3];
			struct Triangle t;
			struct Vertex v;
			struct VertexAttr3* vAtt3;
			struct VertexAttr2* vAtt2;

			//For each vertex in the triangle
			for (int i = 0; i < 3; i++)
			{
				//Get the face info & the amount of info given
				int infoCount = fscanf(fp, " %d/%d/%d", indices, indices + 1, indices + 2);

				switch (infoCount)
				{
				case 3:		//Grabbing normals if present
					vAtt3 = (struct VertexAttr3*)DynamicArray_Index(normals, (indices[2] - 1));
					v.nx = vAtt3->x;
					v.ny = vAtt3->y;
					v.nz = vAtt3->z;
				case 2:		//Grabbing texture coords if present
					vAtt2 = (struct VertexAttr2*)DynamicArray_Index(texCoords, (indices[1] - 1));
					v.tx = vAtt2->x;
					v.ty = vAtt2->y;
				case 1:		//Grabbing vertex position
					vAtt3 = (struct VertexAttr3*)DynamicArray_Index(vertices, (indices[0] - 1));
					v.x = vAtt3->x;
					v.y = vAtt3->y;
					v.z = vAtt3->z;
				}

				//Store vertex in triangle t
				*(&(t.a) + i) = v;

			}
			//And add to triangles array
			DynamicArray_Append(triangles, &t);
		}
		//Else if we are reading a comment
		else if (type[0] == '#')
		{
			//REad until you get to a new line
			char c = '0';
			while (c != '\n' && c != '\r')
			{
				c = fgetc(fp);
			}
		}
		//Else if we don't know what we're reading
		else
		{
			//Read until the next line
			char c = '0';

			while (c != '\n' && c != '\r')
			{
				c = fgetc(fp);
			}
		}
	}//End of file reached
	
	fclose(fp);

	//File parsed, creating mesh
	
	Mesh* parsed = Mesh_Allocate();
	Mesh_Initialize(parsed, (struct Triangle*)triangles->data, triangles->size, GL_STATIC_DRAW);
	DynamicArray_Free(vertices);
	DynamicArray_Free(normals);
	DynamicArray_Free(texCoords);
	DynamicArray_Free(triangles);

	return parsed;
}

///
//Loads and parses a .BMP file constructing an image
//holding it's contents.
//
//WARNING: CURRENTLY ONLY READS 24 BIT BITMAPS!!!!
//
//Parameters:
//	fPath: The filepath of the .bmp file to load
//
//Returns:
//	A pointer to a newly allocated image containing the data from the BMP
struct Image* Loader_Load24BitBMPFile(const char* fPath)
{
	FILE* fp = fopen(fPath, "rb");

	//Make sure file is open
	if (fp == NULL)
	{
		printf("Unable to open %s\n", fPath);
		return NULL;
	}

	//Read the first two unsigned characters
	unsigned char headerInfo[2];
	fread(headerInfo, sizeof(unsigned char), 2, fp);

	//The first two characters in the file represent the file type.
	//Bitmaps should have the magic number 66,77 ('B','M')
	if (headerInfo[0] != 'B' || headerInfo[1] != 'M')
	{
		printf("File %s is not a bitmap file!", fPath);
	}

	//In bitmap files the locationof the start of the bitmap can be found in bytes 10-14
	fseek(fp, 10, 0);
	unsigned int bitmapOffset = 0;
	fread(&bitmapOffset, sizeof(unsigned int), 1, fp);

	unsigned int width;
	unsigned int height;

	//In bmp files the width and height of the image can be found after 4 more bytes
	//18 - 22 for width
	//22 - 26 for height
	fseek(fp, 18, 0);
	fread(&(width), sizeof(unsigned int), 1, fp);
	fread(&(height), sizeof(unsigned int), 1, fp);

	struct Image* img = Image_Allocate();
	Image_Initialize(img, width, height);

	//Seek to beginning of bitmap array
	fseek(fp, bitmapOffset, 0);
	for (unsigned int i = 0; i < img->height; i++)
	{
		for (unsigned int j = 0; j < img->width; j++)
		{
			//Doing it this way swapped my R and B values
			//When I figure out why, change back. I think openGL trnsposes everything when translating information to shader
			//Read the RGB values and store in bitmap
			//fread(img->bitmap + (((i * img->width) + j) * 4), sizeof(unsigned char), 3, fp);

			//So instead
			//Read one at a time and store them in img->bitmap backwards
			unsigned char r, g, b;
			fread(&r, sizeof(unsigned char), 1, fp);
			fread(&g, sizeof(unsigned char), 1, fp);
			fread(&b, sizeof(unsigned char), 1, fp);

			*(img->bitmap + (((i * img->width) + j) * 4) + 0) = b;
			*(img->bitmap + (((i * img->width) + j) * 4) + 1) = g;
			*(img->bitmap + (((i * img->width) + j) * 4) + 2) = r;



			//After the RGB values, store 255 as the alpha value for that pixel.
			*(img->bitmap + (((i * img->width) + j) * 4) + 3) = (unsigned char)255;
			

		}

		//Read padding at the end of each row
		unsigned int padding = 0 - (img->width * 3) % 4;
		padding *= -1;
		if (padding > 0)
		{
			unsigned char* paddingContents = (unsigned char *)malloc(sizeof(unsigned char)* padding + 1);
			fread(paddingContents, sizeof(unsigned char), padding, fp);
			paddingContents[padding] = '\0';
			printf("\nPadding size/contents: %d/%s", padding, paddingContents);
			free(paddingContents);
		}

	}

	//Close the file
	fclose(fp);

	//REturn the image
	return img;
}
