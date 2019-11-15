/**
 * hello3_image.c - Initializes SDL, loads an image, And displys it in a window
 */

#include <stdio.h>
#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_timer.h>
#define RENDER_SCALE 3 
#define PPU_SCREEN_X 256
#define PPU_SCREEN_Y 240
void draw_sprite(int sprite_table, int i, int j, int sprite_number, unsigned char* chr_data, unsigned int* pixel_buffer);
void draw_chr_data(int i, int j, unsigned char* chr_data,  unsigned int* pixel_buffer, unsigned char pallette);
void updateRenderer(SDL_Renderer* rend, unsigned char* ppu_memory, unsigned char* sprite_data, int chr_length, SDL_Texture* texture);
void updateRenderer_2(int scanline, int ppu_cycle,unsigned char* ppu_memory);
void updateRenderer_3(SDL_Renderer* rend, SDL_Texture* texture);
SDL_Renderer* createRenderer(SDL_Window* win);
SDL_Texture* createTexture(SDL_Renderer* rend);
SDL_Window* createWindow();
char keepRunning(char* controller);
void createWindowAndRenderer(SDL_Window** wind, SDL_Renderer** rend);
