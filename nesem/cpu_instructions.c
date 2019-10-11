#include <cpu_instructions.h>
#include <cpu_addressing.h>
static unsigned char read_controller_reset_await = 0;
static unsigned char controller_read = 0;
static unsigned short ppu_write_address;
static unsigned char ppu_read_buffer = 0;
static unsigned char oam_addr;
static unsigned char cpu_sprite_copy_address_high;

void set_ppu_write_address(unsigned short address)
{
  ppu_write_address = address;
}

unsigned short get_ppu_write_address()
{
  return ppu_write_address;
}
unsigned short get_short_from_chars(unsigned char high_byte, unsigned char low_byte)
{
  return ((high_byte<<8) | low_byte);
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
  if(address==PPU_STATUS_CPU_ADDRESS)
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
  if(address==PPU_OAM_DATA_CPU_ADDRESS)
  {
    value_to_load = ppu->spr_ram[oam_addr];
  }
  if(address==PPU_DATA_CPU_ADDRESS)
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
  cpu->A+=(toAdd+(cpu->status&1));
  ADC_update_status_register(oldA, toAdd ); 
}

void SBC(unsigned char toSubtract)
{
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
  if(address==PPU_STATUS_CPU_ADDRESS)
  {
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==PPU_OAM_ADDRESS_CPU_ADDRESS)
  {
    oam_addr=value;
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==PPU_OAM_DATA_CPU_ADDRESS)
  {
    ppu->spr_ram[oam_addr]=value;
    oam_addr++;
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==PPU_OAM_DMA_CPU_ADDRESS)
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
  if(address==PPU_SCROLL_CPU_ADDRESS)
  {
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==PPU_ADDRESS_CPU_ADDRESS)
  {
    ppu_write_address<<=8;
    ppu_write_address|=value; 
    ppu->status&=224;
    ppu->status|=value;
  }
  if(address==PPU_DATA_CPU_ADDRESS)
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


void RTI()
{

  //print_instruction_info(1, "", "RTI");

  cpu->status =  stack_pull_char();
  switch_status_flag(32, 1);

  cpu->PC = stack_pull_short();  
}



void RTS()
{
  cpu->PC = stack_pull_short() + 1 ;  
  //print_instruction_info(1, "", "RTS");
}
void PHP()
{
  stack_push_char(cpu->status|0x30);
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

unsigned char test_flag_and_branch(unsigned char flag, unsigned char equalTo, unsigned char* newPC_ptr)
{
  if((equalTo && (cpu->status&flag)) || (!equalTo && !(cpu->status&flag)))
  {
    unsigned short newPC = newPC_ptr - cpu->cpu_memory;
    cpu->cycles+=1;
    unsigned char newAddress_upper =  ((unsigned char*)(&newPC))[1];
    unsigned char current_upper    =  ((unsigned char*)(&(cpu->PC)))[1];
    if(newAddress_upper!=current_upper)
    {
      cpu->cycles+=1;
    }
    cpu->PC = newPC;
    return 1;
  }
  else
  {
    return 0;
  }

}

void branch(unsigned char flag, unsigned char equalTo, unsigned char* newPC_ptr)
{
  if(!test_flag_and_branch(flag, equalTo, newPC_ptr))
  {
    increment_PC(2);
  } 
}
void BMI(unsigned char* c)
{
  branch(NES_NEGATIVE_FLAG, 1, c);
}

void BPL(unsigned char* c)
{
  branch(NES_NEGATIVE_FLAG, 0, c);
}

void BVS(unsigned char* c)
{
  branch(NES_OVERFLOW_FLAG, 1, c);
}

void BVC(unsigned char* c)
{
  branch(NES_OVERFLOW_FLAG, 0, c);
}

void BCS(unsigned char* c)
{
  branch(NES_CARRY_FLAG, 1, c);
}

void BCC(unsigned char* c)
{
  branch(NES_CARRY_FLAG, 0, c);
}

void BEQ(unsigned char* c)
{
  branch(NES_ZERO_FLAG, 1, c);
}

void BNE(unsigned char* c)
{
  branch(NES_ZERO_FLAG, 0, c);
}


void JSR(unsigned char* operand_ptr)
{
  stack_push_short(cpu->PC + 2);  
unsigned char* pc_0 = &cpu->cpu_memory[0];
  unsigned char* count = operand_ptr - pc_0; 
  cpu->PC = (unsigned short)count;
}
