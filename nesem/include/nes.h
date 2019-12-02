#ifndef NES_H
#define NES_H

#include <stdio.h>
#include <stdlib.h>
#define PRG_ROM_SIZE 0x4000
#define CHR_ROM_SIZE 0x2000
struct NES_CPU
{
  unsigned char* cpu_memory;
  signed char A;
  unsigned char X;
  unsigned char Y;
  unsigned char status;
  unsigned short PC;
  unsigned char old_PC;
  unsigned char stack_pointer;
  unsigned int cycles;
}; 
struct NES_IO
{
  unsigned char controller1;
  unsigned char controller2;
};
struct NES_PPU
{
  unsigned char* ppu_memory;
  unsigned char* spr_ram;
  unsigned char status;
  unsigned char control;
};
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;
struct NES_PPU* ppu;
struct NES_IO* io;
typedef void (*opcode_action_type)(unsigned char a); 
void run_rom(char is_test);
typedef void (*opcode_action)(unsigned char* ptr);
typedef unsigned char* (*get_operand_ptr_type)();


void load_rom();
void increment_PC(signed char);

#endif
