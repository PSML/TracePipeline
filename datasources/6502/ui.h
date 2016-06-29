#ifndef __UI_H__
#define __UI_H__

#include <SDL/SDL.h>   /* All SDL apps need this */

#define UI_COLOR_BLACK   0
#define UI_COLOR_WHITE   1
#define UI_COLOR_RED     2
#define UI_COLOR_GREEN   3 
#define UI_COLOR_BLUE    4
#define UI_COLOR_YELLOW  5
#define UI_COLOR_PURPLE  6
#define UI_COLOR_ORANGE  7
#define UI_COLOR_NUM     8

struct UI_Struct {  
  SDL_Surface *screen;
  int resx, resy;  
  uint8_t colors[UI_COLOR_NUM];
  uint8_t zoom;
};

typedef struct UI_Struct UI;

extern void ui_block_color(UI *ui, const uint8_t *blk, uintptr_t bsize,
			   uint8_t *fg, uint8_t *bg);

uintptr_t ui_paint(UI *ui, uint8_t *buf, uintptr_t size, uintptr_t bs, char *outbmp,
		   uintptr_t skipbegin, uintptr_t skipend);
intptr_t  ui_init_sdl(UI *ui, int32_t h, int32_t w);

#endif
