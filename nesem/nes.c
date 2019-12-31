#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ppu.h>
#include <time.h>
#include <cpu_addressing.h>
#include <cpu_instructions.h>
#include <opcode_init.h>
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;

struct opcode* opcodes;
struct address* addresses;
unsigned int master_cycle = 0;
unsigned int scanline = 0;
unsigned int cpu_cycle = 0;
void print_instruction_info_from_context(char program_counter_increment, char addressing_mode, unsigned char opcode)
{

    char* address_info = addresses[addressing_mode].address_info;
    char* address_mode_info = (char*)malloc(10 * sizeof(char));
 
    if(program_counter_increment == 1)
    {
       sprintf(address_mode_info,address_info );
    }
    if(program_counter_increment == 2)
    {
      sprintf(address_mode_info,address_info, cpu->cpu_memory[cpu->PC + 1]);
    }
    if(program_counter_increment == 3)
    {
      if(addressing_mode==12) // branch
      {
        sprintf(address_mode_info,address_info, addresses[addressing_mode].get_operand_ptr() - cpu->cpu_memory);
      }
      else
      {
        sprintf(address_mode_info,address_info, cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC + 1]);
      }
    }
    printf("%04X  %s %s\nA:%02X X:%02X Y:%02X P:%02X SP:%02X CYC:%3d\n", cpu->PC, opcodes[opcode].name, address_mode_info, (unsigned char)cpu->A, cpu->X, cpu->Y, cpu->status, cpu->stack_pointer, cpu_cycle);
    free(address_mode_info);

}

void increment_PC(signed char increment)
{
  cpu->PC+=increment;
}

void standard_instruction(unsigned char current_opcode, char is_test)
{
  unsigned char address_mode =  opcodes[current_opcode].address_mode;
  unsigned char* operand_ptr = addresses[address_mode].get_operand_ptr();
  char program_counter_increment = addresses[address_mode].program_counter_increment;
  if(is_test )
  {
    print_instruction_info_from_context( program_counter_increment,  address_mode, current_opcode);
  }
  opcodes[current_opcode].action(operand_ptr);
  if(addresses[address_mode].inc_pc)
  {
    increment_PC(program_counter_increment);
  }
  cpu->cycles+=(opcodes[current_opcode].cycles);
  cpu_cycle=(cpu_cycle + ((cpu->cycles)*3))%341;
  //cpu_cycle= (cpu_cycle > 341) ? cpu_cycle%341 : cpu_cycle;
  cpu->cycles=0;
}

void clear(char* pointer, int length)
{
  for(int i=0; i<length; i++)
  {
    pointer[i]=0;
  }
}

