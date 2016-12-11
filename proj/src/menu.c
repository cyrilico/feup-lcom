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

Menu* create_menu(){
	Menu* menu = (Menu*)(malloc(sizeof(Menu)));
	menu->play_button = create_button(PLAY_BUTTON_X,PLAY_BUTTON_Y,BUTTON_WIDTH,BUTTON_HEIGHT);
	menu->exit_button = create_button(EXIT_BUTTON_X,EXIT_BUTTON_Y,BUTTON_WIDTH,BUTTON_HEIGHT);
	menu->mouse = create_mouse();
	menu->background = loadBitmap(fullPath("menu_background.bmp"),0,0);
	menu->secondary_buffer = (char*)(malloc(vg_get_window_size()));

	menu->state = NOT_DONE;
	return menu;
}

void draw_menu(Menu* menu){
	memcpy(vg_get_video_mem(),menu->secondary_buffer,vg_get_window_size());
}

void update_menu(Menu* menu){
	//Update mouse with latest packet and menu state according to it
	update_mouse(menu->mouse);
	int lbreleased = menu->mouse->left_button_was_released;
	int mouseInPlayButton = mouse_on_button(menu->play_button,menu->mouse);
	int mouseInExitButton = mouse_on_button(menu->exit_button,menu->mouse);
	if(lbreleased && mouseInPlayButton)
		menu->state = PLAY_CHOSEN;
	else if(lbreleased && mouseInExitButton)
		menu->state = EXIT_CHOSEN;

	//Prepare next frame
	drawBitmap(menu->background,menu->secondary_buffer,ALIGN_LEFT);
	draw_mouse(menu->mouse,menu->secondary_buffer);
}

void delete_menu(Menu* menu){
	delete_mouse(menu->mouse);
	delete_button(menu->play_button);
	delete_button(menu->exit_button);
	free(menu->secondary_buffer);
	free(menu);
}
