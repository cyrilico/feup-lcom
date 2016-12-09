#include "game.h"

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->colors[0] = rgb(255,255,255);
	obstacle->colors[1] = rgb(255,0,0);
	obstacle->colors[2] = rgb(0,255,0);
	obstacle->colors[3] = rgb(0,0,255);
	obstacle->lives = 0;
	obstacle->color = rgb(255,255,255);
	obstacle->obs = loadBitmap(fullPath("obstacle0.bmp"),50,50);
	return obstacle;
}

void update_obstacle(Obstacle* obstacle){
	if(obstacle->obs->y + 55 < vg_get_v_res()){
		obstacle->obs->y += 1;
		obstacle->lives += 1;
	}
}

void draw_obstacle(Obstacle* obstacle, char* buffer){
	drawBitmap(obstacle->obs,buffer,ALIGN_LEFT);
}

Game* create_game(){
	Game* game = (Game*)(malloc(sizeof(Game)));
	game->obstacle = create_obstacle(50,50);
	game->mouse = create_mouse();
	game->background = loadBitmap(fullPath("game_background.bmp"),0,0);
	game->player = loadBitmap(fullPath("buzzie.bmp"),50,500);
	game->double_buffer = (char*)(malloc(vg_get_window_size()));
	game->state = GNOT_DONE;
	return game;
}

void update_game(Game* game){
	update_mouse(game->mouse);
	update_obstacle(game->obstacle);
	if(game->mouse->packet[0] & BIT(4)) //Negative x delta
		game->player->x -= ABS_VALUE(TWOSCOMPLEMENT(game->mouse->packet[1]));
	else //Positive x delta
		game->player->x += game->mouse->packet[1];
}

void draw_game(Game* game){
	drawBitmap(game->background,game->double_buffer,ALIGN_LEFT);
	draw_mouse(game->mouse,game->double_buffer);
	drawBitmap(game->player,game->double_buffer,ALIGN_LEFT);
	draw_obstacle(game->obstacle,game->double_buffer);
	memcpy(vg_get_video_mem(),game->double_buffer,vg_get_window_size());
}
