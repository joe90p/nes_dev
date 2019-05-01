/**
 * hello3_image.c - Initializes SDL, loads an image, And displys it in a window
 */
#include </home/phil/git/nes_dev/nesem/sdl_test.h>
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
void updateRenderer(SDL_Renderer* rend, unsigned char* ppu_memory,unsigned char* sprite_data, int chr_length, SDL_Texture* texture) {
    unsigned int* pixel_buffer=malloc(32*30*8*sizeof(unsigned int));
    int row_length = 32;   
    int row_num = 30;
    for(int q=0; q<row_length*row_num; q++)
    {
     int col = q < row_length ? q : q%row_length; 
     int name_table_index = 0x2000;
     int sprite_number = ppu_memory[name_table_index + q];
     draw_sprite(1, col*8,(q/row_length)*8,sprite_number,ppu_memory, pixel_buffer); 
    }

    for(int i=0;i<64; i++)
    {
      unsigned char sprite_index = i *4;
      draw_sprite(0, sprite_data[sprite_index+3], sprite_data[sprite_index]+1, sprite_data[sprite_index+1], ppu_memory, pixel_buffer);
    }
    SDL_UpdateTexture(texture, NULL, pixel_buffer, 256 * sizeof(unsigned int));
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, texture, NULL, NULL);
    SDL_RenderPresent(rend);
    free(pixel_buffer);
    // wait a few seconds
    SDL_Delay(50);   
}

/*void draw(unsigned char* ppu_memory,unsigned char* sprite_data, int chr_length)
{
    SDL_Window* win = createWindow();
    SDL_Renderer* rend = createRenderer(win);
    updateRenderer(rend,ppu_memory,sprite_data,chr_length); 
    
    // clean up resources before exiting
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
}*/

void draw_sprite(int sprite_table, int i, int j, int sprite_number, unsigned char* ppu_memory, unsigned int* pixel_buffer)
{
  for(int m=0; m<8; m++)
  {
     int sprite_index_offset = sprite_table * 0x1000;
     int sprite_index = sprite_index_offset + (sprite_number*16) + m;
     unsigned char* chr_data = &(ppu_memory[sprite_index]);
     draw_chr_data(i, j +m, chr_data, pixel_buffer);
  }
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
        pixel_data = 0xFF0000FF;
        break;
      case 1:
        pixel_data = 0x00FF00FF;
        break;
      case 2:
        pixel_data = 0x0000FFFF;
        break;
      case 3:
        pixel_data = 0xFF00FFFF;
        break;
    }
    pixel_buffer[(j*32*8) + i + 7 -n]=pixel_data;  
    chr_data_1>>=1;
    chr_data_2>>=1;
  }
}
