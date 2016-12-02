#include "menu.h"

Button* create_button(int x, int y, int width, int height){
	Button* button = (Button*)(malloc(sizeof(Button)));
	button->xi = x;
	button->yi = y;
	button->width = width;
	button->height = height;
	return button;
}
int mouse_on_button(Button* button, Mouse* mouse){
	int mouse_x = mouse->crosshair->x;
	int mouse_y = mouse->crosshair->y;
	if(mouse_x >= button->xi &&
	   mouse_x <= (button->xi+button->width) &&
	   mouse_y >= button->yi &&
	   mouse_y <= (button->yi+button->height))
		return 1;
	else return 0;
}

void delete_button(Button* button){
	free(button);
}
