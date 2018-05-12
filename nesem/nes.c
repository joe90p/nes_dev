#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include </home/phil/git/nes_dev/nesem/sdl_test.h>

void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;
void print_instruction_info(char program_counter_increment, char* address_info, char* opcode_info);

void run_rom();
static unsigned short ppu_write_address;

unsigned char* get_immediate_operand_ptr()
{
  return &cpu->cpu_memory[cpu->PC + 1];
}

unsigned char* get_zeropage_operand_ptr()
{
  unsigned char address = get_zeropage_address(cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[address];
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

unsigned char* get_absolute_X_operand_ptr()
{
  unsigned char address = get_absolute_address_X(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  return &cpu->cpu_memory[address];
}

unsigned char* get_absolute_Y_operand_ptr()
{
  unsigned char address = get_absolute_address_Y(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  return &cpu->cpu_memory[address];
}

unsigned char* get_indexed_indirect_X_operand_ptr()
{
  unsigned char address = get_indexed_indirect_X(cpu->cpu_memory[cpu->PC + 1]);
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

unsigned short get_indirect_indexed_Y(unsigned short get_address_input)
{ 
  return get_absolute_address(cpu->cpu_memory[get_address_input + 1], cpu->cpu_memory[get_address_input]) + cpu->Y;
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
  int rom_fileoffset=16;
  char flags6 = ines_file_contents[6];
  char prg_rom_banks_num = ines_file_contents[4];
  char chr_rom_banks_num = ines_file_contents[5];
  if(flags6 & 8)
  {
    rom_fileoffset+=512;
  }
  for(int i=0; i< PRG_ROM_SIZE; i++)
  { 
    cpu_memory[0x8000 + i] = ines_file_contents[rom_fileoffset + i];
    cpu_memory[0xC000 + i] = ines_file_contents[rom_fileoffset + i];
  }
  cpu->cpu_memory=cpu_memory;
  rom_fileoffset+=(prg_rom_banks_num * PRG_ROM_SIZE);
  for(int i=0; i< CHR_ROM_SIZE; i++)
  { 
    ppu_memory[0x0000 + i] = ines_file_contents[rom_fileoffset + i];
  }
  ppu->ppu_memory=ppu_memory;
  draw(ppu->ppu_memory, CHR_ROM_SIZE); 
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

void ADC_update_status_register(signed char oldA)
{ 

  char oldA_sign_bit = oldA&128;
  char newA_sign_bit = (cpu->A)&128;
  switch_status_flag(NES_CARRY_FLAG, cpu->A!=oldA && ((cpu->A<oldA && newA_sign_bit==oldA_sign_bit) || (cpu->A>=oldA && newA_sign_bit!=oldA_sign_bit)));   
  switch_status_flag(NES_ZERO_FLAG,cpu->A == 0);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->A < 0);
  switch_status_flag(NES_OVERFLOW_FLAG,cpu->A < oldA); 
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
  switch_status_flag(NES_CARRY_FLAG,cpu->A >= data);
  switch_status_flag(NES_ZERO_FLAG,cpu->A == data);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->A < data); 
}
void CPY_update_status_register(unsigned char data)
{
  switch_status_flag(NES_CARRY_FLAG,cpu->Y >= data);
  switch_status_flag(NES_ZERO_FLAG,cpu->Y == data);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->Y < data); 
}
void CPX_update_status_register(unsigned char data)
{
  switch_status_flag(NES_CARRY_FLAG,cpu->X >= data);
  switch_status_flag(NES_ZERO_FLAG,cpu->X == data);
  switch_status_flag(NES_NEGATIVE_FLAG,cpu->X < data); 
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


void LDA_ptr(unsigned char* toOr)
{
  LDA(*toOr);  
}

void CMP_ptr(unsigned char* toOr)
{
  CMP_update_status_register(*toOr);  
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
  cpu->A+=toAdd;
  cpu->A+=(cpu->status&1); // add carry if set
  ADC_update_status_register(oldA); 
}

void SBC(unsigned char toSubtract)
{
  unsigned char oldA = cpu->A;
  cpu->A-=toSubtract;
  cpu->A-=(cpu->status&1); // subtract carry if set
  SBC_update_status_register(oldA); 
}

void STA(unsigned short address)
{
  if(address==0x2006)
  {
    ppu_write_address<<=8;
    ppu_write_address|=cpu->A; 
  }
  if(address==0x2007)
  {
    ppu->ppu_memory[ppu_write_address]=cpu->A;
    ppu_write_address+=1;
  }
  cpu->cpu_memory[address]=cpu->A;
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
  *operand_ptr = cpu->X;
}

void LDX(unsigned char* operand_ptr)
{
  cpu->X = *operand_ptr;
  set_negative_zero_flag(*operand_ptr);
}

void STY(unsigned char* operand_ptr)
{
  *operand_ptr = cpu->Y;
}

void LDY(unsigned char* operand_ptr)
{
  cpu->Y = *operand_ptr;
  set_negative_zero_flag(*operand_ptr);
}

void DEC(unsigned char* operand_ptr)
{
  *operand_ptr = *operand_ptr - 1;
  set_negative_zero_flag(*operand_ptr);
}

void BIT(unsigned char* operand_ptr)
{
  unsigned char data = *operand_ptr&cpu->A;
  switch_status_flag(NES_NEGATIVE_FLAG, (*operand_ptr)&NES_NEGATIVE_FLAG);
  switch_status_flag(NES_OVERFLOW_FLAG, (*operand_ptr)&NES_OVERFLOW_FLAG);
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

unsigned short get_short_from_chars(unsigned char high_byte, unsigned char low_byte)
{
  return ((high_byte<<8) | low_byte);
}

unsigned short get_short_from_cpu_memory(unsigned short mem_index)
{
  return get_short_from_chars(cpu->cpu_memory[mem_index + 1],cpu->cpu_memory[mem_index]);
}

void stack_push_char(unsigned char to_push)
{
  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)]=to_push;
  cpu->stack_pointer+=1;
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
  unsigned char value =  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer) +1];
  cpu->stack_pointer-=1;
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

void JSR()
{
  stack_push_short(cpu->PC + 3);  
  unsigned short newPC = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]); 
  print_instruction_info(3, "$%02x%02x", "JSR");
  cpu->PC=newPC -1;
}

