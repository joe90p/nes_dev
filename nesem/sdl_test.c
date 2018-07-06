/**
 * hello3_image.c - Initializes SDL, loads an image, And displys it in a window
 */
#include </home/phil/git/nes_dev/nesem/sdl_test.h>

void draw(unsigned char* ppu_memory,unsigned char* sprite_data, int chr_length)
{
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    Uint32 window_flags = SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS;
    SDL_Window* win = SDL_CreateWindow("Hello, CS50!",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       RENDER_SCALE*256, RENDER_SCALE*240, window_flags);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	    return 1;
    }

    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
      printf("error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(win);
      SDL_Quit();
      return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetScale(rend,RENDER_SCALE,RENDER_SCALE);


    SDL_SetRenderDrawColor( rend, 255, 0, 0, 255);

    SDL_RenderClear(rend);

    int row_length = 32;   
    int row_num = 30;
    for(int q=0; q<row_length*row_num; q++)
    {
     int col = q < row_length ? q : q%row_length; 
     int name_table_index = 0x2000;
     int sprite_number = ppu_memory[name_table_index + q];
     draw_sprite(1, col*8,(q/row_length)*8,sprite_number,ppu_memory,rend); 
    }

    for(int i=0;i<64; i++)
    {
      unsigned char sprite_index = i *4;
      draw_sprite(0, sprite_data[sprite_index+3], sprite_data[sprite_index]+1, sprite_data[sprite_index+1], ppu_memory, rend);
    }

    SDL_RenderPresent(rend);
    // wait a few seconds
    SDL_Delay(5000);
    
    // clean up resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void draw_sprite(int sprite_table, int i, int j, int sprite_number, unsigned char* ppu_memory, SDL_Renderer* rend)
{
  for(int m=0; m<8; m++)
  {
     int sprite_index_offset = sprite_table * 0x1000;
     int sprite_index = sprite_index_offset + (sprite_number*16) + m;
     unsigned char* chr_data = &(ppu_memory[sprite_index]);
     draw_chr_data(i, j +m, chr_data, rend);
  }
}

void draw_chr_data(int i, int j, unsigned char* chr_data, SDL_Renderer* rend)
{
  unsigned char chr_data_1 = chr_data[0];
  unsigned char chr_data_2 = chr_data[8];
  for(int n=0; n<8; n++)
  {
    unsigned char data = (chr_data_1&1) | ((chr_data_2<<1)&3);
    switch(data)
    {
      case 0:
        SDL_SetRenderDrawColor( rend, 255, 0, 0, 255);
        break;
      case 1:
        SDL_SetRenderDrawColor( rend, 0, 255, 0, 255);
        break;
      case 2:
        SDL_SetRenderDrawColor( rend, 0, 0, 255, 255);
        break;
      case 3:
        SDL_SetRenderDrawColor( rend, 255, 0, 255, 255);
        break;
    }
    SDL_RenderDrawPoint(rend, i + 7 -n,j );
    chr_data_1>>=1;
    chr_data_2>>=1;
  }
}
