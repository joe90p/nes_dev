#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void STX(unsigned char address)
{
  cpu->cpu_memory[address] = cpu->X;
}

void LDX(unsigned char address)
{
  cpu->X = cpu->cpu_memory[address];
  switch_status_flag(NES_NEGATIVE_FLAG, (signed char)cpu->X < 0);
  switch_status_flag(NES_ZERO_FLAG, cpu->X==0);
}

void STX_ptr(unsigned char* address_ptr)
{
  STX(*address_ptr); 
}

void LDX_ptr(unsigned char* address_ptr)
{
  LDX(*address_ptr);
}

void get_data_at_address_do_opcode(short address, opcode_action_type opcode_action)
{
  unsigned char data = cpu->cpu_memory[address];
  opcode_action(data);
}

void run_rom()
{
  cpu->PC = cpu->cpu_memory[0xfffc]<<8 | cpu->cpu_memory[0xfffd]>>8;
  printf("first 3 bytes at RESET\n"); 

  for(int j=0; j < 3; j++)
  {
    printf("%02x\n", cpu->cpu_memory[cpu->PC + j]);
  }
  //unsigned char oldA;
  char opcode_context_mask = 3;
  char opcode_mask = 224;
  char addressing_mode_mask = 28;
  char program_counter_increment;
  unsigned char* operand_ptr =0;
  unsigned short address;


  struct opcode opcodes[8];
  opcodes[0].name = "ASL";
  opcodes[0].action = ASL;
  opcodes[1].name = "ROL";
  opcodes[1].action = ROL;
  opcodes[2].name = "LSR";
  opcodes[2].action = LSR;
  opcodes[3].name = "ROR";
  opcodes[3].action = ROR;
  opcodes[4].name = "STX";
  opcodes[4].action = STX_ptr;
  opcodes[5].name = "LDX";
  opcodes[5].action = LDX_ptr;

  for(int k=0; k < 5; k++)
  {
    char current_opcode = cpu->cpu_memory[cpu->PC];
    char opcode_context = current_opcode&opcode_context_mask;
    char opcode = (current_opcode&opcode_mask)>>5;
    char addressing_mode = (current_opcode&addressing_mode_mask)>>2;
    char* address_mode_info = (char*)malloc(10 * sizeof(char));
    char* opcode_info = (char*)malloc(10 * sizeof(char));
    switch(opcode_context)
    {
      case 1:
        switch(addressing_mode)
        {
          case 0:
            //(zero page,X);
            address = get_indexed_indirect_X(cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 2;
            sprintf(address_mode_info,"($%02x,X)", cpu->cpu_memory[cpu->PC + 1]);
            break;
          case 1:
            //zero page
            address = get_zeropage_address(cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 2;
            sprintf(address_mode_info,"$%02x", cpu->cpu_memory[cpu->PC + 1]);
            break;
          case 2:
            //immediate
            address = cpu->PC + 1;
            program_counter_increment = 2;
            sprintf(address_mode_info,"#%02x", cpu->cpu_memory[cpu->PC + 1]);
            break;
          case 3:
            //absolute
            address = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 3;
            sprintf(address_mode_info,"$%02x%02x", cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC+1]);
            break;
          case 4:
            //(zero page),Y
            address = get_indirect_indexed_Y(cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 2;
            sprintf(address_mode_info,"($%02x),Y", cpu->cpu_memory[cpu->PC + 1]);
            break;
          case 5:
            //zero page,X
            address = get_zeropage_X_address(cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 2;
            sprintf(address_mode_info,"$%02x,X", cpu->cpu_memory[cpu->PC + 1]);
            break;
          case 6:
            //absolute,Y
            address = get_absolute_address_Y(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 3;
            sprintf(address_mode_info,"$%02x%02x,Y", cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC + 1]); 
            break;
          case 7:
            //absolute,X
            address = get_absolute_address_X(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 3;
            sprintf(address_mode_info,"$%02x%02x,X", cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC + 1]);
            break;
        }
        switch(opcode)
        {
          case 0:
            get_data_at_address_do_opcode(address, ORA);      
            strcpy(opcode_info, "ORA");
            break;
          case 1:
            AND(cpu->cpu_memory[address]);
            strcpy(opcode_info, "AND");
            break;
          case 2:
            EOR(cpu->cpu_memory[address]);
            strcpy(opcode_info, "EOR");
            break;  
          case 3:
            ADC(cpu->cpu_memory[address]);
            strcpy(opcode_info, "ADC");
            break;
          case 4:
            STA(address);
            strcpy(opcode_info, "STA");
            break;
          case 5:
            LDA(cpu->cpu_memory[address]);
            strcpy(opcode_info, "LDA");
            break;
          case 6:
            CMP_update_status_register(cpu->cpu_memory[address]);
            strcpy(opcode_info, "CMP");
            break;
          case 7:
            SBC(cpu->cpu_memory[address]);
            strcpy(opcode_info, "SBC");
            break;
        }
        // print out opcode info
        printf("%02x: %s %s\n", cpu->PC, opcode_info, address_mode_info);      
        cpu->PC+=program_counter_increment;
        break;
      case 2:
         
        switch(addressing_mode)
        {
          case 0:
            //immediate
            address = cpu->PC + 1;
            program_counter_increment = 2;
            sprintf(address_mode_info,"#%02x", cpu->cpu_memory[cpu->PC + 1]);
            operand_ptr = &cpu->cpu_memory[address];
            break;          
          case 1:
            //zero page
            address = get_zeropage_address(cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 2;
            sprintf(address_mode_info,"$%02x", cpu->cpu_memory[cpu->PC + 1]);
            operand_ptr = &cpu->cpu_memory[address];
            break;
          case 2:
            //accumulator
            program_counter_increment = 1;
            sprintf(address_mode_info, "A");
            operand_ptr = (unsigned char*)&cpu->A;
            break; 
          case 3:
            //absolute
            address = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 3;
            sprintf(address_mode_info,"$%02x%02x", cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC+1]);
            operand_ptr = &cpu->cpu_memory[address];
            break; 
          case 5:
            //zero page,X
            address = get_zeropage_X_address(cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 2;
            sprintf(address_mode_info,"$%02x,X", cpu->cpu_memory[cpu->PC + 1]); 
            operand_ptr = &cpu->cpu_memory[address];
            break;
          case 7:
            //absolute,X
            address = get_absolute_address_X(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
            program_counter_increment = 3;
            sprintf(address_mode_info,"$%02x%02x,X", cpu->cpu_memory[cpu->PC + 2], cpu->cpu_memory[cpu->PC + 1]);
            operand_ptr = &cpu->cpu_memory[address];
            break;
        }

          opcodes[opcode].action(operand_ptr);
          strcpy(opcode_info, opcodes[opcode].name);
    }
    //clear string memory
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
  } 
}