void RTI()
{
  cpu->status =  stack_pull_char();
  cpu->PC = stack_pull_short();  
  print_instruction_info(1, "", "RTI");
}

void RTS()
{
  cpu->PC = stack_pull_short() ;  
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
  set_negative_zero_flag(cpu->stack_pointer);
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
  switch_status_flag(NES_NEGATIVE_FLAG, (signed char)operand < 0);
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

struct opcode opcodes[3][8];
struct address addresses[3][8];
struct opcode opcodes_singlebyte[256];

void set_single_byte_opcode_array()
{
  for(int i=0; i<256; i++)
  {
    opcodes_singlebyte[i].name = 0;
    opcodes_singlebyte[i].action = 0;
  }
  opcodes_singlebyte[0x08].name = "PHP";
  opcodes_singlebyte[0x08].action = PHP;
  opcodes_singlebyte[0x28].name = "PLP";
  opcodes_singlebyte[0x28].action = PLP;
  opcodes_singlebyte[0x48].name = "PHA";
  opcodes_singlebyte[0x48].action = PHA;
  opcodes_singlebyte[0x68].name = "PLA";
  opcodes_singlebyte[0x68].action = PLA;
  opcodes_singlebyte[0x88].name = "DEY";
  opcodes_singlebyte[0x88].action = DEY;
  opcodes_singlebyte[0xA8].name = "TAY";
  opcodes_singlebyte[0xA8].action = TAY;
  opcodes_singlebyte[0xC8].name = "INY";
  opcodes_singlebyte[0xC8].action = INY;
  opcodes_singlebyte[0xE8].name = "INX";
  opcodes_singlebyte[0xE8].action = INX;
  opcodes_singlebyte[0x18].name = "CLC";
  opcodes_singlebyte[0x18].action = CLC;
  opcodes_singlebyte[0x38].name = "SEC";
  opcodes_singlebyte[0x38].action = SEC;
  opcodes_singlebyte[0x58].name = "CLI";
  opcodes_singlebyte[0x58].action = CLI;
  opcodes_singlebyte[0x78].name = "SEI";
  opcodes_singlebyte[0x78].action = SEI;
  opcodes_singlebyte[0x98].name = "TYA";
  opcodes_singlebyte[0x98].action = TYA;
  opcodes_singlebyte[0xB8].name = "CLV";
  opcodes_singlebyte[0xB8].action = CLV;
  opcodes_singlebyte[0xD8].name = "CLD";
  opcodes_singlebyte[0xD8].action = CLD;
  opcodes_singlebyte[0xF8].name = "SED";
  opcodes_singlebyte[0xF8].action = SED;
  opcodes_singlebyte[0x8A].name = "TXA";
  opcodes_singlebyte[0x8A].action = TXA;
  opcodes_singlebyte[0x9A].name = "TXS";
  opcodes_singlebyte[0x9A].action = TXS;
  opcodes_singlebyte[0xAA].name = "TAX";
  opcodes_singlebyte[0xAA].action = TAX;
  opcodes_singlebyte[0xBA].name = "TSX";
  opcodes_singlebyte[0xBA].action = TSX;
  opcodes_singlebyte[0xCA].name = "DEX";
  opcodes_singlebyte[0xCA].action = DEX;
  opcodes_singlebyte[0xEA].name = "NOP";
  opcodes_singlebyte[0xEA].action = NOP;
}

void set_opcode_array()
{
  opcodes[0][1].name = "BIT";
  opcodes[0][1].action = BIT;
  opcodes[0][2].name = "JMP";
  opcodes[0][2].action = JMP;
  opcodes[0][3].name = "JMP";
  opcodes[0][3].action = JMP;
  opcodes[0][4].name = "STY";
  opcodes[0][4].action = STY;
  opcodes[0][5].name = "LDY";
  opcodes[0][5].action = LDY;
  opcodes[0][6].name = "CPY";
  opcodes[0][6].action = CPY;
  opcodes[0][7].name = "CPX";
  opcodes[0][7].action = CPX;

  opcodes[1][0].name = "ORA";
  opcodes[1][0].action = ORA_ptr;
  opcodes[1][1].name = "AND";
  opcodes[1][1].action = AND_ptr;
  opcodes[1][2].name = "EOR";
  opcodes[1][2].action = EOR_ptr;
  opcodes[1][3].name = "ADC";
  opcodes[1][3].action = ADC_ptr;
  opcodes[1][4].name = "STA";
  opcodes[1][4].action = STA_ptr;
  opcodes[1][5].name = "LDA";
  opcodes[1][5].action = LDA_ptr;
  opcodes[1][6].name = "CMP";
  opcodes[1][6].action = CMP_ptr;
  opcodes[1][7].name = "SBC";
  opcodes[1][7].action = SBC_ptr;

  opcodes[2][0].name = "ASL";
  opcodes[2][0].action = ASL;
  opcodes[2][1].name = "ROL";
  opcodes[2][1].action = ROL;
  opcodes[2][2].name = "LSR";
  opcodes[2][2].action = LSR;
  opcodes[2][3].name = "ROR";
  opcodes[2][3].action = ROR;
  opcodes[2][4].name = "STX";
  opcodes[2][4].action = STX;
  opcodes[2][5].name = "LDX";
  opcodes[2][5].action = LDX;
  opcodes[2][6].name = "DEC";
  opcodes[2][6].action = DEC;
  opcodes[2][7].name = "INC";
  opcodes[2][7].action = INC;
 
  addresses[0][0].program_counter_increment = 2;
  addresses[0][0].get_operand_ptr = get_immediate_operand_ptr;
  addresses[0][0].address_info = "#%02x";
  addresses[0][1].program_counter_increment = 2;
  addresses[0][1].get_operand_ptr = get_zeropage_operand_ptr;
  addresses[0][1].address_info = "$%02x";
  addresses[0][2].program_counter_increment = 1;
  addresses[0][2].get_operand_ptr = get_accumulator_operand_ptr;
  addresses[0][2].address_info = "A";
  addresses[0][3].program_counter_increment = 3;
  addresses[0][3].get_operand_ptr = get_absolute_operand_ptr;
  addresses[0][3].address_info = "$%02x%02x";
  addresses[0][5].program_counter_increment = 2;
  addresses[0][5].get_operand_ptr = get_zeropage_X_operand_ptr;
  addresses[0][5].address_info = "$%02x,X";
  addresses[0][7].program_counter_increment = 3;
  addresses[0][7].get_operand_ptr = get_absolute_X_operand_ptr;
  addresses[0][7].address_info = "$%02x%02x,X";


  addresses[1][0].program_counter_increment = 2;
  addresses[1][0].get_operand_ptr = get_indexed_indirect_X_operand_ptr;
  addresses[1][0].address_info = "($%02x,X)";
  addresses[1][1].program_counter_increment = 2;
  addresses[1][1].get_operand_ptr = get_zeropage_operand_ptr;
  addresses[1][1].address_info = "$%02x";
  addresses[1][2].program_counter_increment = 2;
  addresses[1][2].get_operand_ptr = get_immediate_operand_ptr;
  addresses[1][2].address_info = "#%02x";
  addresses[1][3].program_counter_increment = 3;
  addresses[1][3].get_operand_ptr = get_absolute_operand_ptr;
  addresses[1][3].address_info = "$%02x%02x";
  addresses[1][4].program_counter_increment = 2;
  addresses[1][4].get_operand_ptr = get_indirect_indexed_Y_operand_ptr;
  addresses[1][4].address_info = "($%02x),Y";
  addresses[1][5].program_counter_increment = 2;
  addresses[1][5].get_operand_ptr = get_zeropage_X_operand_ptr;
  addresses[1][5].address_info = "$%02x,X";
  addresses[1][6].program_counter_increment = 3;
  addresses[1][6].get_operand_ptr = get_absolute_Y_operand_ptr;
  addresses[1][6].address_info = "$%02x%02x,Y";
  addresses[1][7].program_counter_increment = 3;
  addresses[1][7].get_operand_ptr = get_absolute_X_operand_ptr;
  addresses[1][7].address_info = "$%02x%02x,X"; 

  addresses[2][0].program_counter_increment = 2;
  addresses[2][0].get_operand_ptr = get_immediate_operand_ptr;
  addresses[2][0].address_info = "#%02x";
  addresses[2][1].program_counter_increment = 2;
  addresses[2][1].get_operand_ptr = get_zeropage_operand_ptr;
  addresses[2][1].address_info = "$%02x";
  addresses[2][2].program_counter_increment = 1;
  addresses[2][2].get_operand_ptr = get_accumulator_operand_ptr;
  addresses[2][2].address_info = "A";
  addresses[2][3].program_counter_increment = 3;
  addresses[2][3].get_operand_ptr = get_absolute_operand_ptr;
  addresses[2][3].address_info = "$%02x%02x";
  addresses[2][5].program_counter_increment = 2;
  addresses[2][5].get_operand_ptr = get_zeropage_X_operand_ptr;
  addresses[2][5].address_info = "$%02x,X";
  addresses[2][7].program_counter_increment = 3;
  addresses[2][7].get_operand_ptr = get_absolute_X_operand_ptr;
  addresses[2][7].address_info = "$%02x%02x,X";

}

void print_instruction_info(char program_counter_increment, char* address_info, char* opcode_info)
{
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
    printf("%02x: %s %s\n", cpu->PC, opcode_info, address_mode_info);
    free(address_mode_info);
}

void print_instruction_info_from_context(char program_counter_increment, char opcode_context, char addressing_mode, char opcode)
{
    char* opcode_info = (char*)malloc(10 * sizeof(char));
    char* address_info = addresses[opcode_context][addressing_mode].address_info;
    strcpy(opcode_info, opcodes[opcode_context][opcode].name);
    print_instruction_info(program_counter_increment, address_info, opcode_info);
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
  char opcode_context = current_opcode&OPCODE_CONTEXT_MASK;
  char opcode = (current_opcode&OPCODE_MASK)>>5;
  char addressing_mode = (current_opcode&ADDRESSING_MODE_MASK)>>2;
  unsigned char* operand_ptr = addresses[opcode_context][addressing_mode].get_operand_ptr();
  char program_counter_increment = addresses[opcode_context][addressing_mode].program_counter_increment;
       
  print_instruction_info_from_context( program_counter_increment, opcode_context, addressing_mode, opcode);
  opcodes[opcode_context][opcode].action(operand_ptr);
  if(strcmp("JMP", opcodes[opcode_context][opcode].name))
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
  set_opcode_array();
  set_single_byte_opcode_array();
  cpu->PC = get_short_from_cpu_memory(0xfffc); 
  cpu->cpu_memory[0x2002] = 128;
  int run_instructions_no_prompt = 0;
  char arg2 = ' ';
  int arg1 = 0;
  int draw_screen_count = 2400;
  while(1==1)
  {
    char input[20];
    char raw_input[20];
    clear(input, 20);
    if(run_instructions_no_prompt==0)
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
          printf("cpu->X = %d (%x), cpu->Y = %d (%x), cpu->A = %d (%x), cpu->PC = %d (%x), cpu->status = %d (%x) \n", cpu->X, cpu->X, cpu->Y, cpu->Y, cpu->A, cpu->A, cpu->PC, cpu->PC, cpu->status, cpu->status);
        }
        if(strcmp("run", input)==0)
        {
          run_instructions_no_prompt=arg1;
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
    }
    if(!exceptional_instruction)
    { 
      if(opcodes_singlebyte[current_opcode].action)
      {
        unsigned char* dummy_ptr = 0;
        print_instruction_info(1, "", opcodes_singlebyte[current_opcode].name);
        opcodes_singlebyte[current_opcode].action(dummy_ptr);      
        increment_PC(1);
      }
      else
      {
        if((current_opcode&COND_BRANCH_MASK)==16)
        {
          conditional_branch_instruction(current_opcode);
        }
        else
        { 
          standard_instruction(current_opcode);
        }
      }
    }
    draw_screen_count--;
    if(draw_screen_count==0)
    {
      draw(ppu->ppu_memory, CHR_ROM_SIZE);
      draw_screen_count=2400;
    }
    
  } 
}

