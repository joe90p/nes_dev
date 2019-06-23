#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include </home/phil/git/nes_dev/nesem/sdl_test.h>
#include <time.h>

void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;
void print_instruction_info(char program_counter_increment, char* address_info, char* opcode_info);

void run_rom();
static unsigned char read_controller_reset_await = 0;
static unsigned char controller_read = 0;
static unsigned short ppu_write_address;
static unsigned char ppu_read_buffer = 0;
static unsigned char oam_addr;
static unsigned char cpu_sprite_copy_address_high;
unsigned char* get_immediate_operand_ptr()
{
  return &cpu->cpu_memory[cpu->PC + 1];
}

unsigned char* get_zeropage_operand_ptr()
{
  unsigned char address = get_zeropage_address(cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[address];
}         

unsigned short get_short_from_chars(unsigned char high_byte, unsigned char low_byte)
{
  return ((high_byte<<8) | low_byte);
}

unsigned char* get_accumulator_operand_ptr()
{
  return (unsigned char*)&cpu->A;
}

unsigned char* get_absolute_operand_ptr()
{
  unsigned short address = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  return &cpu->cpu_memory[address];
}

unsigned char* get_zeropage_X_operand_ptr()
{
  unsigned char address = get_zeropage_X_address(cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[address];
}

unsigned char* get_zeropage_Y_operand_ptr()
{
  unsigned char address = get_zeropage_Y_address(cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[address];
}

unsigned char* get_dummy_operand_ptr()
{
  return &cpu->cpu_memory[0];
}

unsigned char* get_absolute_X_operand_ptr()
{
  unsigned short address = get_absolute_address_X(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  return &cpu->cpu_memory[address];
}

unsigned char* get_absolute_Y_operand_ptr()
{
  unsigned short address = get_absolute_address_Y(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  return &cpu->cpu_memory[address];
}

unsigned char* get_indexed_indirect_X_operand_ptr()
{
  unsigned short address = get_indexed_indirect_X(cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[address];
}

unsigned char* get_indirect_indexed_Y_operand_ptr()
{
  unsigned short intermediate_address = get_absolute_address(0, cpu->cpu_memory[cpu->PC + 1]);
  unsigned short address = get_indirect_indexed_Y(intermediate_address);
  return &cpu->cpu_memory[address];
}

unsigned short get_absolute_address(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_address_input_upper_byte<<8 | get_address_input_lower_byte;
}

unsigned short get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_absolute_address(get_address_input_upper_byte, get_address_input_lower_byte)  + cpu->X;
}

unsigned short get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_absolute_address(get_address_input_upper_byte, get_address_input_lower_byte)  + cpu->Y;
}

unsigned short get_zeropage_address(unsigned char get_address_input)
{
  return get_absolute_address(0x00, get_address_input);
}

unsigned short get_zeropage_X_address(unsigned char get_address_input)
{
  return get_absolute_address_X(0x00, get_address_input);
}

unsigned short get_zeropage_Y_address(unsigned char get_address_input)
{
  return get_absolute_address_Y(0x00, get_address_input);
}

unsigned short get_indexed_indirect_X(unsigned char get_address_input)
{
  unsigned char indir_address = get_address_input + cpu->X; 
  unsigned char indir_address_high = indir_address + 1;
  return get_absolute_address(cpu->cpu_memory[indir_address_high], cpu->cpu_memory[indir_address]);
}

unsigned short get_indirect(unsigned short get_address_input)
{ 
  unsigned short address_high = get_address_input + 1;
  if(get_address_input&0x00ff==0x00ff)
  {
    address_high=get_address_input&0xff00;
  }
   
  return get_absolute_address(cpu->cpu_memory[address_high ], cpu->cpu_memory[get_address_input]);

}

unsigned short get_indirect_indexed_Y(unsigned short get_address_input)
{ 
  unsigned char address_input_high = (unsigned char)get_address_input + 1;
  return get_absolute_address(cpu->cpu_memory[address_input_high ], cpu->cpu_memory[get_address_input]) + cpu->Y;
  //return get_indirect_indexed(get_address_input, cpu->Y);
}

void set_ppu_write_address(unsigned short address)
{
  ppu_write_address = address;
}

unsigned short get_ppu_write_address()
{
  return ppu_write_address;
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

void ADC_update_status_register(signed char oldA, unsigned char toAdd)
{ 

  unsigned char old_carry = cpu->status&1;
  unsigned char oldA_sign_bit = oldA&128;
  //unsigned char newA_sign_bit = (cpu->A)&128;
  unsigned char toAdd_sign_bit = toAdd&128;
  //unsigned char overflow = (oldA_sign_bit==toAdd_sign_bit) && newA_sign_bit!=oldA_sign_bit;
  //switch_status_flag(NES_CARRY_FLAG, cpu->A!=oldA && ((cpu->A<oldA && newA_sign_bit==oldA_sign_bit) || (cpu->A>=oldA && newA_sign_bit!=oldA_sign_bit)));   
  unsigned char oldA_u = oldA;
  unsigned char has_carry_6 = ((oldA_u&127) + (toAdd&127) + (cpu->status&1))&128;
  unsigned char has_carry_7 = ((has_carry_6 && (oldA_sign_bit||toAdd_sign_bit)) || (!has_carry_6 && (oldA_sign_bit && toAdd_sign_bit)))<<7;
  unsigned char overflow = has_carry_6^has_carry_7;
  switch_status_flag(NES_CARRY_FLAG, has_carry_7);
  //unsigned char old oldA_u&128;
  switch_status_flag(NES_ZERO_FLAG,cpu->A == 0);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->A < 0);
  switch_status_flag(NES_OVERFLOW_FLAG,overflow); 
}

void SBC_update_status_register(signed char oldA)
{
  char oldA_sign_bit = oldA&128;
  char newA_sign_bit = (cpu->A)&128;
  switch_status_flag(NES_CARRY_FLAG,cpu->A!=oldA && ((cpu->A<oldA && newA_sign_bit!=oldA_sign_bit) || (cpu->A>=oldA && newA_sign_bit==oldA_sign_bit))); 
  switch_status_flag(NES_ZERO_FLAG,cpu->A == 0);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->A < 0);
  switch_status_flag(NES_OVERFLOW_FLAG,cpu->A > oldA); 
}

void CMP_update_status_register(unsigned char data)
{
  unsigned char a = (unsigned char)cpu->A;
  switch_status_flag(NES_CARRY_FLAG,a >= data);
  switch_status_flag(NES_ZERO_FLAG,a == data);
  switch_status_flag(NES_NEGATIVE_FLAG, (a-data)&128);
}
void CPY_update_status_register(unsigned char data)
{
  switch_status_flag(NES_CARRY_FLAG,cpu->Y >= data);
  switch_status_flag(NES_ZERO_FLAG,cpu->Y == data);
  switch_status_flag(NES_NEGATIVE_FLAG,(cpu->Y - data)&128); 
}
void CPX_update_status_register(unsigned char data)
{
  switch_status_flag(NES_CARRY_FLAG,cpu->X >= data);
  switch_status_flag(NES_ZERO_FLAG,cpu->X == data);
  switch_status_flag(NES_NEGATIVE_FLAG,(cpu->X-  data)&128); 
}
//also applies to
//AND
//EOR
//LDA
void ORA_update_status_register()
{ 
  switch_status_flag(NES_ZERO_FLAG,cpu->A == 0);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->A < 0);
}


void ORA_ptr(unsigned char* toOr)
{
  ORA(*toOr);  
}

void AND_ptr(unsigned char* toOr)
{
  AND(*toOr);  
}

void EOR_ptr(unsigned char* toOr)
{
  EOR(*toOr);  
}

void ADC_ptr(unsigned char* toOr)
{
  ADC(*toOr);  
}

void STA_ptr(unsigned char* toOr)
{
  unsigned short address = toOr - cpu->cpu_memory; 

  STA(address);  
}


unsigned char get_value(unsigned char* operand_ptr, unsigned char peek)
{
  unsigned short address = operand_ptr - cpu->cpu_memory;
  
  unsigned char value_to_load = *operand_ptr;
  if(address==0x4016)
  {
    value_to_load = controller_read&1;
    if(!peek)
    {
      controller_read=controller_read>>1;
    }

  }
  if(address==0x2002)
  {
    value_to_load = ppu->status;
    if(!peek)
    {
      ppu->status&=127;
      ppu_write_address = 0;
    }
  }
  if(address==PPU_CONTROL_CPU_ADDRESS)
  {
    value_to_load = ppu->control;
  }
  if(address==0x2004)
  {
    value_to_load = ppu->spr_ram[oam_addr];
  }
  if(address==0x2007)
  {
      unsigned char toReturn = ppu_read_buffer;
      if(ppu_write_address<0x3f00)
      {
        ppu_read_buffer = ppu->ppu_memory[ppu_write_address];
      }
      else
      {
        ppu_read_buffer = ppu->ppu_memory[ppu_write_address - 0x1000];
      }
      return toReturn;  
  }
  return value_to_load;
}
unsigned char peek_value(unsigned char* operand_ptr)
{
  get_value(operand_ptr, 1); 
}
unsigned char get_value_to_load(unsigned char* operand_ptr)
{
  get_value(operand_ptr, 0); 
}
void LDA_ptr(unsigned char* toOr)
{
  /*unsigned short address = toOr - cpu->cpu_memory;
  
  unsigned char value_to_load = *toOr;
  if(address==0x4016)
  {
    //value_to_load = io->controller1&1;
    //io->controller1=io->controller1>>1;
    value_to_load = controller_read&1;
    controller_read=controller_read>>1;

  }
  if(address==0x2002)
  {
    value_to_load = ppu->status;
    ppu->status&=127;
    ppu_write_address = 0;
  }
  if(address==PPU_CONTROL_CPU_ADDRESS)
  {
    value_to_load = ppu->control;
  }*/
  unsigned char value_to_load = get_value_to_load(toOr);
  LDA(value_to_load);  
}



void CMP_ptr(unsigned char* toOr)
{
  CMP_update_status_register(*toOr);  
}
void CMP(unsigned char* toOr)
{
  CMP_ptr(toOr);  
}
void CPX(unsigned char* toOr)
{
  CPX_update_status_register(*toOr);  
}
void CPY(unsigned char* toOr)
{
  CPY_update_status_register(*toOr);  
}
void SBC_ptr(unsigned char* toOr)
{
  SBC(*toOr);  
}

void ORA(unsigned char toOr)
{
  cpu->A|=toOr;
  ORA_update_status_register();
}

void AND(unsigned char toAnd)
{
  cpu->A&=toAnd;
  ORA_update_status_register();
 
}

void EOR(unsigned char toEor)
{
  cpu->A^=toEor;
  ORA_update_status_register();
 
}

void ADC(unsigned char toAdd)
{ 
  unsigned char oldA = cpu->A;
  //cpu->A+=toAdd;
  cpu->A+=(toAdd+(cpu->status&1));
  //cpu->A+=(cpu->status&1); // add carry if set
  ADC_update_status_register(oldA, toAdd ); 
}

void SBC(unsigned char toSubtract)
{
  //unsigned char oldA = cpu->A;
  //cpu->A-=toSubtract;
  //cpu->A-=(cpu->status&1); // subtract carry if set
  //SBC_update_status_register(oldA); 
  ADC(255 - toSubtract);
}

void store_value_at_address(unsigned char value, unsigned short address)
{
  if(address==PPU_CONTROL_CPU_ADDRESS)
  {
    ppu->control=value;
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x2001)
  {
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x2003)
  {
    oam_addr=value;
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x2004)
  {
    ppu->spr_ram[oam_addr]=value;
    oam_addr++;
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x4014)
  {
    cpu_sprite_copy_address_high=value;
    unsigned short cpu_sprite_copy_address = get_short_from_chars( cpu_sprite_copy_address_high, 0);
    
    for(int i=0; i< 256; i++)
    {
      ppu->spr_ram[(unsigned char)(oam_addr + (unsigned char)i)]=cpu->cpu_memory[cpu_sprite_copy_address + i]; 
    }
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x2005)
  {
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x2006)
  {
    ppu_write_address<<=8;
    ppu_write_address|=value; 
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x2007)
  {
    ppu->ppu_memory[ppu_write_address]=value;
    if(ppu->control&4)
    {
      ppu_write_address+=32;
    }
    else
    {
      ppu_write_address+=1;
    }
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==0x4016)
  {
    if(value==1) {
      read_controller_reset_await = 1;
    }
    else {
      if(value==0 && read_controller_reset_await) {
        controller_read = io->controller1;
      }
      read_controller_reset_await = 0;
      io->controller1=0;
    }
  }
  cpu->cpu_memory[address]=value;
  
  if(address<0x2000)
  {
    unsigned short address_mirror_range = 0x0800; 
    unsigned short start_address = (address<address_mirror_range) ? address : address%address_mirror_range;

    for(unsigned char w=1;w<4;w++)
    {
      cpu->cpu_memory[start_address + (w*address_mirror_range)]=value;
    }
  }
}

void STA(unsigned short address)
{
  store_value_at_address((unsigned char)cpu->A, address);
}

void SAX_do(unsigned short address)
{
  cpu->cpu_memory[address]=((unsigned char)cpu->A)&cpu->X;
}
void LDA(unsigned char newA)
{
  cpu->A=newA;
  ORA_update_status_register();
}
void shift_right(unsigned char* operand_ptr, char rotate)
{
  char original_carry = cpu->status&NES_CARRY_FLAG;
  switch_status_flag(NES_CARRY_FLAG, *operand_ptr&1);
  *operand_ptr = *operand_ptr>>1;
  if(rotate)
  {
    *operand_ptr|=(original_carry<<7);
  }
  switch_status_flag(NES_NEGATIVE_FLAG, (signed char)*operand_ptr < 0);
  switch_status_flag(NES_ZERO_FLAG, *operand_ptr==0);

}
void LSR(unsigned char* operand_ptr)
{
  shift_right(operand_ptr, 0);
}
void ROR(unsigned char* operand_ptr)
{
  shift_right(operand_ptr, 1);
}
void shift_left(unsigned char* operand_ptr, char rotate)
{
  char original_carry = cpu->status&NES_CARRY_FLAG;
  switch_status_flag(NES_CARRY_FLAG, *operand_ptr&128);
  *operand_ptr = *operand_ptr<<1;
  if(rotate)
  {
    *operand_ptr|=original_carry;
  }
  switch_status_flag(NES_NEGATIVE_FLAG, (signed char)*operand_ptr < 0);
  switch_status_flag(NES_ZERO_FLAG, *operand_ptr==0);
}

void ASL(unsigned char* operand_ptr)
{
  shift_left(operand_ptr, 0);

}

void ROL(unsigned char* operand_ptr)
{
  shift_left(operand_ptr, 1);
}


void STX(unsigned char* operand_ptr)
{
  //*operand_ptr = cpu->X;
  unsigned short address = operand_ptr - cpu->cpu_memory;
  store_value_at_address(cpu->X, address);
  
}

void LDX(unsigned char* operand_ptr)
{
  unsigned char value_to_load = get_value_to_load(operand_ptr);
  cpu->X = value_to_load;
  set_negative_zero_flag(cpu->X);
}

void STY(unsigned char* operand_ptr)
{
  unsigned short address = operand_ptr - cpu->cpu_memory;
  store_value_at_address(cpu->Y, address);
}

void LDY(unsigned char* operand_ptr)
{
  unsigned char value_to_load = get_value_to_load(operand_ptr);
  cpu->Y = value_to_load;
  set_negative_zero_flag(*operand_ptr);
}

void DEC(unsigned char* operand_ptr)
{
  *operand_ptr = *operand_ptr - 1;
  set_negative_zero_flag(*operand_ptr);
}

void BIT(unsigned char* operand_ptr)
{
  unsigned char value = peek_value(operand_ptr);
  unsigned char data = value&cpu->A;
  switch_status_flag(NES_NEGATIVE_FLAG, (value)&NES_NEGATIVE_FLAG);
  switch_status_flag(NES_OVERFLOW_FLAG, (value)&NES_OVERFLOW_FLAG);
  switch_status_flag(NES_ZERO_FLAG, data==0);
}

void JMP(unsigned char* operand_ptr)
{
  unsigned char* pc_0 = &cpu->cpu_memory[0];
  unsigned char* count = operand_ptr - pc_0; 
  cpu->PC = (unsigned short)count;
}

void INC(unsigned char* operand_ptr)
{
  *operand_ptr = *operand_ptr + 1;
  set_negative_zero_flag(*operand_ptr);
}

void STP(unsigned char* operand_ptr)
{
}

void SLO(unsigned char* operand_ptr)
{
  ASL(operand_ptr);
  ORA(*operand_ptr);
}

void SHX(unsigned char* operand_ptr)
{
}
void TAS(unsigned char* operand_ptr)
{
}


void ISC(unsigned char* operand_ptr)
{
  INC(operand_ptr);
  SBC(*operand_ptr); 
}

void AXS(unsigned char* operand_ptr)
{
}

void DCP(unsigned char* operand_ptr)

{

  //unsigned short address = operand_ptr - cpu->cpu_memory;
  //cpu->cpu_memory[address]-=1;
  *operand_ptr-=1;
  CMP_update_status_register(*operand_ptr);
}

void LAS(unsigned char* operand_ptr)
{
}

void SHY(unsigned char* operand_ptr)
{
}

void AHX(unsigned char* operand_ptr)
{
}

void XAA(unsigned char* operand_ptr)
{
}

void ARR(unsigned char* operand_ptr)
{
}

void RRA(unsigned char* operand_ptr)
{
  ROR(operand_ptr);
  ADC(*operand_ptr);
}


void SRE(unsigned char* operand_ptr)
{
  LSR(operand_ptr);
  EOR(*operand_ptr);
}

void ALR(unsigned char* operand_ptr)
{
}

void RLA(unsigned char* operand_ptr)
{
  ROL(operand_ptr);
  AND(*operand_ptr);
}
void SAX(unsigned char* operand_ptr)
{
  unsigned short address = operand_ptr - cpu->cpu_memory; 

  SAX_do(address);  

}

void LAX(unsigned char* operand_ptr)
{
  LDA_ptr(operand_ptr);
  LDX(operand_ptr);
}
void ANC(unsigned char* operand_ptr)
{
}



unsigned short get_short_from_cpu_memory(unsigned short mem_index)
{
  return get_short_from_chars(cpu->cpu_memory[mem_index + 1],cpu->cpu_memory[mem_index]);
}

void stack_push_char(unsigned char to_push)
{
  cpu->cpu_memory[STACK_BOTTOM+(cpu->stack_pointer)]=to_push;
  cpu->stack_pointer-=1;
}

void stack_push_short(unsigned short to_push)
{
  unsigned char low_byte = to_push;
  unsigned char high_byte = to_push>>8;
  stack_push_char(high_byte);
  stack_push_char(low_byte);
}

unsigned char stack_pull_char()
{
  unsigned char value =  cpu->cpu_memory[STACK_BOTTOM+(cpu->stack_pointer) +1];
  cpu->stack_pointer+=1;
  return value;
}

unsigned short stack_pull_short()
{
  unsigned char low_byte = stack_pull_char();
  unsigned char high_byte = stack_pull_char();
  return get_short_from_chars(high_byte, low_byte);
}

void BRK()
{
  increment_PC(2);
  stack_push_short(cpu->PC);  
  stack_push_char(cpu->status|NES_BREAK_FLAG);
  cpu->status|=NES_INTERRUPT_DISABLE_FLAG;
  unsigned short interrupt_vector = get_short_from_cpu_memory(0xfffe);
  cpu->PC=interrupt_vector;
}
void NMI()
{
  //increment_PC(3);
  stack_push_short(cpu->PC) ;  
  stack_push_char(cpu->status|NES_BREAK_FLAG);
  cpu->status|=NES_INTERRUPT_DISABLE_FLAG;
  ppu->status|=0x80;
  unsigned short interrupt_vector = get_short_from_cpu_memory(0xfffa);
  cpu->PC=interrupt_vector;
}
void JSR()
{
  stack_push_short(cpu->PC + 2);  
  unsigned short newPC = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  print_instruction_info(3, "$%02x%02x", "JSR");
  cpu->PC=newPC;
}

void RTI()
{

  print_instruction_info(1, "", "RTI");

  cpu->status =  stack_pull_char();
  switch_status_flag(32, 1);

  cpu->PC = stack_pull_short();  
}

void JMP_ind()
{
  unsigned short intermediate_address = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
  unsigned short newPC = get_indirect(intermediate_address);
  print_instruction_info(3, "$(%02x%02x)", "JMP");
  cpu->PC=newPC;
}

void RTS()
{
  cpu->PC = stack_pull_short() + 1 ;  
  print_instruction_info(1, "", "RTS");
}
void PHP()
{
  stack_push_char(cpu->status);
}
void PHA()
{
  stack_push_char(cpu->A);
}
void PLA()
{
  cpu->A=stack_pull_char();
  set_negative_zero_flag(cpu->A); 
}
void PLP()
{
  cpu->status=stack_pull_char();
  switch_status_flag(NES_BREAK_FLAG, 0);
  switch_status_flag(32, 1);
}
void DEY()
{
  cpu->Y-=1;
  set_negative_zero_flag(cpu->Y);
}
void TAY()
{
  cpu->Y=cpu->A;
  set_negative_zero_flag(cpu->Y);
}
void TYA()
{
  cpu->A=cpu->Y;
  set_negative_zero_flag(cpu->A);
}
void TXA()
{
  cpu->A=cpu->X;
  set_negative_zero_flag(cpu->A);
}
void TAX()
{
  cpu->X=cpu->A;
  set_negative_zero_flag(cpu->X);
}
void TXS()
{
  cpu->stack_pointer=cpu->X;
  //set_negative_zero_flag(cpu->stack_pointer);
}
void TSX()
{
  cpu->X=cpu->stack_pointer;
  set_negative_zero_flag(cpu->X);
}
void INX()
{
  cpu->X+=1;
  set_negative_zero_flag(cpu->X);
}
void DEX()
{
  cpu->X-=1;
  set_negative_zero_flag(cpu->X);
}
void INY()
{
  cpu->Y+=1;
  set_negative_zero_flag(cpu->Y);
}
void CLC()
{
  switch_status_flag(NES_CARRY_FLAG, 0);
}
void CLV()
{
  switch_status_flag(NES_OVERFLOW_FLAG, 0);
}
void CLD()
{
  switch_status_flag(NES_DECIMAL_MODE_FLAG, 0);
}
void SED()
{
  switch_status_flag(NES_DECIMAL_MODE_FLAG, 1);
}
void SEC()
{
  switch_status_flag(NES_CARRY_FLAG, 1);
}
void CLI()
{
  switch_status_flag(NES_INTERRUPT_DISABLE_FLAG, 0);
}
void SEI()
{
  switch_status_flag(NES_INTERRUPT_DISABLE_FLAG, 1);
}
void NOP()
{
}
void set_negative_zero_flag(unsigned char operand)
{
  switch_status_flag(NES_NEGATIVE_FLAG, operand&NES_NEGATIVE_FLAG);
  switch_status_flag(NES_ZERO_FLAG, operand==0);
}

void get_data_at_address_do_opcode(short address, opcode_action_type opcode_action)
{
  unsigned char data = cpu->cpu_memory[address];
  opcode_action(data);
}

unsigned char test_flag_and_branch(unsigned char flag, unsigned char equalTo, unsigned char offset)
{
  if((equalTo && (cpu->status&flag)) || (!equalTo && !(cpu->status&flag)))
  {
    increment_PC(offset + 2);
    return 1;
  }
  else
  {
    return 0;
  }

}

void branch(char* name, unsigned char flag, unsigned char equalTo)
{
  unsigned char offset = cpu->cpu_memory[cpu->PC+1];
  print_instruction_info(2, "#%02x", name);
  if(!test_flag_and_branch(flag, equalTo, offset))
  {
    increment_PC(2);
  } 
}
void BMI(unsigned char* c)
{
  branch("BMI", NES_NEGATIVE_FLAG, 1);
}

void BPL(unsigned char* c)
{
  branch("BPL", NES_NEGATIVE_FLAG, 0);
}

void BVS(unsigned char* c)
{
  branch("BMI", NES_OVERFLOW_FLAG, 1);
}

void BVC(unsigned char* c)
{
  branch("BVC", NES_OVERFLOW_FLAG, 0);
}

void BCS(unsigned char* c)
{
  branch("BCS", NES_CARRY_FLAG, 1);
}

void BCC(unsigned char* c)
{
  branch("BCC", NES_CARRY_FLAG, 0);
}

void BEQ(unsigned char* c)
{
  branch("BEQ", NES_CARRY_FLAG, 1);
}

void BNE(unsigned char* c)
{
  branch("BNE", NES_ZERO_FLAG, 0);
}

struct opcode* opcodes;// = malloc(256 * sizeof(struct opcode));
struct address* addresses;//[10];

void set_opcodes()
{
  opcodes = malloc(256 * sizeof(struct opcode));
  addresses = malloc(11 * sizeof(struct address));
  unsigned char IMMEDIATE_ADDRESS_MODE = 0;
  unsigned char ZEROPAGE_ADDRESS_MODE = 1;
  unsigned char ACCUMULATOR_ADDRESS_MODE = 2;
  unsigned char ABSOLUTE_ADDRESS_MODE = 3;
  unsigned char ZEROPAGE_X_ADDRESS_MODE = 4;
  unsigned char ABSOLUTE_X_ADDRESS_MODE = 5;
  unsigned char INDEXED_INDIRECT_X_ADDRESS_MODE = 6;
  unsigned char INDIRECT_INDEXED_Y_ADDRESS_MODE = 7; 
  unsigned char ABSOLUTE_Y_ADDRESS_MODE = 8;
  unsigned char ZEROPAGE_Y_ADDRESS_MODE = 9;
  unsigned char NONE_ADDRESS_MODE = 10;
  unsigned char ABSOLUTE_INDIRECT_ADDRESS_MODE = 11;
  
  addresses[0].program_counter_increment = 2;
  addresses[0].get_operand_ptr = get_immediate_operand_ptr;
  addresses[0].address_info = "#%02x";
  
  addresses[1].program_counter_increment = 2;
  addresses[1].get_operand_ptr = get_zeropage_operand_ptr;
  addresses[1].address_info = "$%02x";
 
  addresses[2].program_counter_increment = 1;
  addresses[2].get_operand_ptr = get_accumulator_operand_ptr;
  addresses[2].address_info = "A";
 
  addresses[3].program_counter_increment = 3;
  addresses[3].get_operand_ptr = get_absolute_operand_ptr;
  addresses[3].address_info = "$%02x%02x";
  
  addresses[4].program_counter_increment = 2;
  addresses[4].get_operand_ptr = get_zeropage_X_operand_ptr;
  addresses[4].address_info = "$%02x,X";

  addresses[5].program_counter_increment = 3;
  addresses[5].get_operand_ptr = get_absolute_X_operand_ptr;
  addresses[5].address_info = "$%02x%02x,X";

  addresses[6].program_counter_increment = 2;
  addresses[6].get_operand_ptr = get_indexed_indirect_X_operand_ptr;
  addresses[6].address_info = "($%02x,X)";

  addresses[7].program_counter_increment = 2;
  addresses[7].get_operand_ptr = get_indirect_indexed_Y_operand_ptr;
  addresses[7].address_info = "($%02x),Y";

  addresses[8].program_counter_increment = 3;
  addresses[8].get_operand_ptr = get_absolute_Y_operand_ptr;
  addresses[8].address_info = "$%02x%02x,Y";

  addresses[9].program_counter_increment = 2;
  addresses[9].get_operand_ptr = get_zeropage_Y_operand_ptr;
  addresses[9].address_info = "$%02x,Y";

  addresses[10].program_counter_increment = 1;
  addresses[10].get_operand_ptr = get_dummy_operand_ptr;
  addresses[10].address_info = "";

  for(int i=0; i<256; i++)
  {
    opcodes[i].name = 0;
    opcodes[i].action = 0;
  }

  opcodes[0x00].name = "BRK";
  opcodes[0x00].action = BRK;
  opcodes[0x00].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x01].name = "ORA";
  opcodes[0x01].action = ORA_ptr;
  opcodes[0x01].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x02].name = "STP";
  opcodes[0x02].action = STP;
  opcodes[0x02].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x03].name = "SLO";
  opcodes[0x03].action = SLO;
  opcodes[0x03].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x04].name = "NOP";
  opcodes[0x04].action = NOP;
  opcodes[0x04].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x05].name = "ORA";
  opcodes[0x05].action = ORA_ptr;
  opcodes[0x05].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x06].name = "ASL";
  opcodes[0x06].action = ASL;
  opcodes[0x06].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x07].name = "SLO";
  opcodes[0x07].action = SLO;
  opcodes[0x07].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x08].name = "PHP";
  opcodes[0x08].action = PHP;
  opcodes[0x08].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x09].name = "ORA";
  opcodes[0x09].action = ORA_ptr;
  opcodes[0x09].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x0a].name = "ASL";
  opcodes[0x0a].action = ASL;
  opcodes[0x0a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  opcodes[0x0b].name = "ANC";
  opcodes[0x0b].action = ANC;
  opcodes[0x0b].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x0c].name = "NOP";
  opcodes[0x0c].action = NOP;
  opcodes[0x0c].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x0d].name = "ORA";
  opcodes[0x0d].action = ORA_ptr;
  opcodes[0x0d].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x0e].name = "ASL";
  opcodes[0x0e].action = ASL;
  opcodes[0x0e].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x0f].name = "SLO";
  opcodes[0x0f].action = SLO;
  opcodes[0x0f].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x10].name = "BPL";
  opcodes[0x10].action = BPL;
  opcodes[0x10].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x11].name = "ORA";
  opcodes[0x11].action = ORA_ptr;
  opcodes[0x11].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x12].name = "STP";
  opcodes[0x12].action = STP;
  opcodes[0x12].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x13].name = "SLO";
  opcodes[0x13].action = SLO;
  opcodes[0x13].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x14].name = "NOP";
  opcodes[0x14].action = NOP;
  opcodes[0x14].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x15].name = "ORA";
  opcodes[0x15].action = ORA_ptr;
  opcodes[0x15].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x16].name = "ASL";
  opcodes[0x16].action = ASL;
  opcodes[0x16].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x17].name = "SLO";
  opcodes[0x17].action = SLO;
  opcodes[0x17].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x18].name = "CLC";
  opcodes[0x18].action = CLC;
  opcodes[0x18].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x19].name = "ORA";
  opcodes[0x19].action = ORA_ptr;
  opcodes[0x19].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x1a].name = "NOP";
  opcodes[0x1a].action = NOP;
  opcodes[0x1a].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x1b].name = "SLO";
  opcodes[0x1b].action = SLO;
  opcodes[0x1b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x1c].name = "NOP";
  opcodes[0x1c].action = NOP;
  opcodes[0x1c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x1d].name = "ORA";
  opcodes[0x1d].action = ORA_ptr;
  opcodes[0x1d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x1e].name = "ASL";
  opcodes[0x1e].action = ASL;
  opcodes[0x1e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x1f].name = "SLO";
  opcodes[0x1f].action = SLO;
  opcodes[0x1f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x1f].name = "SLO";
  opcodes[0x1f].action = SLO;
  opcodes[0x1f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x1f].name = "SLO";
  opcodes[0x1f].action = SLO;
  opcodes[0x1f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x20].name = "JSR";
  opcodes[0x20].action = JSR;
  opcodes[0x20].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x21].name = "AND";
  opcodes[0x21].action = AND_ptr;
  opcodes[0x21].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x22].name = "STP";
  opcodes[0x22].action = STP;
  opcodes[0x22].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x23].name = "RLA";
  opcodes[0x23].action = RLA;
  opcodes[0x23].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x24].name = "BIT";
  opcodes[0x24].action = BIT;
  opcodes[0x24].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x25].name = "AND";
  opcodes[0x25].action = AND_ptr;
  opcodes[0x25].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x26].name = "ROL";
  opcodes[0x26].action = ROL;
  opcodes[0x26].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x27].name = "RLA";
  opcodes[0x27].action = RLA;
  opcodes[0x27].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x28].name = "PLP";
  opcodes[0x28].action = PLP;
  opcodes[0x28].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x29].name = "AND";
  opcodes[0x29].action = AND_ptr;
  opcodes[0x29].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x2a].name = "ROL";
  opcodes[0x2a].action = ROL;
  opcodes[0x2a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  opcodes[0x2b].name = "ANC";
  opcodes[0x2b].action = ANC;
  opcodes[0x2b].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x2c].name = "BIT";
  opcodes[0x2c].action = BIT;
  opcodes[0x2c].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x2d].name = "AND";
  opcodes[0x2d].action = AND_ptr;
  opcodes[0x2d].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x2e].name = "ROL";
  opcodes[0x2e].action = ROL;
  opcodes[0x2e].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x2f].name = "RLA";
  opcodes[0x2f].action = RLA;
  opcodes[0x2f].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x30].name = "BMI";
  opcodes[0x30].action = BMI;
  opcodes[0x30].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x31].name = "AND";
  opcodes[0x31].action = AND_ptr;
  opcodes[0x31].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x32].name = "STP";
  opcodes[0x32].action = STP;
  opcodes[0x32].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x33].name = "RLA";
  opcodes[0x33].action = RLA;
  opcodes[0x33].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x34].name = "NOP";
  opcodes[0x34].action = NOP;
  opcodes[0x34].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x35].name = "AND";
  opcodes[0x35].action = AND_ptr;
  opcodes[0x35].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x36].name = "ROL";
  opcodes[0x36].action = ROL;
  opcodes[0x36].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x37].name = "RLA";
  opcodes[0x37].action = RLA;
  opcodes[0x37].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x38].name = "SEC";
  opcodes[0x38].action = SEC;
  opcodes[0x38].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x39].name = "AND";
  opcodes[0x39].action = AND_ptr;
  opcodes[0x39].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x3a].name = "NOP";
  opcodes[0x3a].action = NOP;
  opcodes[0x3a].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x3b].name = "RLA";
  opcodes[0x3b].action = RLA;
  opcodes[0x3b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x3c].name = "NOP";
  opcodes[0x3c].action = NOP;
  opcodes[0x3c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x3d].name = "AND";
  opcodes[0x3d].action = AND_ptr;
  opcodes[0x3d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x3e].name = "ROL";
  opcodes[0x3e].action = ROL;
  opcodes[0x3e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x3f].name = "RLA";
  opcodes[0x3f].action = RLA;
  opcodes[0x3f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x40].name = "RTI";
  opcodes[0x40].action = RTI;
  opcodes[0x40].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x41].name = "EOR";
  opcodes[0x41].action = EOR_ptr;
  opcodes[0x41].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x42].name = "STP";
  opcodes[0x42].action = STP;
  opcodes[0x42].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x43].name = "SRE";
  opcodes[0x43].action = SRE;
  opcodes[0x43].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x44].name = "NOP";
  opcodes[0x44].action = NOP;
  opcodes[0x44].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x45].name = "EOR";
  opcodes[0x45].action = EOR_ptr;
  opcodes[0x45].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x46].name = "LSR";
  opcodes[0x46].action = LSR;
  opcodes[0x46].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x47].name = "SRE";
  opcodes[0x47].action = SRE;
  opcodes[0x47].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x48].name = "PHA";
  opcodes[0x48].action = PHA;
  opcodes[0x48].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x49].name = "EOR";
  opcodes[0x49].action = EOR_ptr;
  opcodes[0x49].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x4a].name = "LSR";
  opcodes[0x4a].action = LSR;
  opcodes[0x4a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  opcodes[0x4b].name = "ALR";
  opcodes[0x4b].action = ALR;
  opcodes[0x4b].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x4c].name = "JMP";
  opcodes[0x4c].action = JMP;
  opcodes[0x4c].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x4d].name = "EOR";
  opcodes[0x4d].action = EOR_ptr;
  opcodes[0x4d].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x4e].name = "LSR";
  opcodes[0x4e].action = LSR;
  opcodes[0x4e].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x4f].name = "SRE";
  opcodes[0x4f].action = SRE;
  opcodes[0x4f].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x50].name = "BVC";
  opcodes[0x50].action = BVC;
  opcodes[0x50].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x51].name = "EOR";
  opcodes[0x51].action = EOR_ptr;
  opcodes[0x51].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x52].name = "STP";
  opcodes[0x52].action = STP;
  opcodes[0x52].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x53].name = "SRE";
  opcodes[0x53].action = SRE;
  opcodes[0x53].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;


  opcodes[0x54].name = "NOP";
  opcodes[0x54].action = NOP;
  opcodes[0x54].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x55].name = "EOR";
  opcodes[0x55].action = EOR_ptr;
  opcodes[0x55].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x56].name = "LSR";
  opcodes[0x56].action = LSR;
  opcodes[0x56].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x57].name = "SRE";
  opcodes[0x57].action = SRE;
  opcodes[0x57].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x58].name = "CLI";
  opcodes[0x58].action = CLI;
  opcodes[0x58].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x59].name = "EOR";
  opcodes[0x59].action = EOR_ptr;
  opcodes[0x59].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x5a].name = "NOP";
  opcodes[0x5a].action = NOP;
  opcodes[0x5a].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x5b].name = "SRE";
  opcodes[0x5b].action = SRE;
  opcodes[0x5b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x5c].name = "NOP";
  opcodes[0x5c].action = NOP;
  opcodes[0x5c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x5d].name = "EOR";
  opcodes[0x5d].action = EOR_ptr;
  opcodes[0x5d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x5e].name = "LSR";
  opcodes[0x5e].action = LSR;
  opcodes[0x5e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x5f].name = "SRE";
  opcodes[0x5f].action = SRE;
  opcodes[0x5f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x60].name = "RTS";
  opcodes[0x60].action = RTS;
  opcodes[0x60].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x61].name = "ADC";
  opcodes[0x61].action = ADC_ptr;
  opcodes[0x61].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x62].name = "STP";
  opcodes[0x62].action = STP;
  opcodes[0x62].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x63].name = "RRA";
  opcodes[0x63].action = RRA;
  opcodes[0x63].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x64].name = "NOP";
  opcodes[0x64].action = NOP;
  opcodes[0x64].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x65].name = "ADC";
  opcodes[0x65].action = ADC_ptr;
  opcodes[0x65].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x66].name = "ROR";
  opcodes[0x66].action = ROR;
  opcodes[0x66].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x67].name = "RRA";
  opcodes[0x67].action = RRA;
  opcodes[0x67].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x68].name = "PLA";
  opcodes[0x68].action = PLA;
  opcodes[0x68].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x69].name = "ADC";
  opcodes[0x69].action = ADC_ptr;
  opcodes[0x69].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x6a].name = "ROR";
  opcodes[0x6a].action = ROR;
  opcodes[0x6a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  opcodes[0x6b].name = "ARR";
  opcodes[0x6b].action = ARR;
  opcodes[0x6b].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x6c].name = "JMP";
  opcodes[0x6c].action = JMP;
  opcodes[0x6c].address_mode = ABSOLUTE_INDIRECT_ADDRESS_MODE;

  opcodes[0x6d].name = "ADC";
  opcodes[0x6d].action = ADC_ptr;
  opcodes[0x6d].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x6e].name = "ROR";
  opcodes[0x6e].action = ROR;
  opcodes[0x6e].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x6f].name = "RRA";
  opcodes[0x6f].action = RRA;
  opcodes[0x6f].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x70].name = "BVS";
  opcodes[0x70].action = BVS;
  opcodes[0x70].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x71].name = "ADC";
  opcodes[0x71].action = ADC_ptr;
  opcodes[0x71].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x72].name = "STP";
  opcodes[0x72].action = STP;
  opcodes[0x72].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x73].name = "RRA";
  opcodes[0x73].action = RRA;
  opcodes[0x73].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x74].name = "NOP";
  opcodes[0x74].action = NOP;
  opcodes[0x74].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x75].name = "ADC";
  opcodes[0x75].action = ADC_ptr;
  opcodes[0x75].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x76].name = "ROR";
  opcodes[0x76].action = ROR;
  opcodes[0x76].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x77].name = "RRA";
  opcodes[0x77].action = RRA;
  opcodes[0x77].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x78].name = "SEI";
  opcodes[0x78].action = SEI;
  opcodes[0x78].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x79].name = "ADC";
  opcodes[0x79].action = ADC_ptr;
  opcodes[0x79].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x7a].name = "NOP";
  opcodes[0x7a].action = NOP;
  opcodes[0x7a].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x7b].name = "RRA";
  opcodes[0x7b].action = RRA;
  opcodes[0x7b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x7c].name = "NOP";
  opcodes[0x7c].action = NOP;
  opcodes[0x7c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x7d].name = "ADC";
  opcodes[0x7d].action = ADC_ptr;
  opcodes[0x7d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x7e].name = "ROR";
  opcodes[0x7e].action = ROR;
  opcodes[0x7e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x7f].name = "RRA";
  opcodes[0x7f].action = RRA;
  opcodes[0x7f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x80].name = "NOP";
  opcodes[0x80].action = NOP;
  opcodes[0x80].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x81].name = "STA";
  opcodes[0x81].action = STA_ptr;
  opcodes[0x81].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x82].name = "NOP";
  opcodes[0x82].action = NOP;
  opcodes[0x82].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x83].name = "SAX";
  opcodes[0x83].action = SAX;
  opcodes[0x83].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0x84].name = "STY";
  opcodes[0x84].action = STY;
  opcodes[0x84].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x85].name = "STA";
  opcodes[0x85].action = STA_ptr;
  opcodes[0x85].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x86].name = "STX";
  opcodes[0x86].action = STX;
  opcodes[0x86].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x87].name = "SAX";
  opcodes[0x87].action = SAX;
  opcodes[0x87].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x88].name = "DEY";
  opcodes[0x88].action = DEY;
  opcodes[0x88].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x89].name = "NOP";
  opcodes[0x89].action = NOP;
  opcodes[0x89].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x8a].name = "TXA";
  opcodes[0x8a].action = TXA;
  opcodes[0x8a].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x8b].name = "XAA";
  opcodes[0x8b].action = XAA;
  opcodes[0x8b].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0x8c].name = "STY";
  opcodes[0x8c].action = STY;
  opcodes[0x8c].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x8d].name = "STA";
  opcodes[0x8d].action = STA_ptr;
  opcodes[0x8d].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x8e].name = "STX";
  opcodes[0x8e].action = STX;
  opcodes[0x8e].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x8f].name = "SAX";
  opcodes[0x8f].action = SAX;
  opcodes[0x8f].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0x90].name = "BCC";
  opcodes[0x90].action = BCC;
  opcodes[0x90].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0x91].name = "STA";
  opcodes[0x91].action = STA_ptr;
  opcodes[0x91].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0x92].name = "STP";
  opcodes[0x92].action = STP;
  opcodes[0x92].address_mode = NONE_ADDRESS_MODE;
  
  opcodes[0x93].name = "AHX";
  opcodes[0x93].action = AHX;
  opcodes[0x93].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;
  
  opcodes[0x94].name = "STY";
  opcodes[0x94].action = STY;
  opcodes[0x94].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x95].name = "STA";
  opcodes[0x95].action = STA_ptr;
  opcodes[0x95].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0x96].name = "STX";
  opcodes[0x96].action = STX;
  opcodes[0x96].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  opcodes[0x97].name = "SAX";
  opcodes[0x97].action = SAX;
  opcodes[0x97].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  opcodes[0x98].name = "TYA";
  opcodes[0x98].action = TYA;
  opcodes[0x98].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x99].name = "STA";
  opcodes[0x99].action = STA_ptr;
  opcodes[0x99].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x9a].name = "TXS";
  opcodes[0x9a].action = TXS;
  opcodes[0x9a].address_mode = NONE_ADDRESS_MODE;

  opcodes[0x9b].name = "TAS";
  opcodes[0x9b].action = TAS;
  opcodes[0x9b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x9c].name = "SHY";
  opcodes[0x9c].action = SHY;
  opcodes[0x9c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x9d].name = "STA";
  opcodes[0x9d].action = STA_ptr;
  opcodes[0x9d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0x9e].name = "SHX";
  opcodes[0x9e].action = SHX;
  opcodes[0x9e].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0x9f].name = "AHX";
  opcodes[0x9f].action = AHX;
  opcodes[0x9f].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xa0].name = "LDY";
  opcodes[0xa0].action = LDY;
  opcodes[0xa0].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xa1].name = "LDA";
  opcodes[0xa1].action = LDA_ptr;
  opcodes[0xa1].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0xa2].name = "LDX";
  opcodes[0xa2].action = LDX;
  opcodes[0xa2].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xa3].name = "LAX";
  opcodes[0xa3].action = LAX;
  opcodes[0xa3].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0xa4].name = "LDY";
  opcodes[0xa4].action = LDY;
  opcodes[0xa4].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xa5].name = "LDA";
  opcodes[0xa5].action = LDA_ptr;
  opcodes[0xa5].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xa6].name = "LDX";
  opcodes[0xa6].action = LDX;
  opcodes[0xa6].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xa7].name = "LAX";
  opcodes[0xa7].action = LAX;
  opcodes[0xa7].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xa8].name = "TAY";
  opcodes[0xa8].action = TAY;
  opcodes[0xa8].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xa9].name = "LDA";
  opcodes[0xa9].action = LDA_ptr;
  opcodes[0xa9].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xaa].name = "TAX";
  opcodes[0xaa].action = TAX;
  opcodes[0xaa].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xab].name = "LAX";
  opcodes[0xab].action = LAX;
  opcodes[0xab].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xac].name = "LDY";
  opcodes[0xac].action = LDY;
  opcodes[0xac].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xad].name = "LDA";
  opcodes[0xad].action = LDA_ptr;
  opcodes[0xad].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xae].name = "LDX";
  opcodes[0xae].action = LDX;
  opcodes[0xae].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xaf].name = "LAX";
  opcodes[0xaf].action = LAX;
  opcodes[0xaf].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xb0].name = "BCS";
  opcodes[0xb0].action = BCS;
  opcodes[0xb0].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xb1].name = "LDA";
  opcodes[0xb1].action = LDA_ptr;
  opcodes[0xb1].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0xb2].name = "STP";
  opcodes[0xb2].action = STP;
  opcodes[0xb2].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xb3].name = "LAX";
  opcodes[0xb3].action = LAX;
  opcodes[0xb3].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0xb4].name = "LDY";
  opcodes[0xb4].action = LDY;
  opcodes[0xb4].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xb5].name = "LDA";
  opcodes[0xb5].action = LDA_ptr;
  opcodes[0xb5].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xb6].name = "LDX";
  opcodes[0xb6].action = LDX;
  opcodes[0xb6].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  opcodes[0xb7].name = "LAX";
  opcodes[0xb7].action = LAX;
  opcodes[0xb7].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  opcodes[0xb8].name = "CLV";
  opcodes[0xb8].action = CLV;
  opcodes[0xb8].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xb9].name = "LDA";
  opcodes[0xb9].action = LDA_ptr;
  opcodes[0xb9].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xba].name = "TSX";
  opcodes[0xba].action = TSX;
  opcodes[0xba].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xbb].name = "LAS";
  opcodes[0xbb].action = LAS;
  opcodes[0xbb].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xbc].name = "LDY";
  opcodes[0xbc].action = LDY;
  opcodes[0xbc].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0xbd].name = "LDA";
  opcodes[0xbd].action = LDA_ptr;
  opcodes[0xbd].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0xbe].name = "LDX";
  opcodes[0xbe].action = LDX;
  opcodes[0xbe].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xbf].name = "LAX";
  opcodes[0xbf].action = LAX;
  opcodes[0xbf].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xc0].name = "CPY";
  opcodes[0xc0].action = CPY;
  opcodes[0xc0].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xc1].name = "CMP";
  opcodes[0xc1].action = CMP;
  opcodes[0xc1].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0xc2].name = "NOP";
  opcodes[0xc2].action = NOP;
  opcodes[0xc2].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xc3].name = "DCP";
  opcodes[0xc3].action = DCP;
  opcodes[0xc3].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0xc4].name = "CPY";
  opcodes[0xc4].action = CPY;
  opcodes[0xc4].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xc5].name = "CMP";
  opcodes[0xc5].action = CMP;
  opcodes[0xc5].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xc6].name = "DEC";
  opcodes[0xc6].action = DEC;
  opcodes[0xc6].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xc7].name = "DCP";
  opcodes[0xc7].action = DCP;
  opcodes[0xc7].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xc8].name = "INY";
  opcodes[0xc8].action = INY;
  opcodes[0xc8].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xc9].name = "CMP";
  opcodes[0xc9].action = CMP;
  opcodes[0xc9].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xca].name = "DEX";
  opcodes[0xca].action = DEX;
  opcodes[0xca].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xcb].name = "AXS";
  opcodes[0xcb].action = AXS;
  opcodes[0xcb].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xcc].name = "CPY";
  opcodes[0xcc].action = CPY;
  opcodes[0xcc].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xcd].name = "CMP";
  opcodes[0xcd].action = CMP;
  opcodes[0xcd].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xce].name = "DEC";
  opcodes[0xce].action = DEC;
  opcodes[0xce].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xcf].name = "DCP";
  opcodes[0xcf].action = DCP;
  opcodes[0xcf].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xd0].name = "BNE";
  opcodes[0xd0].action = BNE;
  opcodes[0xd0].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xd1].name = "CMP";
  opcodes[0xd1].action = CMP;
  opcodes[0xd1].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0xd2].name = "STP";
  opcodes[0xd2].action = STP;
  opcodes[0xd2].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xd3].name = "DCP";
  opcodes[0xd3].action = DCP;
  opcodes[0xd3].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0xd4].name = "NOP";
  opcodes[0xd4].action = NOP;
  opcodes[0xd4].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xd5].name = "CMP";
  opcodes[0xd5].action = CMP;
  opcodes[0xd5].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xd6].name = "DEC";
  opcodes[0xd6].action = DEC;
  opcodes[0xd6].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xd7].name = "DCP";
  opcodes[0xd7].action = DCP;
  opcodes[0xd7].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xd8].name = "CLD";
  opcodes[0xd8].action = CLD;
  opcodes[0xd8].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xd9].name = "CMP";
  opcodes[0xd9].action = CMP;
  opcodes[0xd9].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xda].name = "NOP";
  opcodes[0xda].action = NOP;
  opcodes[0xda].address_mode = NONE_ADDRESS_MODE;

  opcodes[0xdb].name = "DCP";
  opcodes[0xdb].action = DCP;
  opcodes[0xdb].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xdc].name = "NOP";
  opcodes[0xdc].action = NOP;
  opcodes[0xdc].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0xdd].name = "CMP";
  opcodes[0xdd].action = CMP;
  opcodes[0xdd].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  opcodes[0xde].name = "DEC";
  opcodes[0xde].action = DEC;
  opcodes[0xde].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  opcodes[0xdf].name = "DCP";
  opcodes[0xdf].action = DCP;
  opcodes[0xdf].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  opcodes[0xe0].name = "CPX";
  opcodes[0xe0].action = CPX;
  opcodes[0xe0].address_mode = IMMEDIATE_ADDRESS_MODE;
  
  opcodes[0xe1].name = "SBC";
  opcodes[0xe1].action = SBC_ptr;
  opcodes[0xe1].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0xe2].name = "NOP";
  opcodes[0xe2].action = NOP;
  opcodes[0xe2].address_mode = IMMEDIATE_ADDRESS_MODE;
  
  opcodes[0xe3].name = "ISC";
  opcodes[0xe3].action = ISC;
  opcodes[0xe3].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  opcodes[0xe4].name = "CPX";
  opcodes[0xe4].action = CPX;
  opcodes[0xe4].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  opcodes[0xe5].name = "SBC";
  opcodes[0xe5].action = SBC_ptr;
  opcodes[0xe5].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  opcodes[0xe6].name = "INC";
  opcodes[0xe6].action = INC;
  opcodes[0xe6].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  opcodes[0xe7].name = "ISC";
  opcodes[0xe7].action = ISC;
  opcodes[0xe7].address_mode = ZEROPAGE_ADDRESS_MODE;

  opcodes[0xe8].name = "INX";
  opcodes[0xe8].action = INX;
  opcodes[0xe8].address_mode = NONE_ADDRESS_MODE;
  
  opcodes[0xe9].name = "SBC";
  opcodes[0xe9].action = SBC_ptr;
  opcodes[0xe9].address_mode = IMMEDIATE_ADDRESS_MODE;

  opcodes[0xea].name = "NOP";
  opcodes[0xea].action = NOP;
  opcodes[0xea].address_mode = NONE_ADDRESS_MODE;
  
  opcodes[0xeb].name = "SBC";
  opcodes[0xeb].action = SBC_ptr;
  opcodes[0xeb].address_mode = IMMEDIATE_ADDRESS_MODE;
  
  opcodes[0xec].name = "CPX";
  opcodes[0xec].action = CPX;
  opcodes[0xec].address_mode = ABSOLUTE_ADDRESS_MODE;
  
  opcodes[0xed].name = "SBC";
  opcodes[0xed].action = SBC_ptr;
  opcodes[0xed].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xee].name = "INC";
  opcodes[0xee].action = INC;
  opcodes[0xee].address_mode = ABSOLUTE_ADDRESS_MODE;
  
  opcodes[0xef].name = "ISC";
  opcodes[0xef].action = ISC;
  opcodes[0xef].address_mode = ABSOLUTE_ADDRESS_MODE;

  opcodes[0xf0].name = "BEQ";
  opcodes[0xf0].action = BEQ;
  opcodes[0xf0].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  opcodes[0xf1].name = "SBC";
  opcodes[0xf1].action = SBC_ptr;
  opcodes[0xf1].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;
  
  opcodes[0xf2].name = "STP";
  opcodes[0xf2].action = STP;
  opcodes[0xf2].address_mode = NONE_ADDRESS_MODE;
  
  opcodes[0xf3].name = "ISC";
  opcodes[0xf3].action = ISC;
  opcodes[0xf3].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  opcodes[0xf4].name = "NOP";
  opcodes[0xf4].action = NOP;
  opcodes[0xf4].address_mode = ZEROPAGE_X_ADDRESS_MODE;
  
  opcodes[0xf5].name = "SBC";
  opcodes[0xf5].action = SBC_ptr;
  opcodes[0xf5].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  opcodes[0xf6].name = "INC";
  opcodes[0xf6].action = INC;
  opcodes[0xf6].address_mode = ZEROPAGE_X_ADDRESS_MODE;
  
  opcodes[0xf7].name = "ISC";
  opcodes[0xf7].action = ISC;
  opcodes[0xf7].address_mode = ZEROPAGE_X_ADDRESS_MODE;
  
  opcodes[0xf8].name = "SED";
  opcodes[0xf8].action = SED;
  opcodes[0xf8].address_mode = NONE_ADDRESS_MODE;
  
  opcodes[0xf9].name = "SBC";
  opcodes[0xf9].action = SBC_ptr;
  opcodes[0xf9].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xfa].name = "NOP";
  opcodes[0xfa].action = NOP;
  opcodes[0xfa].address_mode = NONE_ADDRESS_MODE;
  
  opcodes[0xfb].name = "ISC";
  opcodes[0xfb].action = ISC;
  opcodes[0xfb].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  opcodes[0xfc].name = "NOP";
  opcodes[0xfc].action = NOP;
  opcodes[0xfc].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  opcodes[0xfd].name = "SBC";
  opcodes[0xfd].action = SBC_ptr;
  opcodes[0xfd].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  opcodes[0xfe].name = "INC";
  opcodes[0xfe].action = INC;
  opcodes[0xfe].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  opcodes[0xff].name = "ISC";
  opcodes[0xff].action = ISC;
  opcodes[0xff].address_mode = ABSOLUTE_X_ADDRESS_MODE;


}


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
    //char* opcode_info = (char*)malloc(10 * sizeof(char));
    char* address_info = addresses[addressing_mode].address_info;
   
    //strcpy(opcode_info, opcodes[opcode].name);
    print_instruction_info(program_counter_increment, address_info, opcodes[opcode].name);
    //free(opcode_info);
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
  if(strcmp("JMP", opcodes[current_opcode].name))
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

void run_rom()
{
  
  clock_t nmi_time = clock();
  SDL_Window* win =createWindow();
  SDL_Renderer* rend =createRenderer(win);
  //createWindowAndRenderer(&win, &rend);
  SDL_Texture* text = createTexture(rend);
  set_opcodes();
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
    unsigned char exceptional_instruction = 0;
    switch(current_opcode)
    {
      case 0x20:
        JSR();
        exceptional_instruction= 1;
        break;
      case 0x60:
        RTS();
        exceptional_instruction= 1;
        break;
      case 0x40:
        RTI();
        exceptional_instruction= 1;
        break;
      case 0x6C:
        JMP_ind();
        exceptional_instruction=1;
        break;
    }
    if(!exceptional_instruction)
    { 
      //if(opcodes[current_opcode].action)
      //{
        //unsigned char* dummy_ptr = 0;
        //print_instruction_info(1, "", opcodes[current_opcode].name);
        //opcodes[current_opcode].action(dummy_ptr);      
        //increment_PC(1);
      //}
      //else
      //{
        if((current_opcode&COND_BRANCH_MASK)==16)
        {
          conditional_branch_instruction(current_opcode);
        }
        else
        { 
          standard_instruction(current_opcode);
        }
      //}
    }
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

