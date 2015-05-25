#include "Image.h"

#include <stdlib.h>

///
//Allocates memory to hold a new image
//
//Returns:
//	Returns a pointer to a newly allocated image
struct Image* Image_Allocate(void)
{
	struct Image* img = (struct Image*)malloc(sizeof(struct Image));
	return img;
}

///
//Initializes an image
//
//PArameters:
//	img: The image to initialize
//	width: The width of the image
//	height: The height of the image
void Image_Initialize(struct Image* img, unsigned int width, unsigned int height)
{
	img->width = width;
	img->height = height;
	img->bitmap = (unsigned char *)calloc(sizeof(unsigned char), img->width * img->height * 4);
}

///
//Frees memory being used by an image
//
//Parameters:
//	img: The image to free
void Image_Free(struct Image* img)
{
	free(img->bitmap);
	free(img);
}
