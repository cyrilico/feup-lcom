#include "game.h"

Obstacle* create_obstacle(int x, int y, int width, int height){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->x = x;
	obstacle->y = y;
	obstacle->width = width;
	obstacle->height = height;
	obstacle->colors[0] = rgb(255,255,255);
	obstacle->colors[1] = rgb(255,0,0);
	obstacle->colors[2] = rgb(0,255,0);
	obstacle->colors[3] = rgb(0,0,255);
	obstacle->lives = 0;
	obstacle->color = obstacle->colors[obstacle->lives];
	return obstacle;
}

void update_obstacle(Obstacle* obstacle){
	if(obstacle->y+obstacle->height < vg_get_v_res()){
		obstacle->x += 1;
		obstacle->lives += 1;
		obstacle->color = obstacle->colors[obstacle->lives%4];
	}
}

void draw_obstacle(Obstacle* obstacle, char* buffer){
	/*
	int xi = obstacle->x;
	int xf = xi + obstacle->width;
	int yi = obstacle->y;
	int yf = yi + obstacle->height;
	while(yi < yf){
		while(xi < xf)
			vg_fill_pixel(xi++,yi,(unsigned long)obstacle->color,buffer);
		xi = obstacle->x;
		yi++;
	}

	 */
	vg_fill_screen(obstacle->x, obstacle->y, obstacle->width, obstacle->height, obstacle->color);
}

Game* create_game(){
	Game* game = (Game*)(malloc(sizeof(Game)));
	game->obstacle = create_obstacle(50,50,60,50);
	game->mouse = create_mouse();
	game->background = loadBitmap(fullPath("game_background.bmp"),0,0);
	game->double_buffer = (char*)(malloc(vg_get_window_size()));
	game->state = GNOT_DONE;
	return game;
}

void update_game(Game* game){
	update_mouse(game->mouse);
	update_obstacle(game->obstacle);
}

void draw_game(Game* game){
	drawBitmap(game->background,game->double_buffer,ALIGN_LEFT);
	draw_mouse(game->mouse,game->double_buffer);
	draw_obstacle(game->obstacle,game->double_buffer);
	memcpy(vg_get_video_mem(),game->double_buffer,vg_get_window_size());
}
