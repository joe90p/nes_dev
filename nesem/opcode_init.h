#include </home/phil/git/nes_dev/nesem/cpu_addressing.h>
#include </home/phil/git/nes_dev/nesem/cpu_instructions.h>

struct address
{
  char program_counter_increment;
  get_operand_ptr_type get_operand_ptr;
  char* address_info;
  char inc_pc;
};
struct opcode
{
  opcode_action action;
  char* name;
  unsigned char address_mode;
  unsigned char cycles;
};
void set_opcodes(struct opcode** opcodes, struct address** addresses);
