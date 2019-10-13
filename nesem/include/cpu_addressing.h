#ifndef CPU_ADDRESSING_H
#define CPU_ADDRESSING_H
unsigned char* get_immediate_operand_ptr();
unsigned char* get_zeropage_operand_ptr();
unsigned char* get_accumulator_operand_ptr();
unsigned char* get_absolute_operand_ptr();
unsigned char* get_zeropage_X_operand_ptr();
unsigned char* get_zeropage_Y_operand_ptr();
unsigned char* get_dummy_operand_ptr();
unsigned char* get_absolute_X_operand_ptr();
unsigned char* get_absolute_Y_operand_ptr();
unsigned char* get_indexed_indirect_X_operand_ptr();
unsigned char* get_indirect_indexed_Y_operand_ptr();
unsigned char* get_absolute_indirect_operand_ptr();
unsigned char* get_branch_operand_ptr();
unsigned char* get_absolute_X_nocycle_operand_ptr();
unsigned char* get_absolute_Y_nocycle_operand_ptr();
#endif
