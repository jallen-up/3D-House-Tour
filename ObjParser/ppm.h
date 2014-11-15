#include <stdbool.h>
extern bool readPpmImage(
	char *filename, // name of the file
	GLfloat *target, // pointer first element of 3D array of float which contains
					 // the pixels in RGB representation
	int x, // the x-location in the target array for the lower-left pixel
	int y, // the location in the target array for the lower-left pixel
	int width, // the width of the target-array
	int height); // the height of the target-array
