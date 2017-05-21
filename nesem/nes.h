#include <stdio.h>
#include <stdlib.h>

struct NES_CPU
{
  unsigned char* cpu_memory;
  unsigned char A;
  unsigned char X;
  unsigned char Y;
  unsigned char status;
  unsigned short PC;
}; 
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;

void run_rom();



