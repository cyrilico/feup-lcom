#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "rtc.h"
#include "video_gr.h"
#include "utils.h"
#include "game.h"

Game* create_game(){
	Game* game = (Game*)(malloc(sizeof(Game)));
	game->mouse = create_mouse();
	game->keyboard = create_keyboard();
	game->background = loadBitmap(fullPath("game_background.bmp"),0,0);
	game->player = create_player();
	game->secondary_buffer = (char*)(malloc(vg_get_window_size()));
	game->obstacles = (Obstacle***)(malloc(N_OBSTACLE_LINES*sizeof(Obstacle**)));
	init_current_max_lives();
	init_current_min_lives();
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

void detect_game_end(Game* game){
	if(game->player->bitmap->y >= vg_get_v_res() - PLAYER_DEATH_TOLERANCE){
		//Change state to score registering screen and initialize 'attributes' related to it
		game->state = GAME_SCORE;
		//Substitute background
		deleteBitmap(game->background);
		game->background = loadBitmap(fullPath("score_background.bmp"),0,0);
		game->current_highscores = read_scores_from_file();
		game->namestate = FIRST_LETTER;
		int i;
		for(i = 0; i < NAME_LENGTH+1; i++)
			game->session_name[i] = 0;
		unsigned long* session_time = bcd_to_binary_array(rtc_get(CURRENT_TIME),TIME_LENGTH);
		unsigned long* session_date = bcd_to_binary_array(rtc_get(CURRENT_DATE),DATE_LENGTH);
		game->session_score = create_score(game->player->score_minutes,game->player->score_seconds,session_time,session_date,game->session_name); //Current session name will be registered by user

		//Prepare first frame so it doesn't get 'stuck' in GAME_RUNNING screen until user presses a key
		drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);

		//Draw current session score
		int position_x = SESSION_SCORE_X_START;
		draw_score_number(game->session_score->points_minutes,position_x,SESSION_SCORE_Y,game->secondary_buffer);
		position_x += 2*SCORE_BITMAP_WIDTH; //2 numbers were drawn in above call
		draw_colon(position_x,SESSION_SCORE_Y,game->secondary_buffer);
		position_x += SCORE_BITMAP_WIDTH;
		draw_score_number(game->session_score->points_seconds,position_x,SESSION_SCORE_Y,game->secondary_buffer);

		//Draw current highscores
		draw_scores(game->current_highscores,20,400,game->secondary_buffer);
	}
}

