/**
 * hello3_image.c - Initializes SDL, loads an image, And displys it in a window
 */

#include <stdio.h>
#include </usr/include/SDL2/SDL.h>
#include </usr/include/SDL2/SDL_timer.h>
#define RENDER_SCALE 3 
void draw_sprite(int sprite_table, int i, int j, int sprite_number, unsigned char* chr_data, SDL_Renderer* rend);
void draw_chr_data(int i, int j, unsigned char* chr_data, SDL_Renderer* rend);
void updateRenderer(SDL_Renderer* rend, unsigned char* ppu_memory, unsigned char* sprite_data, int chr_length);
SDL_Renderer* createRenderer(SDL_Window* win);
SDL_Window* createWindow();
char keepRunning();
