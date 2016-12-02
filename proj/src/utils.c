#include <string.h>
#include "stdio.h"
#include "utils.h"


int file_exists(const char* filename) {
	FILE* file = fopen(filename, "r");

	if(file == NULL)
		return -1;
	return 0;
}

const char* fullPath(const char* filename){
	char* result = (char*)malloc(256);
	//strcpy(result,"/home/lcom/lcom1617-t4g11/proj/images/"); //Damas' home version of the path
	//strcpy(result,"/home/lcom/feup2/lcom1617-t4g11/proj/images"); //Antonio's home version of the path
	strcpy(result,"/home/lcom/svn/lcom1617-t4g11/proj/images/"); //Damas' FEUP version of the path
	strcat(result,filename);
	return result;
}

int rgb(unsigned char r, unsigned char g, unsigned char b) {

	if(r > 255 || g > 255 || b > 255)
		return -1;

	int red = r*31/255;
	int green = g*63/255;
	int blue = b*31/255;

	return (red << 11 | green << 5| blue);
}

