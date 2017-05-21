#include <stdio.h>
#include <stdlib.h>
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;

void run_rom();


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

void ADC_update_status_register(unsigned char oldA)
{
  cpu->status|=cpu->A&128;
  cpu->status|=((cpu->A&128)^(oldA&128))>>1;
  cpu->status|=(cpu->A==0)<<1;
  cpu->status|=((cpu->A&128)^(oldA&128))>>7;

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
  unsigned char indir_address;
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
        cpu->A+=cpu->cpu_memory[cpu->cpu_memory[cpu->PC+1]];
        ADC_update_status_register(oldA);
        cpu->PC+=2;
        break;
      case 0x0075:
        printf("%02x: ADC $%02x,X\n", cpu->PC, cpu->cpu_memory[cpu->PC+1]); 
        cpu->A+=cpu->cpu_memory[cpu->cpu_memory[cpu->PC+1]+cpu->X];
        ADC_update_status_register(oldA);
        cpu->PC+=2;
        break;
     case 0x006d:
        printf("%02x: ADC $%02x%02x\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);
        address = cpu->cpu_memory[cpu->PC+2]<<8 | cpu->cpu_memory[cpu->PC+1];
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);
        cpu->PC+=3;
        break;
      case 0x007d:
        printf("%02x: ADC $%02x%02x,X\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);
        address = (cpu->cpu_memory[cpu->PC+2]<<8 | cpu->cpu_memory[cpu->PC+1]) + cpu->X;
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=3;
        break; 
      case 0x0079:
        printf("%02x: ADC $%02x%02x,Y\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);
        address = (cpu->cpu_memory[cpu->PC+2]<<8 | cpu->cpu_memory[cpu->PC+1]) + cpu->Y;
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=3;
        break;
      case 0x0061:
        printf("%02x: ADC $%02x%02x,Y\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]);        
        indir_address = cpu->cpu_memory[cpu->PC+1 + cpu->X];
        address = cpu->cpu_memory[indir_address + 1] << 8 | cpu->cpu_memory[indir_address];
        cpu->A+=cpu->cpu_memory[address];
        ADC_update_status_register(oldA);   
        cpu->PC+=2;
        break;
      case 0x0071:
        printf("%02x: ADC $%02x%02x,Y\n", cpu->PC, cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC+1]); 
        indir_address = cpu->cpu_memory[cpu->PC+1] + cpu->Y;
        address = cpu->cpu_memory[indir_address + 1] << 8 | cpu->cpu_memory[indir_address];
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
