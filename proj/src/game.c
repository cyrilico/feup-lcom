#include "game.h"

Bullet* create_bullet(int x, int y){
	Bullet* bullet = (Bullet*)(malloc(sizeof(Bullet)));
	bullet->bitmap = loadBitmap(fullPath("bullet.bmp"), x, y);
	return bullet;
}

int update_bullet(Bullet* bullet){
	bullet->bitmap->y -= BULLET_SPEED;
	if(bullet->bitmap->y <= 0)
		return 1;
	else
		return 0;
}

int bullet_obstacle_collision(Bullet* bullet, Obstacle* obstacle){
	int bullet_top_left_x = bullet->bitmap->x;
	int bullet_y = bullet->bitmap->y;
	int bullet_top_right_x = bullet_top_left_x + bullet->bitmap->bitmapInfoHeader.width;
	int obstacle_x = obstacle->bitmaps[0]->x;
	int obstacle_y = obstacle->bitmaps[0]->y;
	int obstacle_width = obstacle->bitmaps[0]->bitmapInfoHeader.width;
	int obstacle_height = obstacle->bitmaps[0]->bitmapInfoHeader.height;

	int top_left_x_alligned = (bullet_top_left_x >= obstacle_x && bullet_top_left_x <= obstacle_x + obstacle_width ? 1 : 0);
	int top_right_x_alligned = (bullet_top_right_x >= obstacle_x && bullet_top_right_x <= obstacle_x + obstacle_width ? 1 : 0);
	int alligned_x_axis = top_left_x_alligned | top_right_x_alligned;
	int alligned_y_axis = (bullet_y <= obstacle_y + obstacle_height && bullet_y >= obstacle_y ? 1 : 0);

	if(alligned_x_axis && alligned_y_axis)
		return 1;
	else
		return 0;
}

void draw_bullet(Bullet* bullet, char* buffer){
	drawBitmap(bullet->bitmap,buffer,ALIGN_LEFT);
}

void delete_bullet(Bullet* bullet){
	deleteBitmap(bullet->bitmap);
	free(bullet);
}

Game* create_game(){
	Game* game = (Game*)(malloc(sizeof(Game)));
	game->mouse = create_game_mouse();
	game->keyboard = create_keyboard();
	game->background = loadBitmap(fullPath("new_game_background3.bmp"),0,0);
	game->background_score = loadBitmap(fullPath("score_background.bmp"),0,0);
	game->player = create_player();
	game->secondary_buffer = (char*)(malloc(vg_get_window_size()));
	game->obstacles = (Obstacle***)(malloc(N_OBSTACLE_LINES*sizeof(Obstacle**)));
	int i;
	for(i = 0; i < N_OBSTACLE_LINES; i++){
		game->obstacles[i] = (Obstacle**)(malloc(N_OBSTACLES*sizeof(Obstacle*)));
		int empty = rand() % RANDOM_OBSTACLE_FACTOR;
		if(empty % 2 == 0){
			int j;
			for(j = 0; j < N_OBSTACLES; j++)
				game->obstacles[i][j] = NULL;
		}
		else
			generate_obstacle_line(game->obstacles[i],N_OBSTACLES,i);
	}
	game->bullets = (Bullet**)(malloc(MAX_BULLETS_ON_SCREEN*sizeof(Bullet*)));
	for(i = 0; i < N_BULLETS; i++)
		game->bullets[i] = NULL;
	game->drawstate = DONTDRAW;
	game->state = GAME_RUNNING;
	return game;
}

int determine_index(int bullet_x){
	int i;
	for(i = 0; i < N_OBSTACLES; i++){
		if(bullet_x >= (i+1)*OBSTACLE_WIDTH && bullet_x <= (i+1)*OBSTACLE_WIDTH + 66)
			return i;
	}
	return -1;
}

void game_state_handler(Game* game){
	if(game->player->bitmap->y == vg_get_v_res() - PLAYER_DEATH_TOLERANCE)
		game->state = GAME_OVER; /* TO DO: Change later when final scoreboard screen is implemented */
	/*TO DO: Maybe add a pause state (then, on interrupts, we simply read the values and ignore them, not updating anything, unless it's the pause key again) */
}

