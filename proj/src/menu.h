#pragma once

typedef struct{
	int xi;
	int yi;
	int width;
	int height;
}Button;

Button* create_button(int x, int y, int width, int height);
int mouse_on_button(Button* button, Mouse* mouse);
void delete_button(Button* button);

/*
PLAY (300,280)
EXIT - starts at (300,345)
rectangles have (width,height) = (145,50)
*/
