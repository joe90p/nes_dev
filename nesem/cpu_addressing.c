#include </home/phil/git/nes_dev/nesem/cpu_addressing.h>

unsigned short get_absolute_address(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  unsigned short newAddress =  get_address_input_upper_byte<<8 | get_address_input_lower_byte;
  return newAddress;
}

unsigned short get_absolute_address_indexed(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte, unsigned char index)
{
  unsigned short newAddress =  get_absolute_address(get_address_input_upper_byte, get_address_input_lower_byte)  + index;
  unsigned char newAddress_upper =  ((unsigned char*)(&newAddress))[1];
  if(newAddress_upper != get_address_input_upper_byte)
  {
    //increment cycles by 1 - page boundary crossed 
    cpu->cycles+=1;
  }
  return newAddress;
}
unsigned short get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
    return get_absolute_address_indexed(get_address_input_upper_byte, get_address_input_lower_byte, cpu->X);
}

unsigned short get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte)
{
  return get_absolute_address_indexed(get_address_input_upper_byte, get_address_input_lower_byte, cpu->Y);
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
  return get_absolute_address_indexed(cpu->cpu_memory[address_input_high ],cpu->cpu_memory[get_address_input] , cpu->Y);
}
unsigned char* get_immediate_operand_ptr()
{
  return &cpu->cpu_memory[cpu->PC + 1];
}

unsigned char* get_zeropage_operand_ptr()
{
  unsigned char address = get_zeropage_address(cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[address];
}         

unsigned char* get_branch_operand_ptr()
{
  signed char offset = cpu->cpu_memory[cpu->PC +1] + 2;
  unsigned short address = cpu->PC + offset; 
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

unsigned char* get_absolute_indirect_operand_ptr()
{
  unsigned short intermediate_address = get_absolute_address(cpu->cpu_memory[cpu->PC+2], cpu->cpu_memory[cpu->PC + 1]);
  return &cpu->cpu_memory[get_indirect(intermediate_address)];
}
