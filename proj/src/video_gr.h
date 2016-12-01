#pragma once

#include "sprite.h"

/** @defgroup video_gr video_gr
 * @{
 *
 * Functions for outputing data to screen in graphics mode
 */

/**
 * @brief Initializes the video module in graphics mode
 * 
 * Uses the VBE INT 0x10 interface to set the desired
 *  graphics mode, maps VRAM to the process' address space and
 *  initializes static global variables with the resolution of the screen, 
 *  and the number of colors
 * 
 * @param mode 16-bit VBE mode to set
 * @return Virtual address VRAM was mapped to. NULL, upon failure.
 */
void *vg_init(unsigned short mode);

 /**
 * @brief Returns to default Minix 3 text mode (0x03: 25 x 80, 16 colors)
 * 
 * @return 0 upon success, non-zero upon failure
 */
int vg_exit(void);

int vg_fill_pixel(unsigned int x, unsigned int y, unsigned long color);
int vg_fill_screen(unsigned int xi, unsigned int yi, unsigned int width, unsigned int height, unsigned long color);
int vg_draw_sprite(Sprite* s);
int vg_move_sprite(Sprite* s, float* x_cumulative, float* y_cumulative);

unsigned int vg_get_h_res();
unsigned int vg_get_v_res();
unsigned int vg_get_bits_per_pixel();
char* vg_get_video_mem();


 /** @} end of video_gr */