void update_game_running(Game* game){
	//Update obstacles' positions
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
	//Update shot bullets' positions (for each, checks what obstacle is in its way and checks collision with only that one)
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
							if(game->player->bonus == DOUBLE_BULLETS && game->obstacles[1][index]->lives >= 2)
								game->obstacles[1][index]->lives -= 2;
							else
								game->obstacles[1][index]->lives--;
							if(game->obstacles[1][index]->lives == 0){
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
							if(game->player->bonus == DOUBLE_BULLETS && game->obstacles[0][index]->lives >= 2)
								game->obstacles[0][index]->lives -= 2;
							else
								game->obstacles[0][index]->lives--;
							if(game->obstacles[0][index]->lives == 0){
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
	int time_to_generate_bonus = (game->player->score_seconds % BONUS_FREQUENCY == 0 ? 1 : 0);
	int not_game_beginning = (game->player->score_seconds+game->player->score_minutes != 0 ? 1 : 0); // 0 % any number is 0, so this is to avoid giving a bonus when the game starts
	int score_seconds_just_incremented = (game->player->score_aux == 0 ? 1 : 0); //if a bonus is generated every 15 seconds, we'd generate 60 different bonuses during said second and the next one. only want to do it once
	if(time_to_generate_bonus && not_game_beginning && score_seconds_just_incremented){
		generate_bonus(game->player);
		update_lives_boundaries();
	}

	/* PREPARE NEXT FRAME */
	//Draw background
	drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);

	//Draw obstacles
	for(i = 0; i < N_OBSTACLE_LINES; i++){
		for(j = 0; j < N_OBSTACLES; j++){
			if(game->obstacles[i][j] != NULL)
				draw_obstacle(game->obstacles[i][j],game->secondary_buffer);
		}
	}

	//Draw player
	draw_player(game->player,game->secondary_buffer);

	//Draw player's score
	draw_game_number(game->player->score_minutes,3,270,game->secondary_buffer);
	draw_game_number(game->player->score_seconds,39,270,game->secondary_buffer);

	//Draw player's current number of bullets
	draw_game_number(game->player->number_of_bullets,31,400,game->secondary_buffer);

	//Draw bullets on screen
	for(i = 0; i < MAX_BULLETS_ON_SCREEN; i++){
		if(game->bullets[i] != NULL)
			draw_bullet(game->bullets[i],game->secondary_buffer);
	}

	//Draw current player bonus information
	if(game->player->bonus == INVINCIBLE)
		drawBitmap(loadBitmap(fullPath("invincibility.bmp"),5,485), game->secondary_buffer, ALIGN_LEFT);
	else if(game->player->bonus == INFINITE_AMMO)
		drawBitmap(loadBitmap(fullPath("infiniteammo.bmp"),10,485), game->secondary_buffer, ALIGN_LEFT);
	else if(game->player->bonus == DOUBLE_BULLETS)
		drawBitmap(loadBitmap(fullPath("doublebullets.bmp"),10,485), game->secondary_buffer, ALIGN_LEFT);
	else //bonus == NO_BONUS
		drawBitmap(loadBitmap(fullPath("nobonus.bmp"),10,485), game->secondary_buffer, ALIGN_LEFT);

	detect_game_end(game);
}

void game_score_event_handler(Game* game, char current_key){
	switch(game->namestate){
	case FIRST_LETTER:
		if(current_key != NOT_VALID){
			game->session_name[FIRST_LETTER] = current_key;
			game->namestate = SECOND_LETTER;
		}
		break;
	case SECOND_LETTER:
		if(key_detected(game->keyboard,BACKSPACE_BREAK)){
			game->session_name[FIRST_LETTER] = 0;
			game->namestate = FIRST_LETTER;
		}
		else if(current_key != NOT_VALID){
			game->session_name[SECOND_LETTER] = current_key;
			game->namestate = THIRD_LETTER;
		}
		break;
	case THIRD_LETTER:
		if(key_detected(game->keyboard,BACKSPACE_BREAK)){
			game->session_name[SECOND_LETTER] = 0;
			game->namestate = SECOND_LETTER;
		}
		else if(current_key != NOT_VALID){
			game->session_name[THIRD_LETTER] = current_key;
			game->namestate = FOURTH_LETTER;
		}
		break;
	case FOURTH_LETTER:
		if(key_detected(game->keyboard,BACKSPACE_BREAK)){
			game->session_name[THIRD_LETTER] = 0;
			game->namestate = THIRD_LETTER;
		}
		else if(current_key != NOT_VALID){
			game->session_name[FOURTH_LETTER] = current_key;
			game->namestate = NAME_RECEIVED;
		}
		break;
	case NAME_RECEIVED:
		if(key_detected(game->keyboard,BACKSPACE_BREAK)){
			game->session_name[FOURTH_LETTER] = 0;
			game->namestate = FOURTH_LETTER;
		}
		else if(key_detected(game->keyboard,ENTER_BREAK)){
			set_score_name(game->session_score,game->session_name);
			write_score_to_file(game->session_score);
			game->state = GAME_OVER;
		}
		break;
	}
}

void update_game_score(Game* game) {
	char current_key = scancode_to_letter(game->keyboard->scancode);
	game_score_event_handler(game, current_key);

	/* PREPARE NEXT FRAME */
	//Draw background
	drawBitmap(game->background,game->secondary_buffer,ALIGN_LEFT);

	//Draw current session score
	int position_x = SESSION_SCORE_X_START;
	draw_score_number(game->session_score->points_minutes,position_x,SESSION_SCORE_Y,game->secondary_buffer);
	position_x += 2*SCORE_BITMAP_WIDTH; //2 numbers were drawn in above call
	draw_colon(position_x,SESSION_SCORE_Y,game->secondary_buffer);
	position_x += SCORE_BITMAP_WIDTH;
	draw_score_number(game->session_score->points_seconds,position_x,SESSION_SCORE_Y,game->secondary_buffer);

	//Draw current session name (or part of it)
	int i;
	for(i = 0; i < NAME_LENGTH; i++){
		if(game->session_name[i] != NOT_VALID)
			draw_letter(game->session_name[i],PLAYER_NAME_X_START+i*UNDERSCORE_GAP,PLAYER_NAME_Y_START,game->secondary_buffer);
	}

	//Draw current highscores
	draw_scores(game->current_highscores,20,400,game->secondary_buffer);
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
			game->bullets[i] = create_bullet(game->player, x+BULLET_OFFSET,y-BULLET_HEIGHT);
			success = 1;
			break;
		}
	}

	return success;
}

void draw_game(Game* game){
	if(game->state == GAME_PAUSED)
		drawBitmap(loadBitmap(fullPath("pause_message.bmp"),PAUSE_MESSAGE_X,PAUSE_MESSAGE_Y),game->secondary_buffer,ALIGN_LEFT);
	memcpy(vg_get_video_mem(),game->secondary_buffer,vg_get_window_size());
}

void delete_game(Game* game){
	delete_keyboard(game->keyboard);
	delete_mouse(game->mouse);
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
	delete_score(game->session_score);
	for(i = 0; game->current_highscores[i] != NULL; i++)
		delete_score(game->current_highscores[i]);
	free(game->secondary_buffer);
	free(game);
}
