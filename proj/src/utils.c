#include "utils.h"

int file_exists(const char* filename) {
	FILE* file = fopen(filename, 'r');

	if(file == NULL)
		return -1;
	return 0;
}

int rgb(unsigned char r, unsigned char g, unsigned char b) {

	if(r > 255 || g > 255 || b > 255)
		return -1;

	int red = r*31/255;
	int green = g*63/255;
	int blue = b*31/255;

	return (red << 11 | green << 5| blue);
}

