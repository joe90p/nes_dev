#include <stdio.h>
#include <stdlib.h>
#define NES_CARRY_FLAG 1
#define NES_ZERO_FLAG 2
#define NES_OVERFLOW_FLAG 64
#define NES_NEGATIVE_FLAG 128

struct NES_CPU
{
  unsigned char* cpu_memory;
  signed char A;
  unsigned char X;
  unsigned char Y;
  unsigned char status;
  unsigned short PC;
}; 
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;
typedef void (*opcode_action_type)(unsigned char a); 
void run_rom();
typedef void (*opcode_action)(unsigned char* ptr);
typedef unsigned char* (*get_operand_ptr_type)();
struct address
{
  char program_counter_increment;
  get_operand_ptr_type get_operand_ptr;
  char* address_info;
};
struct opcode
{
  opcode_action action;
  char* name;
};

unsigned short get_absolute_address(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_zeropage_address(unsigned char get_address_input);
unsigned short get_zeropage_X_address(unsigned char get_address_input);
unsigned short get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_indexed_indirect_X(unsigned char get_address_input);
unsigned short get_indirect_indexed_Y(unsigned char get_address_input);
void ADC_update_status_register(signed char oldA);
void SBC_update_status_register(signed char oldA);
void ORA_update_status_register();
void CMP_update_status_register(unsigned char data);
void ADC(unsigned char);
void ORA(unsigned char);
void AND(unsigned char);
void EOR(unsigned char);
void LDA(unsigned char);
void STA(unsigned short);
void SBC(unsigned char);
void shift_left(unsigned char*, char);
void shift_right(unsigned char*, char);
void STX(unsigned char*);
void LDX(unsigned char*);
void get_data_at_address_do_opcode(short address, opcode_action_type);
