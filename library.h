/* 
 * Graphics Library header file
 * (c) Mohammad H. Mofrad, 2017
 */

#ifndef _LIBRARY_INCLUDED_H
#define _LIBRARY_INCLUDED_H

#include <stdlib.h>
#include <stdio.h>

#include <linux/fb.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <time.h>
#include <termio.h>
#include <termios.h>
////////////////////////
//#include "iso_font.h" // iso_font.h MUST BE IN SAME DIRECTORY
////////////////////////
// iso_font.h defined in library.c
////////////////////////

typedef unsigned short color_t;    // |15 11|10  5|4  0|
                                   // |red  |green|blue|
                                   //   5   + 6   + 5  =  16 bits
// Macros to extract RGB colors 
#define BMASK(c) (c & 0x001F) // Blue mask
#define GMASK(c) (c & 0x07E0) // Green mask
#define RMASK(c) (c & 0xF800) // Red mask
#define RGB(r, g, b) ((color_t)((r << 10) | (g << 5) | b))
#define RED RGB(63, 0, 0)
#define GREEN RGB(0, 63, 0)
#define BLUE RGB(0, 0, 63)



// Function prototypes
void init_graphics();
void exit_graphics();
void clear_screen();
char getkey();
void sleep_ms(long ms);
void draw_pixel(int x, int y, color_t color);
void draw_rect(int xl, int y1, int width, int height, color_t c);
void draw_text(int x, int y, const char * text, color_t c);

#endif