char starts_with(const char *pre, const char *str)
{
    size_t lenpre = strlen(pre),
           lenstr = strlen(str);
    return lenstr < lenpre ? 0 : strncmp(pre, str, lenpre) == 0;
}
void load_rom()
{
  unsigned char* cpu_memory = malloc(0x10000);
  unsigned char* ppu_memory = malloc(0x10000);
  unsigned char* spr_ram = malloc(0x100);
  int rom_fileoffset=16;
  char flags6 = ines_file_contents[6];
  char prg_rom_banks_num = ines_file_contents[4];
  char chr_rom_banks_num = ines_file_contents[5];
  if(flags6 & 8)
  {
    rom_fileoffset+=512;
  }
  for(int i=0; i< PRG_ROM_SIZE * prg_rom_banks_num; i++)
  { 
    cpu_memory[0x8000 + i] = ines_file_contents[rom_fileoffset + i];
    if(prg_rom_banks_num<=1)
    {
      cpu_memory[0xC000 + i] = ines_file_contents[rom_fileoffset + i];
    }
  }
  cpu->cpu_memory=cpu_memory;
  rom_fileoffset+=(prg_rom_banks_num * PRG_ROM_SIZE);
  for(int i=0; i< CHR_ROM_SIZE; i++)
  { 
    ppu_memory[0x0000 + i] = ines_file_contents[rom_fileoffset + i];
  }
  //initialise spr_ram
  for(int i=0; i< 256; i++)
  {
    spr_ram[i]=0;
  }
  ppu->ppu_memory=ppu_memory;
  ppu->spr_ram=spr_ram;
  //draw(ppu->ppu_memory, ppu->spr_ram, CHR_ROM_SIZE); 

}
void run_rom(char is_test)
{
  
  clock_t nmi_time = clock();
  SDL_Window* win =createWindow();
  SDL_Renderer* rend =createRenderer(win);
  SDL_Texture* text = createTexture(rend);
  set_opcodes(&opcodes, &addresses);
  if(is_test)
  {
    cpu->PC = 0xc000;
  }
  else
  {
    cpu->PC = get_short_from_cpu_memory(0xfffc); 
  }
  cpu->stack_pointer = 0xfd;
  cpu->status = 0x24;
  ppu->status = 0x00;
  cpu->A=0x00;
  for(int q=0;q<0x0100;q++)
  {
    cpu->cpu_memory[q]= 0x5B;
  }
  int run_instructions_no_prompt = 0;
  if(is_test)
  {
    run_instructions_no_prompt = 8991;
    //run_instructions_no_prompt = 9000;
  }
  char arg2 = ' ';
  int arg1 = 0;
  int draw_screen_count = 29606;
  unsigned short breakpoint = 0;
  //while(keepRunning(&(io->controller1))==1)
  while(1==1)
  {
    char input[20];
    char raw_input[20];
    clear(input, 20);
    if(run_instructions_no_prompt==0 || breakpoint==cpu->PC)
    { 
      if(is_test)
      {
        break;
      }
      while (strcmp("run", input))
      {
        printf("> ");
        fgets(raw_input, 20, stdin);
        sscanf(raw_input, "%s %x %c", input, &arg1, &arg2);
        if(strcmp("set", input)==0)
        {
          switch(arg2)
          {
            case 'x':
              cpu->X = (unsigned char)arg1; 
              break;
            case 'y':
              cpu->Y = (unsigned char)arg1; 
              break;
            case 'p':
              cpu->PC = (unsigned short)arg1; 
              break;
            case 's':
              cpu->status = (unsigned char)arg1; 
              break;
            case 'a':
              cpu->A = (unsigned char)arg1; 
              break;
            case '0':
              switch_status_flag(1,arg1);
              break;
            case '1':
              switch_status_flag(2,arg1);
              break;
            case '2':
              switch_status_flag(4,arg1);
               break;
            case '3':
              switch_status_flag(8,arg1);
              break;
            case '4':
              switch_status_flag(16,arg1);
              break;
            case '5':
              switch_status_flag(32,arg1);
              break;
            case '6':
              switch_status_flag(64,arg1);
              break;
            case '7':
              switch_status_flag(128,arg1);
              break;
          }
        }
        if(strcmp("print", input)==0)
        {
          printf("cpu->X = %d (%x), cpu->Y = %d (%x), cpu->A = %d (%x), cpu->PC = %d (%x), cpu->status = %d (%x), breakpoint = %d (%x), cpu->cpu_memory[%x] = %d (%x), ppu->status = %d (%x) \n", cpu->X, cpu->X, cpu->Y, cpu->Y, cpu->A, cpu->A, cpu->PC, cpu->PC, cpu->status, cpu->status, breakpoint, breakpoint, arg1, cpu->cpu_memory[arg1], cpu->cpu_memory[arg1], ppu->status, ppu->status);
          
        }
        if(strcmp("run", input)==0)
        {
          run_instructions_no_prompt=arg1;
        }
        if(strcmp("break", input)==0)
        {
          breakpoint=arg1;
        }
        
      }
      clear(input, 20 ); 
    }

    unsigned char current_opcode = cpu->cpu_memory[cpu->PC];
    if(master_cycle>=cpu_cycle)
    {
      standard_instruction(current_opcode, is_test);
      run_instructions_no_prompt--; 
    }
    if(scanline < 240)
    {
      updateRenderer_2(scanline, master_cycle, ppu->ppu_memory);
    }
    master_cycle++;
    draw_screen_count--; 
    if(master_cycle==341)
    {
      master_cycle=0;
      scanline++;
    }
    if(scanline==262)
    {
      scanline=0;
    }
    if(scanline==241 && master_cycle==1)
    {
      // V BLANK STARTS
      //keepRunning(&(io->controller1));   
      clock_t before = clock();
      //printf("trace: nmi_time %d\n", (clock() - nmi_time) * 1000 / CLOCKS_PER_SEC);
      updateRenderer_3(rend,text);
      clock_t after = (clock() - before) * 1000 / CLOCKS_PER_SEC;
      //printf("trace: updateRenderer time %d\n", after);
      nmi_time = clock();
      ppu->status|=128;
      if(ppu->control&128)
      {
        NMI();
      }
        
    }
    if(draw_screen_count==0)
    {
      if(keepRunning(&(io->controller1))==0)
      {
        break;
      }
      draw_screen_count=29606;
      ppu->status&=127;
    }
    
  } 
  // clean up resources before exiting
  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(win);
  SDL_Quit();
}

