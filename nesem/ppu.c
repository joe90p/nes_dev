
#include <ppu.h>
char keepRunning(char* controller)
{
  SDL_Event event;
  SDL_PollEvent(&event);
  //*controller=8;
  //*controller=0;
  switch(event.type) {
    case SDL_QUIT:
      return 0;
    case SDL_KEYDOWN:
      switch(event.key.keysym.sym)
      {
        case SDLK_w:
           *controller=(*controller)|8;
           //*controller=8;
          break;  
      }  
      return 1;
    default:
      return 1;
  }
}

SDL_Window* createWindow() {

    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        //return 1;
    }
    SDL_SetHint(SDL_HINT_VIDEO_X11_NET_WM_PING, "0");
    Uint32 window_flags = SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS;
    SDL_Window* win = SDL_CreateWindow("Hello, CS50!",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       RENDER_SCALE*256, RENDER_SCALE*240, window_flags);
    if (!win)
    {
        printf("error creating window: %s\n", SDL_GetError());
        SDL_Quit();
	//    return 1;
    }
    return win;
}

unsigned int* pixel_buffer_2;
SDL_Renderer* createRenderer(SDL_Window* win)
{
    // create a renderer, which sets up the graphics hardware
    Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
    if (!rend)
    {
      printf("error creating renderer: %s\n", SDL_GetError());
      SDL_DestroyWindow(win);
      SDL_Quit();
      //return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_RenderSetScale(rend,RENDER_SCALE,RENDER_SCALE);


    SDL_SetRenderDrawColor( rend, 255, 0, 0, 255);
    pixel_buffer_2=malloc(256*240*sizeof(unsigned int));
    return rend;

}

SDL_Texture* createTexture(SDL_Renderer* rend)
{
return SDL_CreateTexture(
        rend,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
256, 240);
}
void createWindowAndRenderer(SDL_Window** wind, SDL_Renderer** rend)
{
  if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
  {
    printf("BIG error");
  }
  SDL_CreateWindowAndRenderer(PPU_SCREEN_X * 2, PPU_SCREEN_Y *2, SDL_WINDOW_SHOWN, wind, rend);
  SDL_SetWindowTitle(*wind, "nesem by phil");
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
  SDL_RenderSetLogicalSize(*rend, PPU_SCREEN_X, PPU_SCREEN_Y);
} 
void draw_sprite_2(int sprite_table, int i, int j, int sprite_number, unsigned char* ppu_memory, unsigned int* pixel_buffer)
{
     int sprite_index_offset = sprite_table * 0x1000;
     int sprite_index = sprite_index_offset + (sprite_number*16) + (j%8);
     unsigned char* chr_data = &(ppu_memory[sprite_index]);
     draw_chr_data(i, j, chr_data, pixel_buffer);
}

void updateRenderer_2(int scanline, int ppu_cycle,unsigned char* ppu_memory){
  if(ppu_cycle <=257 && ((ppu_cycle-1)%8)==0)
  {
    int nt_byte = (ppu_cycle - 1) /8; 
    int name_table_index = 0x2000;
    int q = ((scanline/8)*32) + nt_byte;
    int sprite_number = ppu_memory[name_table_index + q];
    draw_sprite_2(1, nt_byte*8,scanline,sprite_number,ppu_memory, pixel_buffer_2); 
  }
}

void updateRenderer_3(SDL_Renderer* rend, SDL_Texture* texture) {

    SDL_UpdateTexture(texture, NULL, pixel_buffer_2, 256 * sizeof(unsigned int));
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, texture, NULL, NULL);
    SDL_RenderPresent(rend);
}


void draw_chr_data(int i, int j, unsigned char* chr_data, unsigned int* pixel_buffer)
{
  unsigned char chr_data_1 = chr_data[0];
  unsigned char chr_data_2 = chr_data[8];
  for(int n=0; n<8; n++)
  {
    unsigned char data = (chr_data_1&1) | ((chr_data_2<<1)&3);
    unsigned int pixel_data= 0;
    switch(data)

    {
      case 0:
        pixel_data = 0xFFFF0000;
        break;
      case 1:
        pixel_data = 0xFF00FF00;
        break;
      case 2:
        pixel_data = 0xFF0000FF;
        break;
      case 3:
        pixel_data = 0xFFFF00FF;
        break;
    }
    pixel_buffer[(j*32*8) + i + 7 -n]=pixel_data;  
    chr_data_1>>=1;
    chr_data_2>>=1;
  }
}
