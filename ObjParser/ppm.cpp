// PPM-reading function, adapted from Angel
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "cs432.h"
#include <stdbool.h>
#include "ppm.h"

// reads an image from a PPM file
//   - filename: the name of the file
//   - target: pointer first element of 3D array of float which contains
//             the pixels in RGB representation
//   - x and y: in the target array for the lower-left pixel
//   - width: the width of the target-array
//   - height: the height of the target-array
bool readPpmImage(char *filename, GLfloat *target, int x, int y,
				  int width, int height) {

	// open the file
	FILE* fd;
	fopen_s(&fd, filename, "r");
	if (fd == NULL) return false;

	char xx, yy, dummy;
	fscanf(fd, "%c", &xx);
	fscanf(fd, "%c", &yy);
	fscanf(fd, "%c", &dummy);
	if (xx != 'P'|| yy != '3'){
		return false;
	}
	
	// skip over any comment-lines
#if 0
	char c;
	fscanf_s(fd, "%c",&c);
	while(c == '#') {
		fscanf_s(fd, "%[^\n] ", buffer);
		fscanf_s(fd, "%c",&c);
	}
	ungetc(c,fd); // push back non-# character
#else
	char c;
	for (;;) {
		fscanf(fd, "%c", &c);
		if (c != '#') {
			ungetc(c, fd);
			break;
		}
		while (c != '\n') {
			fscanf(fd, "%c", &c);
		}
	}
#endif

	// read the image width, height and full-intensity value
	int imageWidth, imageHeight, maxValue;
	fscanf(fd, " %d %d %d", &imageWidth, &imageHeight, &maxValue);

	// iterate over each pixel, putting into the proper place in the array
	for(int yy = imageHeight-1; yy >= 0; yy--) {
		// (we iterate over rows in reverse because top row is the last one
		// in the array)
		for (int xx = 0; xx < imageWidth; xx++) {
			// read the pixel-values
			int red, green, blue;
			fscanf(fd,"%d %d %d", &red, &green, &blue);

			// translate to proper location in target-array
			int yyModified = yy+y;
			int xxModified = xx+x;

			// if we're not out of bounds in targer-array, store pixel-values,
			// scaled by the full-intensity value
			if (yyModified < height && xxModified < width &&
				yyModified >= 0 && xxModified >= 0) {
				// we need to do the address-arithmetic ourselves to simulate a
				// 3D array of size heightXwidth
				target[3*(yyModified*width+xxModified)+0] = (GLfloat)red/maxValue;
				target[3*(yyModified*width+xxModified)+1] = (GLfloat)green/maxValue;
				target[3*(yyModified*width+xxModified)+2] = (GLfloat)blue/maxValue;
			}
		}
	}

	// return success status
	return true;
}

