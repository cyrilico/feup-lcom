#include "utils.h"
#include "obstacle.h"
#include "player.h"

Player* create_player(){
	Player* player = (Player*)(malloc(sizeof(Player)));
	player->bitmap = loadBitmap(fullPath("buzz.bmp"), PLAYER_START_X, PLAYER_START_Y);
	player->bitmap_shield = loadBitmap(fullPath("buzz_shield.bmp"), PLAYER_START_X, PLAYER_START_Y);
	player->bonus = NO_BONUS;
	player->score_minutes = 0;
	player->score_seconds = 0;
	player->score_aux = 0;
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

	if(player->bonus != INVINCIBLE){
		/* Testing pixels around buzz to guarantee he doesn't go over obstacles on his sides (unless he has the invincibility bonus)
		 * Must test top, middle and bottom pixels to cover all cases
		 */
		unsigned long top_pixel_left = *(buffer + ((player->bitmap->y)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
		unsigned long top_pixel_right = *(buffer + ((player->bitmap->y)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

		unsigned long middle_pixel_left = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height/2)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
		unsigned long middle_pixel_right = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height/2)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

		unsigned long bottom_pixel_left = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height)*vg_get_h_res() + player->bitmap->x-1)*vg_get_bytes_per_pixel());
		unsigned long bottom_pixel_right = *(buffer + ((player->bitmap->y + player->bitmap->bitmapInfoHeader.height)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

		if(top_pixel_left != BLACK || top_pixel_right != BLACK || middle_pixel_left != BLACK || middle_pixel_right != BLACK || bottom_pixel_left != BLACK || bottom_pixel_right != BLACK)
			player->bitmap->x = previous_x;
	}

	//Update shield bitmap position
	player->bitmap_shield->x = player->bitmap->x;
	player->bitmap_shield->y = player->bitmap->y;
}

void update_player_collision(Player* player, char* buffer){
	if(player->bonus != INVINCIBLE){
		/* Check collision on top side of Buzz with obstacles */
		unsigned long topLeftPixel = *(buffer + ((player->bitmap->y-OBSTACLE_SPEED)*vg_get_h_res() + player->bitmap->x)*vg_get_bytes_per_pixel());
		unsigned long topRightPixel =  *(buffer + ((player->bitmap->y-OBSTACLE_SPEED)*vg_get_h_res() + (player->bitmap->x+player->bitmap->bitmapInfoHeader.width))*vg_get_bytes_per_pixel());

		if(topLeftPixel != BLACK || topRightPixel != BLACK)
			player->bitmap->y += OBSTACLE_SPEED;
		else if(player->bitmap->y != PLAYER_START_Y)
			player->bitmap->y--;
	}
}

void update_number_of_bullets(Player* player){
	if(player->bonus != INFINITE_AMMO)
		player->number_of_bullets--;
}

int player_has_bullets(Player* player){
	if(player->bonus == INFINITE_AMMO)
		return 1;
	else if(player->number_of_bullets > 0)
		return 1;
	else
		return 0;
}

void generate_bonus(Player* player){
	int bonus = rand() % NUMBER_OF_BONUSES;
	if(bonus == 0)
		player->bonus = INVINCIBLE;
	else if(bonus == 1)
		player->bonus = INFINITE_AMMO;
	else
		player->bonus = DOUBLE_BULLETS;

	player->bonus_timer = BONUS_DURATION;
}

void update_player_score(Player* player){
	player->score_aux++;
	if(player->score_aux == 60){
		player->score_aux = 0;
		player->score_seconds++;
		update_player_bonus(player);
		if(player->score_seconds == 60){
			player->score_seconds = 0;
			player->score_minutes++;
		}
	}
}

void update_player_bonus(Player* player){
	if(player->bonus != NO_BONUS){
		if(--(player->bonus_timer) == 0)
			player->bonus = NO_BONUS;
	}
}

void draw_player(Player* player, char* buffer){
	if(player->bonus == INVINCIBLE)
		drawBitmap(player->bitmap_shield, buffer, ALIGN_LEFT);
	else
		drawBitmap(player->bitmap,buffer,ALIGN_LEFT);
}

void delete_player(Player* player){
	deleteBitmap(player->bitmap);
	deleteBitmap(player->bitmap_shield);
	free(player);
}
