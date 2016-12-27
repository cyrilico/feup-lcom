#pragma once

#include "mouse.h"
#include "keyboard.h"
#include "score.h"
#include "bullet.h"

#define PLAYER_NAME_X_START 300
#define PLAYER_NAME_Y_START 266
#define PAUSE_MESSAGE_X 300
#define PAUSE_MESSAGE_Y 264
#define UNDERSCORE_GAP 67
#define HIGHSCORE_NAME_X 120
#define HIGHSCORE_NAME_Y 57

typedef enum gamestate_t {GAME_RUNNING, GAME_PAUSED, GAME_SCORE, GAME_OVER} gamestate;
typedef enum scorenamestate_t {FIRST_LETTER, SECOND_LETTER, THIRD_LETTER, FOURTH_LETTER, NAME_RECEIVED} scorenamestate;
typedef enum gamedrawstate_t {DONTDRAW, DRAW} gamedrawstate;

typedef struct{
	Obstacle*** obstacles;
	Mouse* mouse;
	Keyboard* keyboard;
	Bitmap* background;
	Player* player;
	char* secondary_buffer;
	Bullet** bullets;
	gamedrawstate drawstate;
	gamestate state;
	Score* session_score;
	Score** current_highscores;
	scorenamestate namestate;
	char session_name[NAME_LENGTH+1];
}Game;

Game* create_game();
int determine_index(int bullet_x); //Determines the index of the obstacle (in the first line) to test collision with (avoids testing for all and thus more calculations which would slow the game). Returns -1 if no obstacle in bullet's way
void update_game_running(Game* game);
void game_score_event_handler(Game* game, char current_key);
void update_game_score(Game* game);
void detect_game_end(Game* game);
void update_draw_state(Game* game);
void draw_game(Game* game);
int add_bullet_shot(Game* game, int x, int y); //Tries to shoot a bullet from player (whose position is (x,y)) by adding it to the game object (it may not be possible if MAX_BULLETS bullets are already on screen). Returns 1 if successful, 0 otherwise
void delete_game(Game* game);
