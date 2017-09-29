#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;

void run_rom();

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
  unsigned char address = get_zeropage_address(cpu->cpu_memory[cpu->PC + 1]);
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
  unsigned char address = get_indirect_indexed_Y(cpu->cpu_memory[cpu->PC + 1]);
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
  STA(*toOr);  
}

void LDA_ptr(unsigned char* toOr)
{
  LDA(*toOr);  
}

void CMP_ptr(unsigned char* toOr)
{
  CMP_update_status_register(*toOr);  
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
  switch_status_flag(NES_NEGATIVE_FLAG, (signed char)cpu->X < 0);
  switch_status_flag(NES_ZERO_FLAG, cpu->X==0);
}

void get_data_at_address_do_opcode(short address, opcode_action_type opcode_action)
{
  unsigned char data = cpu->cpu_memory[address];
  opcode_action(data);
}
//struct address addresses2[8];
struct opcode opcodes[2][8];
struct address addresses[2][8];
//struct opcode opcodes2[8];

void set_opcode_array()
{
  opcodes[0][0].name = "ORA";
  opcodes[0][0].action = ORA_ptr;
  opcodes[0][1].name = "AND";
  opcodes[0][1].action = AND_ptr;
  opcodes[0][2].name = "EOR";
  opcodes[0][2].action = EOR_ptr;
  opcodes[0][3].name = "ADC";
  opcodes[0][3].action = ADC_ptr;
  opcodes[0][4].name = "STA";
  opcodes[0][4].action = STA_ptr;
  opcodes[0][5].name = "LDA";
  opcodes[0][5].action = LDA_ptr;
  opcodes[0][6].name = "CMP";
  opcodes[0][6].action = CMP_ptr;
  opcodes[0][7].name = "SBC";
  opcodes[0][7].action = SBC_ptr;

  opcodes[1][0].name = "ASL";
  opcodes[1][0].action = ASL;
  opcodes[1][1].name = "ROL";
  opcodes[1][1].action = ROL;
  opcodes[1][2].name = "LSR";
  opcodes[1][2].action = LSR;
  opcodes[1][3].name = "ROR";
  opcodes[1][3].action = ROR;
  opcodes[1][4].name = "STX";
  opcodes[1][4].action = STX;
  opcodes[1][5].name = "LDX";
  opcodes[1][5].action = LDX;
  
  addresses[1][0].program_counter_increment = 2;
  addresses[1][0].get_operand_ptr = get_immediate_operand_ptr;
  addresses[1][0].address_info = "#%02x";
  addresses[1][1].program_counter_increment = 2;
  addresses[1][1].get_operand_ptr = get_zeropage_operand_ptr;
  addresses[1][1].address_info = "$%02x";
  addresses[1][2].program_counter_increment = 1;
  addresses[1][2].get_operand_ptr = get_accumulator_operand_ptr;
  addresses[1][2].address_info = "A";
  addresses[1][3].program_counter_increment = 3;
  addresses[1][3].get_operand_ptr = get_absolute_operand_ptr;
  addresses[1][3].address_info = "$%02x%02x";
  addresses[1][5].program_counter_increment = 2;
  addresses[1][5].get_operand_ptr = get_zeropage_X_operand_ptr;
  addresses[1][5].address_info = "$%02x,X";
  addresses[1][7].program_counter_increment = 3;
  addresses[1][7].get_operand_ptr = get_absolute_X_operand_ptr;
  addresses[1][7].address_info = "$%02x%02x,X";

  addresses[0][0].program_counter_increment = 2;
  addresses[0][0].get_operand_ptr = get_indexed_indirect_X_operand_ptr;
  addresses[0][0].address_info = "($%02x,X)";
  addresses[0][1].program_counter_increment = 2;
  addresses[0][1].get_operand_ptr = get_zeropage_operand_ptr;
  addresses[0][1].address_info = "$%02x";
  addresses[0][2].program_counter_increment = 2;
  addresses[0][2].get_operand_ptr = get_immediate_operand_ptr;
  addresses[0][2].address_info = "#%02x";
  addresses[0][3].program_counter_increment = 3;
  addresses[0][3].get_operand_ptr = get_absolute_operand_ptr;
  addresses[0][3].address_info = "$%02x%02x";
  addresses[0][4].program_counter_increment = 2;
  addresses[0][4].get_operand_ptr = get_indirect_indexed_Y_operand_ptr;
  addresses[0][4].address_info = "($%02x),Y";
  addresses[0][5].program_counter_increment = 2;
  addresses[0][5].get_operand_ptr = get_zeropage_X_operand_ptr;
  addresses[0][5].address_info = "$%02x,X";
  addresses[0][6].program_counter_increment = 3;
  addresses[0][6].get_operand_ptr = get_absolute_Y_operand_ptr;
  addresses[0][6].address_info = "$%02x%02x,Y";
  addresses[0][7].program_counter_increment = 3;
  addresses[0][7].get_operand_ptr = get_absolute_X_operand_ptr;
  addresses[0][7].address_info = "$%02x%02x,X"; 
}
void run_rom()
{
  set_opcode_array();
  cpu->PC = cpu->cpu_memory[0xfffc]<<8 | cpu->cpu_memory[0xfffd]>>8;
  printf("first 3 bytes at RESET\n"); 

  for(int j=0; j < 3; j++)
  {
    printf("%02x\n", cpu->cpu_memory[cpu->PC + j]);
  }
  char opcode_context_mask = 3;
  char opcode_mask = 224;
  char addressing_mode_mask = 28;
  char program_counter_increment;
  unsigned char* operand_ptr =0;
  
  for(int k=0; k < 5; k++)
  {
    char current_opcode = cpu->cpu_memory[cpu->PC];
    
    char opcode_context = current_opcode&opcode_context_mask;
    char opcode = (current_opcode&opcode_mask)>>5;
    char addressing_mode = (current_opcode&addressing_mode_mask)>>2;
    char* address_mode_info = (char*)malloc(10 * sizeof(char));
    char* opcode_info = (char*)malloc(10 * sizeof(char));

    operand_ptr = addresses[opcode_context-1][addressing_mode].get_operand_ptr();
    char* address_info = addresses[opcode_context-1][addressing_mode].address_info;
    program_counter_increment = addresses[opcode_context-1][addressing_mode].program_counter_increment;
 
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
    opcodes[opcode_context-1][opcode].action(operand_ptr);
    strcpy(opcode_info, opcodes[opcode_context-1][opcode].name);


    printf("%02x: %s %s\n", cpu->PC, opcode_info, address_mode_info);
    free(opcode_info);  
    free(address_mode_info);
 
    switch(cpu->cpu_memory[cpu->PC])
    {
      
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
    cpu->PC+=program_counter_increment;
  } 
}