void update_game(Game* game){
	//Update objects' positions
	int i, j;
	int off_screen[2];
	for(i = 0; i < N_OBSTACLE_LINES; i++){
		for(j = 0; j < N_OBSTACLES; j++){
			if(game->obstacles[i][j] != NULL)
				off_screen[i] = update_obstacle(game->obstacles[i][j]);
			else
				off_screen[i] = 1;
		}
	}

	//If current obstacle lines are off screen, generate new ones
	if(off_screen[0] && off_screen[1]){
		for(i = 0; i < N_OBSTACLE_LINES; i++){
			delete_obstacle_line(game->obstacles[i],N_OBSTACLES);
			generate_obstacle_line(game->obstacles[i],N_OBSTACLES,i);
		}
	}

	int bullet_off_screen;
	//Update shot bullets' positions
	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL){
			bullet_off_screen = update_bullet(game->bullets[i]);
			if(bullet_off_screen){
				delete_bullet(game->bullets[i]);
				game->bullets[i] = NULL;
			}
			else{
				int index = determine_index(game->bullets[i]->bitmap->x);
				if(index != -1){
					if(game->obstacles[1][index] != NULL){
						if(bullet_obstacle_collision(game->bullets[i],game->obstacles[1][index])){
							delete_bullet(game->bullets[i]);
							game->bullets[i] = NULL;
							if(--(game->obstacles[1][index]->lives) == 0){
								game->player->number_of_bullets += game->obstacles[1][index]->const_lives * BULLET_GAIN_FACTOR;
								delete_obstacle(game->obstacles[1][index]);
								game->obstacles[1][index] = NULL;
							}
						}
					}
					else if(game->obstacles[0][index] != NULL){
						if(bullet_obstacle_collision(game->bullets[i],game->obstacles[0][index])){
							delete_bullet(game->bullets[i]);
							game->bullets[i] = NULL;
							if(--(game->obstacles[0][index]->lives) == 0){
								game->player->number_of_bullets += game->obstacles[0][index]->const_lives * BULLET_GAIN_FACTOR;
								delete_obstacle(game->obstacles[0][index]);
								game->obstacles[0][index] = NULL;
							}
						}
					}
				}
			}
		}
	}

	update_player_collision(game->player,game->secondary_buffer);
	update_player_score(game->player);
	if(game->player->score_seconds % BONUS_FREQUENCY == 0 && game->player->score_seconds+game->player->score_minutes != 0 && game->player->score_aux == 0)
		generate_bonus(game->player);

	//Prepare next frame
	drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);

	for(i = 0; i < N_OBSTACLE_LINES; i++){
		for(j = 0; j < N_OBSTACLES; j++){
			if(game->obstacles[i][j] != NULL)
				draw_obstacle(game->obstacles[i][j],game->secondary_buffer);
		}
	}


	draw_player(game->player,game->secondary_buffer);

	//Draw player score
	draw_number(game->player->score_minutes,3,270,game->secondary_buffer);
	draw_number(game->player->score_seconds,39,270,game->secondary_buffer);

	//Draw player current number of bullets
	draw_number(game->player->number_of_bullets,31,400,game->secondary_buffer);

	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL)
			draw_bullet(game->bullets[i],game->secondary_buffer);
	}

	//Draw current player bonus information
	if(game->player->bonus == INVINCIBLE)
		drawBitmap(loadBitmap(fullPath("invincibility.bmp"),5,485), game->secondary_buffer, ALIGN_LEFT);
	else if(game->player->bonus == INFINITE_AMMO)
		drawBitmap(loadBitmap(fullPath("infiniteammo.bmp"),10,485), game->secondary_buffer, ALIGN_LEFT);
	else //bonus == NO_BONUS
		drawBitmap(loadBitmap(fullPath("nobonus.bmp"),10,485), game->secondary_buffer, ALIGN_LEFT);

	game_state_handler(game);
}

void update_draw_state(Game* game){
	if(game->drawstate == DONTDRAW)
		game->drawstate = DRAW;
	else
		game->drawstate = DONTDRAW;
}

int add_bullet_shot(Game* game, int x, int y){
	int success = 0;
	int i;
	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] == NULL){
			game->bullets[i] = create_bullet(x+BULLET_OFFSET,y-BULLET_HEIGHT);
			success = 1;
			break;
		}
	}

	return success;
}

void draw_game(Game* game){
	memcpy(vg_get_video_mem(),game->secondary_buffer,vg_get_window_size());
}

void delete_game(Game* game){
	delete_keyboard(game->keyboard);
	delete_game_mouse(game->mouse);
	deleteBitmap(game->background);
	delete_player(game->player);
	int i, j;
	for(i = 0; i < N_OBSTACLE_LINES; i++){
		for(j = 0; j < N_OBSTACLES; j++){
			if(game->obstacles[i][j] != NULL)
				delete_obstacle(game->obstacles[i][j]);
		}
	}
	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL)
			delete_bullet(game->bullets[i]);
	}
	free(game->secondary_buffer);
	free(game);
}
