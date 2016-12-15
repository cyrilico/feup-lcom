#include "game.h"

#define N_OBSTACLES 10
#define BLACK 0
#define N_BULLETS 5
#define BULLET_HEIGHT 12
#define BULLET_OFFSET 37

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->lives = rand()%3+1;
	int i;
	for(i = 0; i < 3; i++)
		obstacle->bitmaps[i] = loadBitmap(fullPath("obstacle0.bmp"),x,y);
	return obstacle;
}

int update_obstacle(Obstacle* obstacle){
	if(obstacle_off_screen(obstacle))
		return 1;
	else{
		int i;
		for(i = 0; i < 3; i++)
			obstacle->bitmaps[i]->y += 1;

		return 0;
	}
}

void draw_obstacle(Obstacle* obstacle, char* buffer){
	drawBitmap(obstacle->bitmaps[obstacle->lives],buffer,ALIGN_LEFT);
}

int obstacle_off_screen(Obstacle* obstacle){
	if(obstacle->bitmaps[0]->y + 1 == vg_get_v_res())
		return 1;
	else
		return 0;
}

void delete_obstacle(Obstacle* obstacle){
	int i;
	for(i = 0; i < 3; i++)
		deleteBitmap(obstacle->bitmaps[i]);
	free(obstacle);
}

Player* create_player(){
	Player* player = (Player*)(malloc(sizeof(Player)));
	int player_start_x = rand()%(vg_get_h_res()/2) + LEFT_LIMIT; //Random starting position
	player->bitmap = loadBitmap(fullPath("buzz.bmp"), player_start_x, PLAYER_START_Y);
	/*TO DO: initialization of remaining structures (when they're created in Player 'class' */
	/* \/ Save technique for later: fast load of all number bitmaps \/ */
	/*for(i = 0; i < 10; i++){
		char temp[10];
		sprintf(temp,"%d.bmp",i);
		player->numbers[i] = loadBitmap(fullPath(temp),25,300);
	}
	*/
	player->bullets = (Bitmap**)(malloc(N_BULLETS*sizeof(Bitmap*)));
	int i;
	for(i = 0; i < N_BULLETS; i++)
		player->bullets[i] = loadBitmap(fullPath("bullet.bmp"),player->bitmap->x+BULLET_OFFSET,player->bitmap->y-BULLET_HEIGHT);
	return player;
}

void update_player_mouse(Player* player, Mouse* mouse){
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
	int i;
	for(i = 0; i < N_BULLETS; i++){
		if(player->bullets[i] != NULL){
			player->bullets[i]->x = player->bitmap->x+BULLET_OFFSET;
			player->bullets[i]->y = player->bitmap->y-BULLET_HEIGHT;
		}
	}
}

void update_player_collision(Player* player, char* buffer){
	unsigned long firstPixel = *(buffer + ((player->bitmap->y-1)*vg_get_h_res() + player->bitmap->x)*vg_get_bits_per_pixel()/8);
	unsigned long lastPixel =  *(buffer + ((player->bitmap->y-1)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bits_per_pixel()/8);

	if(firstPixel != BLACK || lastPixel != BLACK)
		player->bitmap->y++;
	else if(player->bitmap->y != PLAYER_START_Y)
		player->bitmap->y--;

	int i;
	for(i = 0; i < N_BULLETS; i++){
		if(player->bullets[i] != NULL){
			player->bullets[i]->x = player->bitmap->x+BULLET_OFFSET;
			player->bullets[i]->y = player->bitmap->y-BULLET_HEIGHT;
		}
	}
}

void draw_player(Player* player, char* buffer){
	drawBitmap(player->bitmap,buffer,ALIGN_LEFT);
}

void delete_player(Player* player){
	deleteBitmap(player->bitmap);
	int i;
	for(i = 0; i < N_BULLETS; i++){
		if(player->bullets[i] != NULL)
			deleteBitmap(player->bullets[i]);
	}
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
			game->obstacles[i] = create_obstacle(OBSTACLE_WIDTH*(i+1),OBSTACLE_HEIGHT);
		else
			game->obstacles[i] = NULL;
	}
	game->bullets = (Bitmap**)(malloc(N_BULLETS*sizeof(Bitmap*)));
	for(i = 0; i < N_BULLETS; i++)
		game->bullets[i] = NULL;
	game->state = GAME_RUNNING;
	return game;
}

void remove_bullet_shot(Game* game){
	int i, j;
	int found = 0;
	for(i = N_BULLETS-1; i >= 0; i--){
		if(found)
			break;
		if(game->player->bullets[i] != NULL){
			found = 1;
			for(j = 0; j < N_BULLETS; j++){
				if(game->bullets[j] == NULL){
					game->bullets[j] = game->player->bullets[i];
					game->player->bullets[i] = NULL;
					break;
				}
			}
		}
	}
}

void update_game(Game* game){
	//Update objects' positions
	//update_mouse(game->mouse);
	//update_player_mouse(game->player,game->mouse);
	int i;
	int off_screen;
	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			off_screen = update_obstacle(game->obstacles[i]);
	}

	/* TO DO: Add more layering to this (function to delete array of obstacles, function to generate array of obstacles) */
	//If current obstacle line if off screen, generate a new one
	if(off_screen){
		for(i = 0; i < N_OBSTACLES; i++){
			if(game->obstacles[i] != NULL)
				delete_obstacle(game->obstacles[i]);
		}

		for(i = 0; i < N_OBSTACLES; i++){
			int empty = rand() % 2; //Determine if empty space or enemy's there
			if(!empty)
				game->obstacles[i] = create_obstacle(OBSTACLE_WIDTH*(i+1),OBSTACLE_HEIGHT);
			else
				game->obstacles[i] = NULL;
		}
	}

	//Update shot bullets' positions
	for(i = 0; i < N_BULLETS; i++){
		if(game->bullets[i] != NULL){
			game->bullets[i]->y--;
			if(game->bullets[i] == 0)
				game->bullets[i] = NULL;
		}
	}

	update_player_collision(game->player,game->secondary_buffer);

	//Prepare next frame
	drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);

	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			draw_obstacle(game->obstacles[i],game->secondary_buffer);
	}
	draw_player(game->player,game->secondary_buffer);

	for(i = 0; i < N_BULLETS; i++){
		if(game->bullets[i] != NULL)
			drawBitmap(game->bullets[i],game->secondary_buffer,ALIGN_LEFT);
	}

	if(game->player->bitmap->y == vg_get_v_res() - PLAYER_DEATH_TOLERANCE)
		game->state = GAME_OVER;
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
	for(i = 0; i < N_BULLETS; i++){
		if(game->bullets[i] != NULL)
			deleteBitmap(game->bullets[i]);
	}
	free(game->secondary_buffer);
	free(game);
}
