#ifndef IMAGE_H
#define IMAGE_H

struct Image
{
	unsigned int width;
	unsigned int height;
	unsigned char* bitmap;
};

///
//Allocates memory to hold a new image
//
//Returns:
//	Returns a pointer to a newly allocated image
struct Image* Image_Allocate(void);

///
//Initializes an image
//
//PArameters:
//	img: The image to initialize
//	width: The width of the image
//	height: The height of the image
void Image_Initialize(struct Image* img, unsigned int width, unsigned int height);

///
//Frees memory being used by an image
//
//Parameters:
//	img: The image to free
void Image_Free(struct Image* img);

#endif
