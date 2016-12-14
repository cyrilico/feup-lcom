#include "game.h"

#define N_OBSTACLES 13

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->lives = rand()%3+1;
	int i;
	for(i = 0; i < 3; i++)
		obstacle->bitmaps[i] = loadBitmap(fullPath("villain.bmp"),x,y);
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

void delete_obstacle(Obstacle* obstacle){
	int i;
	for(i = 0; i < 3; i++)
		deleteBitmap(obstacle->bitmaps[i]);
	free(obstacle);
}

Player* create_player(){
	Player* player = (Player*)(malloc(sizeof(Player)));
	player->alive = 1;
	int player_start_x = rand()%(vg_get_h_res()/2) + LEFT_LIMIT; //Random starting position
	player->bitmap = loadBitmap(fullPath("buzz.bmp"), player_start_x, PLAYER_START_Y);
	/*TO DO: initialization of remaining structures (when they're created in Player 'class' */
	return player;
}

void update_player(Player* player, Mouse* mouse){
	if(mouse->packet[0] & BIT(4)){ //Negative x delta
		player->bitmap->x -= ABS_VALUE(TWOSCOMPLEMENT(mouse->packet[1]));
		if(player->bitmap->x < LEFT_LIMIT)
			player->bitmap->x = LEFT_LIMIT;
	}
	else{ //Positive x delta
		player->bitmap->x += mouse->packet[1];
		if(player->bitmap->x + player->bitmap->bitmapInfoHeader.width > RIGHT_LIMIT)
			player->bitmap->x = RIGHT_LIMIT - player->bitmap->bitmapInfoHeader.width;
	}
}

void draw_player(Player* player, char* buffer){
	drawBitmap(player->bitmap,buffer,ALIGN_LEFT);
}

void delete_player(Player* player){
	deleteBitmap(player->bitmap);
	free(player);
}

Game* create_game(){
	Game* game = (Game*)(malloc(sizeof(Game)));
	game->mouse = create_mouse();
	game->keyboard = create_keyboard();
	game->background = loadBitmap(fullPath("game_background.bmp"),0,0);
	game->player = create_player();
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
	//update_player(game->player,game->mouse);

	//Prepare next frame
	drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);
	draw_player(game->player,game->secondary_buffer);
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
	delete_keyboard(game->keyboard);
	deleteBitmap(game->background);
	delete_player(game->player);
	int i;
	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			delete_obstacle(game->obstacles[i]);
	}
	free(game->secondary_buffer);
	free(game);
}
