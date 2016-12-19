#include "player.h"
#include "obstacle.h"

Player* create_player(){
	Player* player = (Player*)(malloc(sizeof(Player)));
	int player_start_x = rand()%(vg_get_h_res()/2) + LEFT_LIMIT; //Random starting position
	/*TO DO: Change buzz bitmap to a green-screen like one and update drawBitmap so it doesn't draw that color (otherwise, when INVINCIBLE bonus is on and buzz
	 * goes over an obstacle, it shows his black background. ALTERNATIVE: Make him Super buzz and instead of going over it, make him destroy it just by touching
	 * it (and gain the respective amount of bullets, of course)
	 */
	player->bitmap = loadBitmap(fullPath("buzz.bmp"), player_start_x, PLAYER_START_Y);
	player->bonus = NO_BONUS;
	player->score_minutes = 0;
	player->score_seconds = 0;
	player->score_aux = 0;
	/*TO DO: initialization of remaining structures (when they're created in Player 'class') */
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
	if(player->bonus != INVINCIBLE){
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
	else
		player->bonus = INFINITE_AMMO;

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
	drawBitmap(player->bitmap,buffer,ALIGN_LEFT);
}

void delete_player(Player* player){
	deleteBitmap(player->bitmap);
	free(player);
}
