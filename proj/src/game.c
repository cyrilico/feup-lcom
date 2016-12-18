#include "game.h"

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->lives = rand()%3+1;
	int i;
	for(i = 0; i < 3; i++){
		char temp[15];
		sprintf(temp,"obstacle%d.bmp",i+1);
		obstacle->bitmaps[i] = loadBitmap(fullPath(temp),x,y);
	}
	return obstacle;
}

int update_obstacle(Obstacle* obstacle){
	if(obstacle_off_screen(obstacle))
		return 1;
	else{
		int i;
		for(i = 0; i < 3; i++)
			obstacle->bitmaps[i]->y += OBSTACLE_SPEED;

		return 0;
	}
}

void draw_obstacle(Obstacle* obstacle, char* buffer){
	drawBitmap(obstacle->bitmaps[obstacle->lives-1],buffer,ALIGN_LEFT);
}

int obstacle_off_screen(Obstacle* obstacle){
	if(obstacle->bitmaps[0]->y + 1 >= vg_get_v_res())
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

void generate_obstacle_line(Obstacle** obstacles, int line_size){
	int i;
	for(i = 0; i < line_size; i++){
		int empty = rand() % EMPTY_FACTOR; //Determine if empty space or enemy's there
		if(!empty)
			obstacles[i] = create_obstacle(OBSTACLE_WIDTH*(i+1),OBSTACLE_HEIGHT);
		else
			obstacles[i] = NULL;
	}
}

void delete_obstacle_line(Obstacle** obstacles, int line_size){
	int i;
	for(i = 0; i < line_size; i++){
		if(obstacles[i] != NULL)
			delete_obstacle(obstacles[i]);
	}
}

Player* create_player(){
	Player* player = (Player*)(malloc(sizeof(Player)));
	int player_start_x = rand()%(vg_get_h_res()/2) + LEFT_LIMIT; //Random starting position
	player->bitmap = loadBitmap(fullPath("buzz.bmp"), player_start_x, PLAYER_START_Y);
	player->score_minutes = 0;
	player->score_seconds = 0;
	player->score_aux = 0;
	/*TO DO: initialization of remaining structures (when they're created in Player 'class' */
	/* \/ Save technique for later: fast load of all number bitmaps \/ */
	/*for(i = 0; i < 10; i++){
		char temp[10];
		sprintf(temp,"%d.bmp",i);
		player->numbers[i] = loadBitmap(fullPath(temp),25,300);
	}
	*/
	player->number_of_bullets = N_BULLETS;

	return player;
}

void update_player_mouse(Player* player, Mouse* mouse, char* buffer){

	int previous_x = player->bitmap->x;

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

	/* TO DO: Try to optimize lateral collision testing */
	unsigned long topPixelLeft = *(buffer + ((player->bitmap->y)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
	unsigned long topPixelRight = *(buffer + ((player->bitmap->y)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

	unsigned long middlePixelLeft = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height/2)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
	unsigned long middlePixelRight = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height/2)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

	unsigned long bottomPixelLeft = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
	unsigned long bottomPixelRight = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

	if(topPixelLeft != BLACK || topPixelRight != BLACK || middlePixelLeft != BLACK || middlePixelRight != BLACK || bottomPixelLeft != BLACK || bottomPixelRight != BLACK)
		player->bitmap->x = previous_x;

	/*int counter;
	unsigned long currentPixelLeft;
	unsigned long currentPixelRight;

	for(counter = 0; counter < player->bitmap->bitmapInfoHeader.height; counter ++) {
		currentPixelLeft = *(buffer + ((player->bitmap->y + counter)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
		currentPixelRight = *(buffer + ((player->bitmap->y + counter)*vg_get_h_res() + player->bitmap->x + player->bitmap->bitmapInfoHeader.width + 1)*vg_get_bytes_per_pixel());
		if(currentPixelLeft != BLACK || currentPixelRight != BLACK)
			player->bitmap->x = previous_x;
	}*/
}

void update_player_collision(Player* player, char* buffer){
	unsigned long topLeftPixel = *(buffer + ((player->bitmap->y-OBSTACLE_SPEED)*vg_get_h_res() + player->bitmap->x)*vg_get_bytes_per_pixel());
	unsigned long topRightPixel =  *(buffer + ((player->bitmap->y-OBSTACLE_SPEED)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

	if(topLeftPixel != BLACK || topRightPixel != BLACK)
		player->bitmap->y += OBSTACLE_SPEED;
	else if(player->bitmap->y != PLAYER_START_Y)
		player->bitmap->y--;
}

void update_number_of_bullets(Player* player){
	player->number_of_bullets--;
}

int player_has_bullets(Player* player){
	if(player->number_of_bullets > 0)
		return 1;
	else
		return 0;
}

void update_player_score(Player* player){
	player->score_aux++;
	if(player->score_aux == 60){
		player->score_aux = 0;
		player->score_seconds++;
		if(player->score_seconds == 60){
			player->score_seconds = 0;
			player->score_minutes++;
		}
	}
}

void draw_player(Player* player, char* buffer){
	drawBitmap(player->bitmap,buffer,ALIGN_LEFT);
}

void delete_player(Player* player){
	deleteBitmap(player->bitmap);
	free(player);
}

Bullet* create_bullet(int x, int y){
	Bullet* bullet = (Bullet*)(malloc(sizeof(Bullet)));
	bullet->bitmap = loadBitmap(fullPath("new_bullet.bmp"), x, y);
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

int determine_index(int x){
	int i;
	for(i = 0; i < N_OBSTACLES; i++){
		if(x >= (i+1)*OBSTACLE_WIDTH && x <= (i+1)*OBSTACLE_WIDTH + 66)
			return i;
	}
	return -1;
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

	/* TO DO: Layer this (function to detect if player has lost) */
	if(game->player->bitmap->y == vg_get_v_res() - PLAYER_DEATH_TOLERANCE)
		game->state = GAME_OVER;
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
