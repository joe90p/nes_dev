#include <stdio.h>
#include <stdlib.h>
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;

void run_rom();

unsigned short get_absolute_address(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_address_input_upper_byte<<8 | get_address_input_lower_byte;
}

unsigned short get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_absolute_address(get_address_input_upper_byte, get_address_input_lower_byte)  + cpu->X;
}

unsigned short get_zeropage_address(unsigned char get_address_input)
{
  return get_absolute_address(0x00, get_address_input);
}

unsigned short get_zeropage_X_address(unsigned char get_address_input)
{
  return get_absolute_address_X(0x00, get_address_input);
}

unsigned short get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_absolute_address(get_address_input_upper_byte, get_address_input_lower_byte)  + cpu->Y;
}

unsigned short get_indexed_indirect_X(unsigned char get_address_input)
{
  unsigned char indir_address = get_address_input + cpu->X; 
  return get_absolute_address(cpu->cpu_memory[indir_address + 1], cpu->cpu_memory[indir_address]);
}

unsigned short get_indirect_indexed_Y(unsigned char get_address_input)
{ 
  return get_absolute_address(cpu->cpu_memory[get_address_input + 1], cpu->cpu_memory[get_address_input]) + cpu->Y;
}

void load_rom()
{
  unsigned char* cpu_memory = malloc(0x10000);
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
  cpu->cpu_memory=cpu_memory;
}

void switch_status_flag(char flag, char switch_on)
{
  if(switch_on)
  {
    cpu->status|=flag;
  }
  else
  {
    cpu->status&=~flag;
  }
}

void ADC_update_status_register(unsigned char oldA)
{
  char carry_flag=1;
  char zero_flag=2;
  char overflow_flag=64;
  char negative_flag=128;

  switch_status_flag(carry_flag,cpu->A < oldA);
  switch_status_flag(zero_flag,cpu->A == 0);
  switch_status_flag(negative_flag,cpu->A < 0);
  switch_status_flag(overflow_flag,cpu->A < oldA); 
}

void run_rom()
{
  cpu->PC = cpu->cpu_memory[0xfffc]<<8 | cpu->cpu_memory[0xfffd]>>8;
  printf("first 3 bytes at RESET\n"); 

  for(int j=0; j < 3; j++)
  {
    printf("%02x\n", cpu->cpu_memory[cpu->PC + j]);
  }
  unsigned char oldA;
  unsigned short address;
  for(int k=0; k < 5; k++)
  {
    oldA = cpu->A;

    switch(cpu->cpu_memory[cpu->PC])
    {
      case 0x0069:
        printf("%02x: ADC #$%02x\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]); 
        cpu->A+=cpu->cpu_memory[cpu->PC+1];
        ADC_update_status_register(oldA);
        cpu->PC+=2;
        break;
      case 0x0065:
        printf("%02x: ADC $%02x\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]); 
        address = get_zeropage_address(cpu->cpu_memory[cpu->PC +1]);
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);
        cpu->PC+=2;
        break;
      case 0x0075:
        printf("%02x: ADC $%02x,X\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]); 
        address = get_zeropage_X_address(cpu->cpu_memory[cpu->PC + 1]);
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);
        cpu->PC+=2;
        break;
     case 0x006d:
        printf("%02x: ADC $%02x%02x\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);
        address = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);
        cpu->PC+=3;
        break;
      case 0x007d:
        printf("%02x: ADC $%02x%02x,X\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);
        address = get_absolute_address_X(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=3;
        break; 
      case 0x0079:
        printf("%02x: ADC $%02x%02x,Y\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);
        address = get_absolute_address_Y(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=3;
        break;
      case 0x0061:
        printf("%02x: ADC ($%02x,X)\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]);        
        address = get_indexed_indirect_X(cpu->cpu_memory[cpu->PC + 1]);
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=2;
        break;
      case 0x0071:
        printf("%02x: ADC ($%02x),Y\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]); 
        address = get_indirect_indexed_Y(cpu->cpu_memory[cpu->PC + 1]);        
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=2;
        break;
      case 0x00d8:
        printf("%02x: CLD\n", cpu->PC);
        cpu->PC++;
        break;
      case 0x00a2:
        printf("%02x: LDX %02x\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]);
        cpu->X=cpu->cpu_memory[cpu->PC+1];
        cpu->PC+=2;
        break;
    } 
  } 
}

