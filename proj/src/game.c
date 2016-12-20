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
	game->background = loadBitmap(fullPath("new_game_background2.bmp"),0,0);
	game->background_menu = loadBitmap(fullPath("score_background.bmp"),0,0);
	game->player = create_player();
	game->secondary_buffer = (char*)(malloc(vg_get_window_size()));
	game->obstacles = (Obstacle**)(malloc(N_OBSTACLES*sizeof(Obstacle*)));
	generate_obstacle_line(game->obstacles,N_OBSTACLES);
	game->bullets = (Bullet**)(malloc(MAX_BULLETS_ON_SCREEN*sizeof(Bullet*)));
	int i;
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
		game->state = GAME_SCORE;
	/*TO DO: Maybe add a pause state (then, on interrupts, we simply read the values and ignore them, not updating anything, unless it's the pause key again) */
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

	//If current obstacle line if off screen, generate a new one
	if(off_screen){
		delete_obstacle_line(game->obstacles,N_OBSTACLES);
		generate_obstacle_line(game->obstacles,N_OBSTACLES);
	}

	//Update shot bullets' positions
	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL){
			off_screen = update_bullet(game->bullets[i]);
			if(off_screen){
				delete_bullet(game->bullets[i]);
				game->bullets[i] = NULL;
			}
			else{
				int index = determine_index(game->bullets[i]->bitmap->x);
				if(index != -1){
					if(game->obstacles[index] != NULL){
						if(bullet_obstacle_collision(game->bullets[i],game->obstacles[index])){
							delete_bullet(game->bullets[i]);
							game->bullets[i] = NULL;
							if(--(game->obstacles[index]->lives) == 0){
								game->player->number_of_bullets += game->obstacles[index]->const_lives * BULLET_GAIN_FACTOR;
								delete_obstacle(game->obstacles[index]);
								game->obstacles[index] = NULL;
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

	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			draw_obstacle(game->obstacles[i],game->secondary_buffer);
	}


	draw_player(game->player,game->secondary_buffer);

	//Draw player score
	/* TO DO: Update its display location) */
	draw_number(game->player->score_minutes,3,270,game->secondary_buffer);
	draw_number(game->player->score_seconds,39,270,game->secondary_buffer);

	//Draw player current number of bullets
	draw_number(game->player->number_of_bullets,31,400,game->secondary_buffer);

	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL)
			draw_bullet(game->bullets[i],game->secondary_buffer);
	}

	/* Update this later (print bonus instead of number corresponding to it)
	 * SIDE NOTE: When it's calculating the next bonus to give to the player, it prints out 0 and 1 like a mod 2 sequence (0,1,0,1,0,1)
	 * very fast until one is finally chosen and remains in the screen. No idea why this happens BUT IT'S FUCKING COOL
	 * TO DO: Change bonus activation to key-pressing detecting mechanism (timer is already used to control its duration so using keyboard for something
	 * other than shooting will show good use of 'simultaneous' interrupts */
	if(game->player->bonus == INVINCIBLE)
		draw_number(0, 30, 500, game->secondary_buffer);
	else if(game->player->bonus == INFINITE_AMMO)
		draw_number(1, 30, 500, game->secondary_buffer);
	else //bonus = NO_BONUS
		draw_number(2, 30, 500, game->secondary_buffer);

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
	int i;
	for(i = 0; i < N_OBSTACLES; i++){
		if(game->obstacles[i] != NULL)
			delete_obstacle(game->obstacles[i]);
	}
	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL)
			delete_bullet(game->bullets[i]);
	}
	free(game->secondary_buffer);
	free(game);
}
