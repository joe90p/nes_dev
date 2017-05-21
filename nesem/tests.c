#include <stdio.h>
#include <stdlib.h>

void load_test_rom()
{
  unsigned char* cpu_memory = malloc(0x10000);
  //short rom_fileoffset=17;
  //char flags6 = ines_file_contents[6];
  /*if(flags6 & 8)
  {
    rom_fileoffset+=512;
  }i*/
  for(int i=0; i< 0x4000; i++)
  { 
    cpu_memory[0x8000 + i] = 0;//ines_file_contents[rom_fileoffset + i];
    cpu_memory[0xC000 + i] = 0;//ines_file_contents[rom_fileoffset + i];
  }
  cpu->cpu_memory=cpu_memory;
}

int main(int argc, char* argv[])
{
  cpu = malloc(sizeof(struct NES_CPU));
  load_test_rom();
  
}
