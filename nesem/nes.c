#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include </home/phil/git/nes_dev/nesem/ppu.h>
#include <time.h>
#include </home/phil/git/nes_dev/nesem/cpu_addressing.h>
#include </home/phil/git/nes_dev/nesem/cpu_instructions.h>
#include </home/phil/git/nes_dev/nesem/opcode_init.h>
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;
void print_instruction_info(char program_counter_increment, char* address_info, char* opcode_info);

void run_rom();

struct opcode* opcodes;
struct address* addresses;

//void set_opcodes(opcodes, addresses);

void print_instruction_info(char program_counter_increment, char* address_info, char* opcode_info)
{
/*
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
      sprintf(address_mode_info,address_info, cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC + 1]);
    }
    printf("%02X %s %s\nA:%X  X:%X  Y:%X  P:%X  SP:%X  \n", cpu->PC, opcode_info, address_mode_info, (unsigned char)cpu->A, cpu->X, cpu->Y, cpu->status, cpu->stack_pointer);
    free(address_mode_info);
*/
}

void print_instruction_info_from_context(char program_counter_increment, char addressing_mode, unsigned char opcode)
{
    char* address_info = addresses[addressing_mode].address_info;
    print_instruction_info(program_counter_increment, address_info, opcodes[opcode].name);
}

void conditional_branch_instruction(unsigned char current_opcode)
{
  unsigned char branch_context = (current_opcode&192)>>6;
  unsigned char equalTo = (current_opcode&32)>>5;

  unsigned char offset = cpu->cpu_memory[cpu->PC+1];
  char* opcode_info;
  char flag;
  switch(branch_context)
  {
    case 0:
      flag=NES_NEGATIVE_FLAG;
      opcode_info=equalTo ? "BMI" : "BPL";
      break;
    case 1:
      flag=NES_OVERFLOW_FLAG;
      opcode_info=equalTo ? "BVS" : "BVC";
      break;
    case 2:
      flag=NES_CARRY_FLAG;
      opcode_info=equalTo ? "BCS" : "BCC";
      break;
    case 3:
      flag=NES_ZERO_FLAG;
      opcode_info=equalTo ? "BEQ" : "BNE";
      break;
  }
  print_instruction_info(2, "#%02x", opcode_info);
  if(!test_flag_and_branch(flag, equalTo, offset))
  {
    increment_PC(2);
  }
}

void increment_PC(signed char increment)
{
  cpu->PC+=increment;
}

void standard_instruction(unsigned char current_opcode)
{
  unsigned char address_mode =  opcodes[current_opcode].address_mode;
  unsigned char* operand_ptr = addresses[address_mode].get_operand_ptr();
  char program_counter_increment = addresses[address_mode].program_counter_increment;
  print_instruction_info_from_context( program_counter_increment,  address_mode, current_opcode);
  opcodes[current_opcode].action(operand_ptr);
  if(addresses[address_mode].inc_pc)
  {
    increment_PC(program_counter_increment);
  }
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
void run_rom()
{
  
  clock_t nmi_time = clock();
  SDL_Window* win =createWindow();
  SDL_Renderer* rend =createRenderer(win);
  SDL_Texture* text = createTexture(rend);
  set_opcodes(&opcodes, &addresses);
  cpu->PC = get_short_from_cpu_memory(0xfffc); 
  //cpu->PC = 0xc000;
  cpu->stack_pointer = 0xfd;
  cpu->status = 0x24;
  ppu->status = 0x00;
  cpu->A=0xfe;
  for(int q=0;q<0x0100;q++)
  {
    cpu->cpu_memory[q]= 0x5B;
  }
  int run_instructions_no_prompt = 0;
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
      run_instructions_no_prompt--; 
      clear(input, 20 ); 
    }
    else
    {
      run_instructions_no_prompt--;
    }
    unsigned char current_opcode = cpu->cpu_memory[cpu->PC];
    standard_instruction(current_opcode);
    draw_screen_count--; 
    if(draw_screen_count==2260)
    {
      // V BLANK STARTS
      //keepRunning(&(io->controller1));   
      clock_t before = clock();
      printf("trace: nmi_time %d\n", (clock() - nmi_time) * 1000 / CLOCKS_PER_SEC);
      updateRenderer(rend,ppu->ppu_memory,ppu->spr_ram,CHR_ROM_SIZE, text);
      clock_t after = (clock() - before) * 1000 / CLOCKS_PER_SEC;
      printf("trace: updateRenderer time %d\n", after);
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

