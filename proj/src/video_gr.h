#pragma once

/*! \file dispatcher.h
 \brief Functions for outputing data to screen in graphics mode
*/

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

/**
 * @brief Provides the value of horizontal resolution in graphics mode being used at that moment
 *
 * @return copy of private global variable h_res, which holds the current horizontal resolution
 */
unsigned int vg_get_h_res();

/**
 * @brief Provides the value of vertical resolution in graphics mode being used at that moment
 *
 * @return copy of private global variable v_res, which holds the current vertical resolution
 */
unsigned int vg_get_v_res();

/**
 * @brief Provides the number of bytes used per pixel in graphics mode being used at that moment
 *
 * @return copy of private global variable bits_per_pixel divided by 8
 */
unsigned int vg_get_bytes_per_pixel();

/**
 * @brief Provides the base address where VRAM was mapped to
 *
 * @return copy of private global variable video_mem
 */
char* vg_get_video_mem();

/**
 * @brief Provides the window's size in graphics mode being used at that moment, in bytes
 *
 * @return window size (in number of bytes)
 */
unsigned int vg_get_window_size();


 /** @} end of video_gr */
