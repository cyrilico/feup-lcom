#include "bullet.h"
#include "utils.h"

Bullet* create_bullet(Player* player, int x, int y){
	Bullet* bullet = (Bullet*)(malloc(sizeof(Bullet)));
	if(player->bonus != DOUBLE_BULLETS)
		bullet->bitmap = loadBitmap(fullPath("bullet.bmp"), x, y);
	else
		bullet->bitmap = loadBitmap(fullPath("bullet_special.bmp"), x, y);
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
