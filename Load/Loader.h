#ifndef LOADER_H
#define LOADER_H


#include <stdio.h>
#include "../Render/Mesh.h"
#include "../Render/Image.h"

///
//Loads a text file into memory
//
//Parameters:
//	fPath: The filepath of the file to load
//
//Returns:
//	Null terminated character array containing contents of file
char* Loader_LoadTextFile(const char* fPath);

///
//Loads and parses a .OBJ file constructing and returning
//the Mesh it defines
//
//Parameters:
//	fPath: The filepath of the .obj file to load
//
//Returns:
//	A pointer to a newly allocated Mesh following the specifications of the given .OBJ file
Mesh* Loader_LoadOBJFile(const char* fPath);

///
//Loads and parses a .BMP file constructing an image
//holding it's contents.
//
//Parameters:
//	fPath: The filepath of the .bmp file to load
//
//Returns:
//	A pointer to a newly allocated image containing the data from the BMP
struct Image* Loader_Load24BitBMPFile(const char* fPath);

#endif
