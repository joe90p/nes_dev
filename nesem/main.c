#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
  cpu = malloc(sizeof(struct NES_CPU)); 
  ppu = malloc(sizeof(struct NES_PPU)); 
  FILE* nes_file_ptr;
  int nes_file_length; 
  nes_file_ptr = fopen(argv[1], "r");
  fseek(nes_file_ptr,0,SEEK_END);
  nes_file_length = ftell(nes_file_ptr);  
  ines_file_contents = malloc(nes_file_length);
  fseek(nes_file_ptr,0,SEEK_SET);
  fread(ines_file_contents, 1, nes_file_length, nes_file_ptr); 
  
  fclose(nes_file_ptr); 
  load_rom();
  run_rom();
  return 0;
}

