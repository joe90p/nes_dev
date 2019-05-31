#include <stdio.h>
#include <stdlib.h>
#define NES_CARRY_FLAG 1
#define NES_ZERO_FLAG 2
#define NES_INTERRUPT_DISABLE_FLAG 4
#define NES_DECIMAL_MODE_FLAG 8
#define NES_BREAK_FLAG 16
#define NES_OVERFLOW_FLAG 64
#define NES_NEGATIVE_FLAG 128
#define STACK_BOTTOM 256
#define OPCODE_CONTEXT_MASK  3
#define OPCODE_MASK  224
#define ADDRESSING_MODE_MASK  28
#define COND_BRANCH_MASK  31 
#define PRG_ROM_SIZE 0x4000
#define CHR_ROM_SIZE 0x2000
#define PPU_CONTROL_CPU_ADDRESS 0x2000
struct NES_CPU
{
  unsigned char* cpu_memory;
  signed char A;
  unsigned char X;
  unsigned char Y;
  unsigned char status;
  unsigned short PC;
  unsigned char old_PC;
  unsigned char stack_pointer;
}; 
struct NES_IO
{
  unsigned char controller1;
};
struct NES_PPU
{
  unsigned char* ppu_memory;
  unsigned char* spr_ram;
  unsigned char status;
  unsigned char control;
};
void load_rom();
unsigned char* ines_file_contents;
struct NES_CPU* cpu;
struct NES_PPU* ppu;
struct NES_IO* io;
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
  unsigned char address_mode;
};

unsigned short get_absolute_address(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_zeropage_address(unsigned char get_address_input);
unsigned short get_zeropage_X_address(unsigned char get_address_input);
unsigned short get_zeropage_Y_address(unsigned char get_address_input);
unsigned short get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_indexed_indirect_X(unsigned char get_address_input);
unsigned short get_indirect_indexed_Y(unsigned short get_address_input);
unsigned short get_ppu_write_address();
void set_ppu_write_address(unsigned short address);
void ADC_update_status_register(signed char oldA, unsigned char overflow);
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
void INC(unsigned char*);
void DEC(unsigned char*);
void BIT(unsigned char*);
void JMP(unsigned char*);
void STY(unsigned char*);
void LDY(unsigned char*);
void get_data_at_address_do_opcode(short address, opcode_action_type);
void set_negative_zero_flag(unsigned char);
void CPX_update_status_register(unsigned char data);
void CPY_update_status_register(unsigned char data);
unsigned char test_flag_and_branch(unsigned char flag, unsigned char equalTo, unsigned char offset);
void BRK();
void JSR();
void RTI();
void RTS();

void PHP();
void PHA();
void PLA();
void PLP();
void DEY();
void TAY();
void INX();
void INY();
void CLC();
void SEC();
void CLI();
void SEI();
void TAY();
void TYA();
void CLV();
void CLD();
void SED();
void TXS();
void TXA();
void TAX();
void TSX();
void DEX();
void NOP();
void stack_push_char(unsigned char);
void increment_PC(signed char);
