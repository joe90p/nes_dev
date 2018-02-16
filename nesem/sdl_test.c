/**
 * hello3_image.c - Initializes SDL, loads an image, And displys it in a window
 */
#include </home/phil/git/nes_dev/nesem/sdl_test.h>
void draw(unsigned char* letter_a, int chr_length)
{
    // attempt to initialize graphics and timer system
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }
    Uint32 window_flags = SDL_WINDOW_RESIZABLE|SDL_WINDOW_FULLSCREEN_DESKTOP;
    SDL_Window* win = SDL_CreateWindow("Hello, CS50!",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       256, 240, window_flags);
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
    SDL_RenderSetLogicalSize(rend, 256, 240); 
    //SDL_RenderSetScale(rend, 3, 3);
    // load the image into memory using SDL_image library function
    /*SDL_Surface* surface;// = IMG_Load("resources/hello.png");
    surface = SDL_CreateRGBSurface(0, 3, 3, 32, 0, 0, 0, 0);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 255, 0, 0));
    if (!surface)
    {
        printf("error creating surface\n");
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // load the image data into the graphics hardware's memory
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);
    if (!tex)
    {
        printf("error creating texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(rend);
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    // clear the window
    SDL_RenderClear(rend);
    
    // draw the image to the window
    SDL_RenderCopy(rend, tex, NULL, NULL);
    SDL_RenderPresent(rend);*/

    SDL_SetRenderDrawColor( rend, 255, 0, 0, 255);

    SDL_RenderClear(rend);
    /*unsigned char* letter_a = malloc(32);
    letter_a[0] = 16;
    letter_a[1] = 0;
    letter_a[2] = 68;
    letter_a[3] = 0;
    letter_a[4] = 254;
    letter_a[5] = 0;
    letter_a[6] = 130;
    letter_a[7] = 0;
    letter_a[8] = 0;
    letter_a[9] = 40;
    letter_a[10] = 68;
    letter_a[11] = 130;
    letter_a[12] = 0;
    letter_a[13] = 130;
    letter_a[14] = 130;
    letter_a[15] = 0;
    letter_a[0] = 128;
    letter_a[1] = 128;
    letter_a[2] = 128;
    letter_a[3] = 128;
    letter_a[4] = 128;
    letter_a[5] = 128;
    letter_a[6] = 128;
    letter_a[7] = 128;
    letter_a[8] = 128;
    letter_a[9] = 128;
    letter_a[10] = 128;
    letter_a[11] = 128;
    letter_a[12] = 128;
    letter_a[13] = 128;
    letter_a[14] = 128;
    letter_a[15] = 128;
    letter_a[16] = 128;
    letter_a[17] = 128;
    letter_a[18] = 128;
    letter_a[19] = 128;
    letter_a[20] = 128;
    letter_a[21] = 128;
    letter_a[22] = 128;
    letter_a[23] = 128;
    letter_a[24] = 128;
    letter_a[25] = 128;
    letter_a[26] = 128;
    letter_a[27] = 128;
    letter_a[28] = 128;
    letter_a[29] = 128;
    letter_a[30] = 128;
    letter_a[31] = 128;*/


    //for(int i=0; i<32; i++)
    //{  
      //for(int j=0; j<30; j++)
      //{
   
       // SDL_Rect rect;
        //rect.x = (i * 8);
        //rect.y = (j * 8);

        //rect.w = 8;
        //rect.h = 8;

        
        //SDL_RenderDrawRect(rend, &rect);
int j = 1;
int i = 0;
int max = chr_length/16;
//int z = 1;
        for(int z=0; z<max; z++)
        {
          for(int m=0; m<8; m++)
          {
            unsigned char* letter_part = &(letter_a[(z*16) + m]);
            draw_chr_data(i*8, (j*8) +m, letter_part, rend);
          }
          i++;
          if(i==16)
          {
            i=0;
            j++;
          }
        }

        
      //}
    //}  
    //SDL_RenderSetViewport(rend, NULL);
    SDL_RenderPresent(rend);
    // wait a few seconds
    SDL_Delay(5000);
    
    // clean up resources before exiting
    //SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
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
    SDL_RenderDrawPoint(rend, i + n,j );
    chr_data_1>>=1;
    chr_data_2>>=1;
  }
}
