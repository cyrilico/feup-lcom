#include "utils.h"

#define NUMBER_WIDTH 16

const char* fullPath(const char* filename){
	char* result = (char*)malloc(256);
	//strcpy(result,"/home/lcom/lcom1617-t4g11/proj/images/"); //Damas' home version of the path
	//strcpy(result,"/home/lcom/feup2/lcom1617-t4g11/proj/images/"); //Antonio's feup version of the path
	//strcpy(result,"/home/lcom/casa2/lcom1617-t4g11/proj/images/"); //Antonio's home version of the path
	strcpy(result,"/home/lcom/svn/lcom1617-t4g11/proj/images/"); //Damas' FEUP version of the path
	strcat(result,filename);
	return result;
}

void draw_number(int number, int x, int y, char* buffer){
	int msd = number/10; //msd = most significant digit
	int lsd = number%10; //lsd = least significant digit
	char msd_file[10];
	char lsd_file[10];
	sprintf(msd_file,"%d.bmp",msd);
	sprintf(lsd_file,"%d.bmp",lsd);
	/*printf("Cheguei aqui e os nomes dos ficheiros são %s e %s\n", msd_file, lsd_file);*/
	Bitmap* msd_bitmap = loadBitmap(fullPath(msd_file),x,y);
	Bitmap* lsd_bitmap = loadBitmap(fullPath(lsd_file),x+NUMBER_WIDTH,y);
	drawBitmap(msd_bitmap,buffer,ALIGN_LEFT);
	drawBitmap(lsd_bitmap,buffer,ALIGN_LEFT);
	deleteBitmap(msd_bitmap);
	deleteBitmap(lsd_bitmap);
}

int start_graphic_mode(){
	return (vg_init(0x114) != NULL ? 0 : -1);
}

int exit_graphic_mode(){
	return vg_exit();
}
