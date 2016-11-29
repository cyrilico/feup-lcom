#include "sprite.h"
#include "video.h"
#include <minix/syslib.h>
#include <minix/drivers.h>
#include <machine/int86.h>
#include <sys/mman.h>
#include <sys/types.h>

Sprite* create_sprite(char *pic[], int x, int y, float xspeed, float yspeed) {
	//allocate space for the "object"
	Sprite *sp = (Sprite *) malloc ( sizeof(Sprite));
	if( sp == NULL )
		return NULL;

	// read the sprite pixmap
	//sp->map = read_xpm(pic, &(sp->width), &(sp->height));

	if( sp->map == NULL ) {
		free(sp);
		return NULL;
	}
	sp->x = x;
	sp->y = y;
	sp->xspeed = xspeed;
	sp->yspeed = yspeed;
	return sp;
}


void destroy_sprite(Sprite *fig, char* base) {
	if( fig == NULL )
		return;
	free(fig->map);
	free(fig);
	fig = NULL; // hopeless: pointer is passed by value
	// should do this @ the caller
}
