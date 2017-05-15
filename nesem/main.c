#include <stdio.h>
#include <stdlib.h>
void load_rom();
unsigned char* ines_file_contents;
unsigned char* cpu_memory;
void run_rom();


int main(int argc, char* argv[])
{
  FILE* nes_file_ptr;
  int nes_file_length; 
  nes_file_ptr = fopen("/home/phil/test.nes", "r");
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

void load_rom()
{
  cpu_memory = malloc(0x10000);
  short rom_fileoffset=17;
  char flags6 = ines_file_contents[6];
  if(flags6 & 8)
  {
    rom_fileoffset+=512;
  }
  for(int i=0; i< 0x4000; i++)
  { 
    cpu_memory[0x8000 + i] = ines_file_contents[rom_fileoffset + i];
    cpu_memory[0xC000 + i] = ines_file_contents[rom_fileoffset + i];
  }
}

void run_rom()
{
  unsigned short program_counter = cpu_memory[0xfffc]<<8 | cpu_memory[0xfffd]>>8;
  printf("first 3 bytes at RESET\n"); 

  for(int j=0; j < 3; j++)
  {
    printf("%02x\n", cpu_memory[program_counter + j]);
  }

  for(int k=0; k < 5; k++)
  {
    switch(cpu_memory[program_counter])
    {
      case 0x00d8:
        printf("%02x: CLD\n", program_counter);
        program_counter++;
        break;
      case 0x00a2:
        printf("%02x: LDX %02x\n", program_counter, cpu_memory[program_counter+1]);
        program_counter+=2;
        break;
    } 
  } 
} 
   
