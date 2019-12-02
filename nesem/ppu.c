
#include <ppu.h>

void setController()
{
  SDL_PumpEvents();
  const Uint8* state = SDL_GetKeyboardState(NULL);
  io->controller1=0;
  if(state[SDL_SCANCODE_K])
  {
    io->controller1|=1;
  }
  if(state[SDL_SCANCODE_L])
  {
    io->controller1|=2;
  }
  if(state[SDL_SCANCODE_G])
  {
    io->controller1|=4;
  }
  if(state[SDL_SCANCODE_H])
  {
    io->controller1|=8;
  }
  if(state[SDL_SCANCODE_W])
  {
    io->controller1|=16;
  }
  if(state[SDL_SCANCODE_S])
  {
    io->controller1|=32;
  }
  if(state[SDL_SCANCODE_A])
  {
    io->controller1|=64;
  }
  if(state[SDL_SCANCODE_D])
  {
    io->controller1|=128;
  }
}
char keepRunning(char* controller)
{
  SDL_Event event;
  SDL_PollEvent(&event);
  //*controller=8;
  //*controller=0;
  switch(event.type) {
    case SDL_QUIT:
      return 0;
    /*case SDL_KEYDOWN:
      switch(event.key.keysym.sym)
      {
        case SDLK_w:
           *controller=(*controller)|8;
           *controller=8;
          break;  
      }  
      return 1;*/
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
unsigned int* nescolormap;

void setnescolormap(unsigned int** nescolormap)
{
  (*nescolormap) = malloc(64 * sizeof(unsigned int));
  (*nescolormap)[0] = 0xff757575;
  (*nescolormap)[1] = 0xff271b8f;
  (*nescolormap)[2] = 0xff0000ab;
  (*nescolormap)[3] = 0xff47009f;
  (*nescolormap)[4] = 0xff8f0077;
  (*nescolormap)[5] = 0xffab0013;
  (*nescolormap)[6] = 0xffa70000;
  (*nescolormap)[7] = 0xff7f0b00;
  (*nescolormap)[8] = 0xff432f00;
  (*nescolormap)[9] = 0xff004700;
  (*nescolormap)[0x0a] = 0xff005100;
  (*nescolormap)[0x0b] = 0xff003f17;
  (*nescolormap)[0x0c] = 0xff1b3f5f;
  (*nescolormap)[0x0d] = 0xff000000;
  (*nescolormap)[0x0e] = 0xff000000;
  (*nescolormap)[0x0f] = 0xff000000;
  (*nescolormap)[0x10] = 0xffbcbcbc;
  (*nescolormap)[0x11] = 0xff0073ef;
  (*nescolormap)[0x12] = 0xff233bef;
  (*nescolormap)[0x13] = 0xff8300f3;
  (*nescolormap)[0x14] = 0xffbf00bf;
  (*nescolormap)[0x15] = 0xffe7005b;
  (*nescolormap)[0x16] = 0xffdb2b00;
  (*nescolormap)[0x17] = 0xffcb4f0f;
  (*nescolormap)[0x18] = 0xff8b7300;
  (*nescolormap)[0x19] = 0xff009700;
  (*nescolormap)[0x1a] = 0xff00ab00;
  (*nescolormap)[0x1b] = 0xff00933b;
  (*nescolormap)[0x1c] = 0xff00838b;
  (*nescolormap)[0x1d] = 0xff000000;
  (*nescolormap)[0x1e] = 0xff000000;
  (*nescolormap)[0x1f] = 0xff000000;
  (*nescolormap)[0x20] = 0xffffffff;
  (*nescolormap)[0x21] = 0xff3fbfff;
  (*nescolormap)[0x22] = 0xff5f97ff;
  (*nescolormap)[0x23] = 0xffa78bfd;
  (*nescolormap)[0x24] = 0xfff77bff;
  (*nescolormap)[0x25] = 0xffff77b7;
  (*nescolormap)[0x26] = 0xffff7763;
  (*nescolormap)[0x27] = 0xffff9b3b;
  (*nescolormap)[0x28] = 0xfff3bf3f;
  (*nescolormap)[0x29] = 0xff83d313;
  (*nescolormap)[0x2a] = 0xff4fdf4b;
  (*nescolormap)[0x2b] = 0xff58f898;
  (*nescolormap)[0x2c] = 0xff00ebdb;
  (*nescolormap)[0x2d] = 0xff000000;
  (*nescolormap)[0x2e] = 0xff000000;
  (*nescolormap)[0x2f] = 0xff000000;
  (*nescolormap)[0x30] = 0xffffffff;
  (*nescolormap)[0x31] = 0xffabe7ff;
  (*nescolormap)[0x32] = 0xffc7d7ff;
  (*nescolormap)[0x33] = 0xffd7cbff;
  (*nescolormap)[0x34] = 0xffffc7ff;
  (*nescolormap)[0x35] = 0xffffc7db;
  (*nescolormap)[0x36] = 0xffffbfb3;
  (*nescolormap)[0x37] = 0xffffdbab;
  (*nescolormap)[0x38] = 0xffffe7a3;
  (*nescolormap)[0x39] = 0xffe3ffa3;
  (*nescolormap)[0x3a] = 0xffabf3bf;
  (*nescolormap)[0x3b] = 0xffb3ffcf;
  (*nescolormap)[0x3c] = 0xff9ffff3;
  (*nescolormap)[0x3d] = 0xff000000;
  (*nescolormap)[0x3e] = 0xff000000;
  (*nescolormap)[0x3f] = 0xff000000;

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
    setnescolormap(&nescolormap);
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
void draw_sprite_2(int sprite_table, int i, int j, int sprite_number, unsigned char* ppu_memory, unsigned int* pixel_buffer, unsigned char section, unsigned char pallette, unsigned char sprite_attribute)
{
     int sprite_index_offset = sprite_table * 0x1000;
     int sprite_index = sprite_index_offset + (sprite_number*16) + section;
     unsigned char* chr_data = &(ppu_memory[sprite_index]);
     draw_chr_data(i, j, chr_data, pixel_buffer, pallette, sprite_attribute);
}

void updateRenderer_2(int scanline, int ppu_cycle,unsigned char* ppu_memory){
  unsigned char* sprite_data = ppu->spr_ram;
  unsigned char ppu_status = cpu->cpu_memory[0x2000];
  if(ppu_cycle <=257  )
  {
    if(((ppu_cycle-1)%8)==0)
    {
      int nt_byte = (ppu_cycle - 1) /8; 
      int name_table_index = 0x2000;
      int q = ((scanline/8)*32) + nt_byte;
      int sprite_number = ppu_memory[name_table_index + q];
      unsigned char attribute_index = ((ppu_cycle-1)/32) + 8*(scanline/32);
      unsigned char quadrant = ((((ppu_cycle-1)/16)+2)%2+(2*(((scanline/16)+2)%2)))*2;
      unsigned char attribute_byte = ppu_memory[0x23c0 + attribute_index];
      unsigned char mask = 3<<quadrant;
      unsigned char pallette = ((((attribute_byte&mask)>>quadrant)&3)<<2);
      draw_sprite_2((ppu_status&16)>>4, nt_byte*8,scanline,sprite_number,ppu_memory, pixel_buffer_2,scanline%8, pallette, 0);
      
    }

  }
  if(ppu_cycle==258)
  {
    for(int i=0;i<64; i++)
    {
      unsigned char sprite_index = i *4;
      if(scanline>=sprite_data[sprite_index]+1 && scanline<=sprite_data[sprite_index]+8 )
      {
        unsigned char section = scanline -  sprite_data[sprite_index] -1;
        unsigned char sprite_attribute =  sprite_data[sprite_index+2];
        unsigned char pallette = ((sprite_attribute&3)<<2)+16;
        draw_sprite_2((ppu_status&8)>>3, sprite_data[sprite_index+3], scanline, sprite_data[sprite_index+1], ppu_memory, pixel_buffer_2,section, pallette, sprite_attribute);
      }

    }
  }
}

void updateRenderer_3(SDL_Renderer* rend, SDL_Texture* texture) {

    SDL_UpdateTexture(texture, NULL, pixel_buffer_2, 256 * sizeof(unsigned int));
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, texture, NULL, NULL);
    SDL_RenderPresent(rend);
}



void draw_chr_data(int i, int j, unsigned char* chr_data, unsigned int* pixel_buffer, unsigned char pallette, unsigned char sprite_attribute)
{
  unsigned char chr_data_1 = chr_data[0];
  unsigned char chr_data_2 = chr_data[8];
  
  for(int n=0; n<8; n++)
  {
    unsigned char data = (chr_data_1&1) | ((chr_data_2<<1)&3);
    unsigned int pixel_data= 0;
    if(!(pallette&16 && data==0))
    {
      unsigned short palette_offset = pallette + data;
      unsigned short address = (palette_offset%4==0) ? 0x3f00 : (0x3f00 + palette_offset );
      unsigned char nes_color = ppu->ppu_memory[address];
      pixel_data = nescolormap[nes_color];
      unsigned char pos = sprite_attribute&64 ? n : 7 -n;
      pixel_buffer[(j*32*8) + i + pos]=pixel_data;  
    }
        chr_data_1>>=1;
    chr_data_2>>=1;
  }
}
