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
  unsigned char x_tile;
  unsigned char x_fine;
  unsigned char y_tile;
  unsigned char y_fine;
  unsigned char mask;
  unsigned char nt;
};
void load_rom();
extern unsigned char* ines_file_contents;
extern struct NES_CPU* cpu;
extern struct NES_PPU* ppu;
extern struct NES_IO* io;
typedef void (*opcode_action_type)(unsigned char a); 
void run_rom(char is_test);
typedef void (*opcode_action)(unsigned char* ptr);
typedef unsigned char* (*get_operand_ptr_type)();


void load_rom();
void increment_PC(signed char);

#endif
