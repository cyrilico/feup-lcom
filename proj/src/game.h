


#define A_MAKE 0x1E
#define D_MAKE 0x20
#define ESC_MAKE 0x01

typedef struct {
	int IRQ_SET_KBD;
	int done, draw;
	unsigned long scancode;
} Game;

//Subscribes peripherals
Game* start_game();
//Refreshes screen information
int update_game(Game* game);
int draw_game(Game* game);
//Unsubscribes peripherals
int exit_program(Game* game);
