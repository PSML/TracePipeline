#include <stdint.h>
#include "ui.h"

static inline void
ui_draw_bit(uint8_t val, uint8_t bit, uint8_t fg, uint8_t bg, SDL_Rect *r, SDL_Surface *s)
{
  uint8_t color = (val & (1<<bit)) ? fg : bg;
  SDL_FillRect(s, r, color);
}

static inline void
ui_draw_byte(uint8_t val, uint8_t fg, uint8_t bg, SDL_Rect *r, SDL_Surface *s)
{
  ui_draw_bit(val, 0, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 1, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 2, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 3, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 4, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 5, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 6, fg, bg, r, s);
  r->x += r->w;
  ui_draw_bit(val, 7, fg, bg, r, s);
  r->x += r->w;
}

uintptr_t 
ui_paint(UI *ui, uint8_t *buf, uintptr_t size, uintptr_t bsize, char *outbmp, uintptr_t skipbegin, uintptr_t skipend)
{
  uint8_t  *b;
  uint8_t   fg, bg;
  intptr_t  i,j,blocks;
  SDL_Rect  bit;

  if (SDL_MUSTLOCK(ui->screen)) SDL_LockSurface(ui->screen);

  bit.h = (ui->zoom) ? ui->zoom : 1;
  if (bit.h > ui->resy) bit.h = ui->resy;
  bit.w = (ui->zoom) ? ui->zoom : 1;
  if (bit.w > ui->resx) bit.w = ui->resx;

  blocks = ui->resy / bit.h;
  if (blocks > (size/bsize)) blocks = size/bsize;

  for (i=0; i<blocks; i++) {
    bit.x = 0;
    bit.y = i * bit.h;
    b = &(buf[i*bsize]);
    ui_block_color(ui, b, bsize, &fg, &bg);
    for (j=0; j<bsize; j++) {
      if (j>=skipbegin && j<=skipend) continue;
      if (bit.x < ui->screen->w) { 
	printf("block=%ld byte=%ld: x=%d y=%d byte=%02x &b[j]=%p\n", i, j, bit.x, bit.y, b[j],&b[j]); 
	ui_draw_byte(b[j], fg, bg, &bit, ui->screen);
      }
    }
  }

  if (SDL_MUSTLOCK(ui->screen)) SDL_UnlockSurface(ui->screen);
 
  if (outbmp) SDL_SaveBMP(ui->screen, outbmp);
  else  SDL_UpdateRect(ui->screen, 0, 0, ui->screen->w, ui->screen->h);

  return blocks * bsize;
}

intptr_t
ui_init_sdl(UI *ui, int32_t h, int32_t w)
{
  static int ui_init_started = 0;
  SDL_Surface *info;
  if (ui_init_started == 0) {
    fprintf(stderr, "UI_init: Initializing SDL.\n");
    
    /* Initialize defaults, Video and Audio subsystems */
    if((SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER)==-1)) { 
      fprintf(stderr, "Could not initialize SDL: %s.\n", SDL_GetError());
      return -1;
    }
    
    atexit(SDL_Quit);
    info = SDL_SetVideoMode(0, 0, 8, SDL_FULLSCREEN|SDL_SWSURFACE);
    ui->resx = info->w; 
    ui->resy = info->h; 

    ui_init_started = 1;
  }

  if (w==0) w = ui->resx;
  if (h==0) h = ui->resy;
  ui->screen = SDL_SetVideoMode(w, h, 8, SDL_SWSURFACE);
  if ( ui->screen == NULL ) {
    fprintf(stderr, "Couldn't set %dx%dx%d video mode: %s\n", w, h, 8,
	    SDL_GetError());
    return -1;
  }
  if(ui->screen->format->BitsPerPixel!=8) {
    fprintf(stderr, "Not an 8-bit surface.\n");
    return -1 ;
  }
 
  ui->colors[UI_COLOR_BLACK]  = SDL_MapRGB(ui->screen->format, 0x00, 0x00, 0x00);
  ui->colors[UI_COLOR_WHITE]  = SDL_MapRGB(ui->screen->format, 0xff, 0xff, 0xff);
  ui->colors[UI_COLOR_RED]    = SDL_MapRGB(ui->screen->format, 0xff, 0x00, 0x00);
  ui->colors[UI_COLOR_GREEN]  = SDL_MapRGB(ui->screen->format, 0x00, 0xff, 0x00);
  ui->colors[UI_COLOR_BLUE]   = SDL_MapRGB(ui->screen->format, 0x00, 0x00, 0xff);
  ui->colors[UI_COLOR_YELLOW] = SDL_MapRGB(ui->screen->format, 0xff, 0xff, 0x00);
  ui->colors[UI_COLOR_PURPLE] = SDL_MapRGB(ui->screen->format, 0x80, 0x00, 0x80);
  ui->colors[UI_COLOR_ORANGE] = SDL_MapRGB(ui->screen->format, 0xFF, 0xA5, 0x00);
  
  fprintf(stderr, "UI_init: SDL initialized.\n");
  fprintf(stderr, "ui_init: SDL initialized: resx=%d resy=%d: h=%d w=%d\n", 
	  ui->resx, ui->resy, h, w);
  
  return 1;
}
 
#ifdef __UI_STAND_ALONE__
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define BUFSIZE (65544 * 100)
uint8_t buf[BUFSIZE];

void
ui_block_color(UI *ui, const uint8_t *blk, uintptr_t bsize, uint8_t *fg, uint8_t *bg)
{
  *bg = ui->colors[UI_COLOR_WHITE];
  *fg = ui->colors[UI_COLOR_BLACK];
}

int 
main(int argc, char **argv)
{
  char       c;
  UI         ui;
  int        fd, n;
  uintptr_t  offset, blocksize;
  char      *filename; 
  uint8_t    zoom;
  char      *out;
  char       outprefix[80];
  intptr_t   skipbegin = 0;
  intptr_t   skipend   = 65280;

  if (argc < 3) {
    fprintf(stderr, "USAGE: %s <blocksize> <filename> [zoom] [out]\n", argv[0]);
    return -1;
  }

  blocksize = atoi(argv[1]);
  filename  = argv[2];
  zoom      = (argc>=4) ? atoi(argv[3]) : 0;
  out       = (argc==5) ? outprefix : NULL;

  fd = open(filename, O_RDONLY);
  if (fd < 0 ) {
    perror("ERROR: open");
    return -1;
  }

  ui_init_sdl(&ui, 0, 0);
  ui.zoom = zoom;

  printf("file=%s blocksize=%" PRIdPTR "zoom=%d out=%s\n",
	 filename, blocksize, zoom, (out) ? argv[4] : NULL);

  while ((n = read(fd, buf, BUFSIZE)) > 0) {
    offset = 0;
    while (offset < n) {
      if (out) {
	snprintf(out, 80, "%s_%" PRIdPTR ".bmp", argv[4], offset);
      }
      printf("offset=%ld n=%d buf=%p (%p) %ld\n", offset, n, buf, &(buf[offset]), 
	     blocksize); 
      offset += ui_paint(&ui, &(buf[offset]), n-offset, blocksize, out, skipbegin,
			 skipend);
      if (out==NULL) c = getchar();
    }
  }
  return 0;
}
#endif
