#include "game.h"

#define N_OBSTACLES 13

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->lives = rand()%3+1;
	int i;
	for(i = 0; i < 3; i++)
		obstacle->bitmaps[i] = loadBitmap(fullPath("obstacle0.bmp"),x,y);
	return obstacle;
}

void update_obstacle(Obstacle* obstacle){
	int i;
	for(i = 0; i < 3; i++){
		if(obstacle->bitmaps[i]->y + 55 < vg_get_v_res())
			obstacle->bitmaps[i]->y += 1;
	}
}

void draw_obstacle(Obstacle* obstacle, char* buffer){
	drawBitmap(obstacle->bitmaps[obstacle->lives],buffer,ALIGN_LEFT);
}

Game* create_game(){
	Game* game = (Game*)(malloc(sizeof(Game)));
	game->mouse = create_mouse();
	game->background = loadBitmap(fullPath("game_background.bmp"),0,0);
	game->player = loadBitmap(fullPath("buzz.bmp"),50,530);
	game->secondary_buffer = (char*)(malloc(vg_get_window_size()));
	game->obstacles = (Obstacle**)(malloc(N_OBSTACLES*sizeof(Obstacle*)));
	int i;
	for(i = 0; i < N_OBSTACLES; i++){
		int empty = rand() % 2; //Determine if empty space or enemy's there
		if(!empty)
			game->obstacles[i] = create_obstacle(50*(i+1),50);
		else
			game->obstacles[i] = NULL;
	}
	game->state = GAME_RUNNING;
	return game;
}

void update_game(Game* game){
	//Update objects' positions
	//update_mouse(game->mouse);
	int i;
	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			update_obstacle(game->obstacles[i]);
	}
	/*if(game->mouse->packet[0] & BIT(4)) //Negative x delta
		game->player->x -= ABS_VALUE(TWOSCOMPLEMENT(game->mouse->packet[1]));
	else //Positive x delta
		game->player->x += game->mouse->packet[1];
	*/
	//Prepare next frame
	drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);
	drawBitmap(game->player,game->secondary_buffer,ALIGN_LEFT);
	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			draw_obstacle(game->obstacles[i],game->secondary_buffer);
	}
}

void draw_game(Game* game){
	memcpy(vg_get_video_mem(),game->secondary_buffer,vg_get_window_size());
}

void delete_game(Game* game){
	delete_mouse(game->mouse);
	deleteBitmap(game->background);
	deleteBitmap(game->player);
	int i;
	for(i = 0; i < 5; i++){
		if(game->obstacles[i] != NULL)
			free(game->obstacles[i]);
	}
	free(game->secondary_buffer);
	free(game);
}
