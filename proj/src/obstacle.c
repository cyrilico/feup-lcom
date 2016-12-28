#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>
#include "utils.h"
#include "obstacle.h"

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->lives = rand() % current_max_lives + current_min_lives;
	obstacle->const_lives = obstacle->lives;
	int i;
	for(i = 0; i < MAX_OBSTACLE_LIVES; i++){
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
		for(i = 0; i < MAX_OBSTACLE_LIVES; i++)
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
	for(i = 0; i < MAX_OBSTACLE_LIVES; i++)
		deleteBitmap(obstacle->bitmaps[i]);
	free(obstacle);
}

void generate_obstacle_line(Obstacle** obstacles, int line_size, int line_number){
	int i;
	for(i = 0; i < line_size; i++){
		int empty = rand() % EMPTY_FACTOR; //Determine if empty space or enemy's there
		if(!empty)
			obstacles[i] = create_obstacle(OBSTACLE_WIDTH*(i+1),OBSTACLE_HEIGHT*(1+line_number));
		else
			obstacles[i] = NULL;
	}
}

void init_current_max_lives() {
	current_max_lives = 1;
}
void init_current_min_lives() {
	current_min_lives = 1;
}

void update_lives_boundaries() {
	if(current_max_lives < MAX_OBSTACLE_LIVES && current_min_lives == 1)
		current_max_lives++;
	else if(current_min_lives < MAX_OBSTACLE_LIVES) {
		if(current_max_lives > 1)
			current_max_lives--; //To make sure lives values stay between 1-5. At this point max_lives is no longer directly the maximum number of an obstacle's lives
		current_min_lives++;
	}
}

void delete_obstacle_line(Obstacle** obstacles, int line_size){
	int i;
	for(i = 0; i < line_size; i++){
		if(obstacles[i] != NULL)
			delete_obstacle(obstacles[i]);
	}
}
