#include "obstacle.h"
#include "utils.h"

Obstacle* create_obstacle(int x, int y){
	Obstacle* obstacle = (Obstacle*)(malloc(sizeof(Obstacle)));
	obstacle->lives = rand()%3+1;
	obstacle->const_lives = obstacle->lives;
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

void delete_obstacle_line(Obstacle** obstacles, int line_size){
	int i;
	for(i = 0; i < line_size; i++){
		if(obstacles[i] != NULL)
			delete_obstacle(obstacles[i]);
	}
}
