#include <stdio.h>
#include <stdlib.h>

void load_test_rom()
{
  unsigned char* cpu_memory = malloc(0x10000);
  
  for(int i=0; i< 0x4000; i++)
  { 
    cpu_memory[0x8000 + i] = 0;
    cpu_memory[0xC000 + i] = 0;
  }
  cpu->cpu_memory=cpu_memory;
  cpu->X = 3;
  cpu->Y = 5;
}

void assert(char bool_result)
{
   if(bool_result)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }

}
void test_get_absolute_address()
{
  printf("test test_get_absolute_address ");
  unsigned short address = get_absolute_address(0x21, 0x45);
  assert(address==0x2145);
}

void test_get_absolute_address_X()
{
  printf("test test_get_absolute_address_X ");
  unsigned short address = get_absolute_address_X(0x21, 0x45);
  assert(address==0x2148);
}

void test_get_absolute_address_Y()
{
  printf("test test_get_absolute_address_Y ");
  unsigned short address = get_absolute_address_Y(0x21, 0x45);
  assert(address==0x214a);
}


void test_get_indexed_indirect_X()
{
  printf("test test_get_indexed_indirect_X ");
  cpu->cpu_memory[0x4A]=0x83;
  unsigned short address = get_indexed_indirect_X(0x47);
  assert(address==0x0083);
}

void test_get_indirect_indexed_Y()
{
  printf("test test_getindirect_indexed_Y ");
  cpu->cpu_memory[0x23]=0x87;
  unsigned short address = get_indirect_indexed_Y(0x23);
  assert(address==0x008C);
}

void ADC_carry_status_flagged_result_less()
{
  printf("test ADC_carry_status_flagged_result_less "); 

  cpu->A=-125;  
  cpu->status=0;
  ADC_update_status_register(1);
  char not_expect_carry = cpu->status;
  cpu->status=0;
  ADC_update_status_register(-1);
  char expect_carry = cpu->status;
  
  cpu->status=0;
  cpu->A=0;
  ADC_update_status_register(1);
  char expect_carry_2 = cpu->status;
  
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG  &&  (expect_carry_2&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void ADC_carry_status_flagged_result_greater()
{
  printf("test ADC_carry_status_flagged_result_greater "); 

  cpu->A=2;  
  cpu->status=0;
  ADC_update_status_register(1);
  char not_expect_carry = cpu->status;
  cpu->status=0;
  ADC_update_status_register(-1);
  char expect_carry = cpu->status;
  
  cpu->status=0;
  cpu->A=0;
  ADC_update_status_register(-1);
  char expect_carry_2 = cpu->status;
  
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG  &&  (expect_carry_2&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void ADC_carry_status_flagged_result_equal()
{
  printf("test ADC_carry_status_flagged_result_equal "); 
  
  cpu->A=0;  
  cpu->status=0;
  ADC_update_status_register(0);
  char not_expect_carry = cpu->status;
 
  cpu->A=-2;  
  cpu->status=0;
  ADC_update_status_register(-2);
  char not_expect_carry_2 = cpu->status;

  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (not_expect_carry_2&NES_CARRY_FLAG)==0); 
}

void SBC_carry_status_flagged_result_less()
{
  printf("test SBC_carry_status_flagged_result_less "); 
  cpu->A=-125;  
  cpu->status=0;
  SBC_update_status_register(1);
  char expect_carry = cpu->status;
  cpu->status=0;
  SBC_update_status_register(-1);
  char not_expect_carry = cpu->status;
  
  cpu->status=0;
  cpu->A=-2;
  SBC_update_status_register(-1);
  char not_expect_carry_2 = cpu->status;
  
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG  &&  (not_expect_carry_2&NES_CARRY_FLAG)==0); 
}

void SBC_carry_status_flagged_result_greater()
{
  printf("test SBC_carry_status_flagged_result_greater "); 
  cpu->A=2;  
  cpu->status=0;
  SBC_update_status_register(1);
  char expect_carry = cpu->status;
  cpu->status=0;
  SBC_update_status_register(-1);
  char not_expect_carry = cpu->status;
  
  cpu->status=0;
  cpu->A=-1;
  SBC_update_status_register(-2);
  char expect_carry_2 = cpu->status;
  
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG  &&  (expect_carry_2&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void SBC_carry_status_flagged_result_equal()
{
  printf("test SBC_carry_status_flagged_result_equal "); 
  cpu->A=0;  
  cpu->status=0;
  SBC_update_status_register(0);
  char not_expect_carry = cpu->status;
 
  cpu->A=-2;  
  cpu->status=0;
  SBC_update_status_register(-2);
  char not_expect_carry_2 = cpu->status;

  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (not_expect_carry_2&NES_CARRY_FLAG)==0); 
}

void ADC_zero_status_flagged()
{
  printf("test ADC_zero_status_flagged "); 
  cpu->A=0;

  ADC_update_status_register(5);
  char expect_zero = cpu->status;
  cpu->A=1;
  ADC_update_status_register(5);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void ADC_overflow_status_flagged()
{
  printf("test ADC_overflow_status_flagged "); 
  cpu->A=3;

  ADC_update_status_register(127);
  char expect_overflow = cpu->status;
  ADC_update_status_register(2);
  char not_expect_overflow = cpu->status; 
  assert((not_expect_overflow&NES_OVERFLOW_FLAG)==0 && (expect_overflow&NES_OVERFLOW_FLAG)==NES_OVERFLOW_FLAG); 
}

void ADC_negative_status_flagged()
{
  printf("test ADC_negative_status_flagged "); 
  cpu->A=-1;

  ADC_update_status_register(2);
  char expect_negative = cpu->status;
  cpu->A=1; 
  ADC_update_status_register(2);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void ADC_get_expected_overflow()
{
  printf("ADC_get_expected_overflow ");
  cpu->status=0;
  cpu->A=126;
  ADC(3); 
  assert(cpu->A==-127);
}

void ADC_get_expected_happy_path()
{
  printf("ADC_get_expected_happy_path ");
  cpu->status=0;
  cpu->A=126;
  ADC(1); 
  assert(cpu->A==127);
}

void SBC_carry_status_flagged()
{
  printf("test SBC_carry_status_flagged "); 
  cpu->A=126; 
  SBC_update_status_register(125);
  char expect_carry = cpu->status;
  SBC_update_status_register(127);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void SBC_zero_status_flagged()
{
  printf("test SBC_zero_status_flagged "); 
  cpu->A=0;
  
  SBC_update_status_register(5);
  char expect_zero = cpu->status;
  cpu->A=1;
  SBC_update_status_register(5);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && 
           (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void SBC_overflow_status_flagged()
{
  printf("test SBC_overflow_status_flagged "); 
  cpu->A=126; 
  SBC_update_status_register(125);
  char expect_overflow = cpu->status;
  SBC_update_status_register(127);
  char not_expect_overflow = cpu->status; 
  assert((not_expect_overflow&NES_OVERFLOW_FLAG)==0 && (expect_overflow&NES_OVERFLOW_FLAG)==NES_OVERFLOW_FLAG); 
}

void SBC_negative_status_flagged()
{
  printf("test SBC_negative_status_flagged "); 
  cpu->A=-1;
  SBC_update_status_register(2);
  char expect_negative = cpu->status;
  cpu->A=1; 
  SBC_update_status_register(2);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void SBC_get_expected_overflow()
{
  printf("SBC_get_expected_overflow ");
  cpu->status=0;//clear carry flag
  cpu->A=-128;
  SBC(1);
  printf("%d", cpu->A); 
  assert(cpu->A==127);
}

void SBC_get_expected_happy_path()
{
  printf("SBC_get_expected_happy_path ");
  cpu->status=0; //clear carry flag
  cpu->A=126;
  SBC(1); 
  assert(cpu->A==125);
}

void ORA_negative_status_flagged()
{
  printf("test ORA_negative_status_flagged "); 
  cpu->A=-1;
  ORA_update_status_register();
  char expect_negative = cpu->status;
  cpu->A=1; 
  ORA_update_status_register();
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void ORA_zero_status_flagged()
{
  printf("test ORA_zero_status_flagged "); 
  cpu->A=0;
  ORA_update_status_register();
  char expect_zero = cpu->status;
  cpu->A=1;
  ORA_update_status_register();
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void ORA_get_expected_happy_path()
{
  printf("test ORA_get_expected_happy_path ");
  cpu->A=3;
  ORA(4);
  assert(cpu->A==7); 
}
 
void get_data_at_address_do_opcode_expected()
{
  printf("get_data_at_address_do_opcode_expected ");
  cpu->A=3;
  short address = 257;
  cpu->cpu_memory[address]=4;
  get_data_at_address_do_opcode(address, ORA);
  assert(cpu->A==7);
}

void AND_get_expected_happy_path()
{
  printf("test AND_get_expected_happy_path ");
  cpu->A=8;
  AND(4); 
  assert(cpu->A==0); 
}

void EOR_get_expected_happy_path()
{
  printf("test EOR_get_expected_happy_path ");
  cpu->A=7;
  EOR(5); 
  assert(cpu->A==2); 
}

void LDA_get_expected_happy_path()
{
  printf("test LDA_get_expected_happy_path "); 
  unsigned char data=7; 
  LDA(data); 
  assert(cpu->A==data); 
}

void STA_get_expected_happy_path()
{
  printf("test STA_get_expected_happy_path "); 
  unsigned char data=7;
  unsigned short address=23;
  cpu->A=data; 
  STA(address);
  assert(cpu->cpu_memory[address]==data);  
}

void CMP_carry_status_flagged()
{
  printf("test CMP_carry_status_flagged "); 
  cpu->A=1; 
  CMP_update_status_register(0);
  char expect_carry = cpu->status;
  CMP_update_status_register(2);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void CMP_zero_status_flagged()
{
  printf("test CMP_zero_status_flagged "); 
  cpu->A=1;
  CMP_update_status_register(1);
  char expect_zero = cpu->status;
  CMP_update_status_register(0);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void CMP_negative_status_flagged()
{
  printf("test CMP_negative_status_flagged "); 
  cpu->A=1;
  CMP_update_status_register(2);
  char expect_negative = cpu->status;
  CMP_update_status_register(0);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void CPX_carry_status_flagged()
{
  printf("test CPX_carry_status_flagged "); 
  cpu->X=1; 
  CPX_update_status_register(0);
  char expect_carry = cpu->status;
  CPX_update_status_register(2);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void CPX_zero_status_flagged()
{
  printf("test CPX_zero_status_flagged "); 
  cpu->X=1;
  CPX_update_status_register(1);
  char expect_zero = cpu->status;
  CPX_update_status_register(0);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void CPX_negative_status_flagged()
{
  printf("test CPX_negative_status_flagged "); 
  cpu->X=1;
  CPX_update_status_register(2);
  char expect_negative = cpu->status;
  CPX_update_status_register(0);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void CPY_carry_status_flagged()
{
  printf("test CPY_carry_status_flagged "); 
  cpu->A=1; 
  CPY_update_status_register(0);
  char expect_carry = cpu->status;
  CPY_update_status_register(2);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void CPY_zero_status_flagged()
{
  printf("test CPY_zero_status_flagged "); 
  cpu->A=1;
  CPY_update_status_register(1);
  char expect_zero = cpu->status;
  CPY_update_status_register(0);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void CPY_negative_status_flagged()
{
  printf("test CPY_negative_status_flagged "); 
  cpu->A=1;
  CPY_update_status_register(2);
  char expect_negative = cpu->status;
  CPY_update_status_register(0);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}
//------------------------

void shift_left_carry_status_flagged()
{
  unsigned char operand_1 = 129;
  unsigned char operand_2 = 1;
  printf("test shift_left_carry_status_flagged "); 

  shift_left(&operand_1, 0);
  char expect_carry = cpu->status;
  shift_left(&operand_2, 0);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void shift_left_zero_status_flagged()
{
  unsigned char operand_1 = 128;
  printf("test shift_left_zero_status_flagged "); 
  shift_left(&operand_1, 0); 
  char expect_zero = cpu->status;

  unsigned char operand_2 = 1; 
  shift_left(&operand_2, 0);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void shift_left_negative_status_flagged()
{
  unsigned char operand_1 = 64;
  unsigned char operand_2 = 129;
  printf("test shift_left_negative_status_flagged "); 

  shift_left(&operand_1, 0);
  char expect_negative = cpu->status;
  shift_left(&operand_2, 0);
  
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void shift_left_get_expected_happy_path()
{
  unsigned char operand_1 = 64;
  unsigned char operand_2 = 129;
  printf("test shift_left_get_expected_happy_path "); 

  shift_left(&operand_1, 0);
  shift_left(&operand_2, 0);
  
  assert(operand_1==128 && operand_2==2); 
}

void shift_left_rotate_expected()
{
  unsigned char operand_1=128;
  cpu->status=0;
  printf("test shift_left_rotate_expected ");
  shift_left(&operand_1, 1);
  unsigned char operand_2 = 0;
  shift_left(&operand_2, 1);
  assert(operand_1==0 && operand_2==1);
}

void shift_right_carry_status_flagged()
{
  unsigned char operand_1 = 1;
  unsigned char operand_2 = 2;
  printf("test shift_right_carry_status_flagged "); 

  shift_right(&operand_1, 0);
  char expect_carry = cpu->status;
  shift_right(&operand_2, 0);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void shift_right_zero_status_flagged()
{
  unsigned char operand_1 = 1;
  printf("test shift_right_zero_status_flagged "); 
  shift_right(&operand_1, 0); 
  char expect_zero = cpu->status;

  unsigned char operand_2 = 2; 
  shift_right(&operand_2, 0);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void shift_right_negative_status_flagged()
{
  unsigned char operand_1 = 128;
  printf("test shift_right_negative_status_flagged "); 

  shift_right(&operand_1, 0);
  
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 ); 
}

void shift_right_get_expected_happy_path()
{
  unsigned char operand_1 = 64;
  unsigned char operand_2 = 129;
  printf("test shift_right_get_expected_happy_path "); 

  shift_right(&operand_1, 0);
  shift_right(&operand_2, 0);
  
  assert(operand_1==32 && operand_2==64); 
}

void shift_right_rotate_expected()
{
  cpu->status=0;
  unsigned char operand_1 = 65;
  unsigned char operand_2 = 2;
  printf("test shift_right_get_expected_happy_path "); 

  shift_right(&operand_1, 1);
  shift_right(&operand_2, 1);
  
  assert(operand_1==32 && operand_2==129); 
}
void STX_get_expected()
{
  printf("STX_get_expected ");
  unsigned char x_value = 250;
  cpu->X=x_value;
  unsigned char address = 23;
  STX(&(cpu->cpu_memory[address]));
  assert(cpu->cpu_memory[address] == x_value);
}

void LDX_get_expected()
{
  printf("LDX_get_expetced ");
  cpu->X=0;
  unsigned char address = 47;
  unsigned char val = 93;
  cpu->cpu_memory[address]=val;
  LDX(&cpu->cpu_memory[address]);
  assert(cpu->X==val);
  
}

void LDX_zero_flag()
{
  printf("LDX_zero_flag ");
  unsigned char address = 23;
  cpu->cpu_memory[address]=0;
  LDX(&cpu->cpu_memory[address]);
  char expect_zero = cpu->status;
  cpu->cpu_memory[address]=1;
  LDX(&cpu->cpu_memory[address]);
  char not_expect_zero = cpu->status;
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 

}

void LDX_negative_flag()
{
  printf("LDX_negative_flag ");
  unsigned char address = 23;
  cpu->cpu_memory[address]=128;
  LDX(&cpu->cpu_memory[address]);
  char expect_negative = cpu->status;
  cpu->cpu_memory[address]=1;
  LDX(&cpu->cpu_memory[address]);
  char not_expect_negative = cpu->status;
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG);
}

void INC_get_expected()
{
  printf("INC_get_expected ");
  unsigned char a = 1;
  unsigned char b = 255;
  INC(&a);
  INC(&b);
  assert(a==2 && b==0);
}
void DEC_get_expected()
{
  printf("DEC_get_expected ");
  unsigned char a = 1;
  unsigned char b = 0;
  DEC(&a);
  DEC(&b);
  assert(a==0 && b==255);
}
// should print
// 00: ADC #a1
// 02: ADC $a3a2
void check_adc_print()
{
  cpu->PC=0;
  cpu->cpu_memory[0xfffc]=0; 
  cpu->cpu_memory[0xfffd]=0;
  cpu->cpu_memory[0]=0x69;
  cpu->cpu_memory[1]=0xA1;
  cpu->cpu_memory[2]=0x6D;
  cpu->cpu_memory[3]=0xA2;
  cpu->cpu_memory[4]=0xA3;
  run_rom();
}

void BIT_get_expected()
{
  printf("BIT_get_expected ");
  cpu->status=0;
  cpu->A = 254;
  unsigned char a = 65;
  BIT(&a);
  assert((cpu->status&NES_NEGATIVE_FLAG)==0 && (cpu->status&NES_OVERFLOW_FLAG)==NES_OVERFLOW_FLAG && (cpu->status&NES_ZERO_FLAG)==0);
}

void JMP_get_expected()
{
  printf("JMP_get_expected ");
  cpu->PC=0;
  unsigned char a[2];
  a[0] = 0xCD;
  a[1] = 0xAB;
  JMP(a);
  assert(cpu->PC==0xABCD);
}

void STY_get_expected()
{
  printf("STY_get_expected ");
  unsigned char y_value = 250;
  cpu->Y=y_value;
  unsigned short address = 0xCDEF;
  STY(&cpu->cpu_memory[address]);
  assert(cpu->cpu_memory[address] == y_value);
}

void LDY_get_expected()
{
  printf("LDY_get_expetced ");
  cpu->Y=0;
  unsigned char address = 47;
  unsigned char val = 93;
  cpu->cpu_memory[address]=val;
  LDY(&cpu->cpu_memory[address]);
  assert(cpu->Y==val);
  
}

void LDY_zero_flag()
{
  printf("LDY_zero_flag ");
  unsigned char address = 23;
  cpu->cpu_memory[address]=0;
  LDY(&cpu->cpu_memory[address]);
  char expect_zero = cpu->status;
  cpu->cpu_memory[address]=1;
  LDY(&cpu->cpu_memory[address]);
  char not_expect_zero = cpu->status;
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 

}

void LDY_negative_flag()
{
  printf("LDY_negative_flag ");
  unsigned char address = 23;
  cpu->cpu_memory[address]=128;
  LDY(&cpu->cpu_memory[address]);
  char expect_negative = cpu->status;
  cpu->cpu_memory[address]=1;
  LDY(&cpu->cpu_memory[address]);
  char not_expect_negative = cpu->status;
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG);
}

void test_flag_and_branch_get_expected()
{
  printf("test_flag_and_branch ");
  unsigned char offset = 7;
  cpu->status=NES_NEGATIVE_FLAG;
  cpu->PC=0;
  unsigned short pc0 = cpu->PC;
  test_flag_and_branch(NES_NEGATIVE_FLAG, 1, offset);
  unsigned short pc1 = cpu->PC;
  test_flag_and_branch(NES_NEGATIVE_FLAG, 0, offset);
  unsigned short pc2 = cpu->PC;
  test_flag_and_branch(NES_ZERO_FLAG, 1, offset);
  unsigned short pc3 = cpu->PC;
  test_flag_and_branch(NES_ZERO_FLAG, 0, offset);
  unsigned short pc4 = cpu->PC;
  assert((pc2-pc0==offset+2) && (pc1==pc2) && (pc2==pc3) && (pc4-pc3==offset+2));
}

void BRK_pc_stack_push()
{
  printf("BRK_pc_stack_push ");
  cpu->cpu_memory[0xfffe]=2;
  cpu->cpu_memory[0xffff]=3;
  printf("BRK_pc_stack_push ");
  cpu->stack_pointer=0;
  cpu->PC=258;
  cpu->status = 128;
  BRK();
  unsigned char expected_pc_high = cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+3];
  unsigned char expected_pc_low = cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+2];
  unsigned char status_with_break=  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+1];
  assert(expected_pc_high==1 && expected_pc_low==4 && status_with_break==144 && cpu->status==132 && cpu->PC==0x0203);
}

void RTI_get_expected()
{
  unsigned char pc_high = 4;
  unsigned char pc_low = 2;
  unsigned char status = 6;
  printf("RTI_get_expected ");
  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+3]=pc_high;
  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+2]=pc_low;
  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+1]=status;
  RTI();
  assert(cpu->PC==0x0402 && cpu->status==status);
}

void RTS_get_expected()
{
  printf("RTS_get_expected ");
  unsigned char pc_high = 5;
  unsigned char pc_low = 6;
  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+2] = pc_high;
  cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+1] = pc_low;
  RTS();
  assert(cpu->PC = 0x0507);
} 
void JSR_get_expected()
{
  printf("JSR_get_expected ");
  cpu->PC=0xfe00;
  cpu->cpu_memory[cpu->PC+1]=3;
  cpu->cpu_memory[cpu->PC+2]=2;
  JSR();
  unsigned char expected_pc_high = cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+2];
  unsigned char expected_pc_low = cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+1];
  assert(expected_pc_high==0xfe && expected_pc_low==0x00 && cpu->PC==0x0203);
}
void PHP_get_expected()
{
  unsigned char status_value = 191;
  printf("PHP_get_expected ");
  cpu->status=status_value;
  PHP();
  assert((cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+1]) == status_value);
}

void PLP_get_expected()
{
  cpu->status=0;
  unsigned char status_value = 193;
  printf("PLP_get_expected ");
  stack_push_char(status_value);
  PLP();
  assert(cpu->status == status_value);
}
void PHA_get_expected()
{
  unsigned char a_value = 192;
  printf("PHA_get_expected ");
  cpu->A=a_value;
  PHA();
  assert((cpu->cpu_memory[STACK_TOP-(cpu->stack_pointer)+1]) == a_value);
}

void PLA_get_expected()
{
  cpu->status=0; 
  unsigned char a_value = 193;
  printf("PLA_get_expected ");
  stack_push_char(a_value);
  PLA();
  assert(cpu->A == (signed char)a_value  && cpu->status==NES_NEGATIVE_FLAG);
}

void DEY_get_expected()
{
  printf("DEY_get_expected ");
  cpu->status=0;
  unsigned char y_value = 1;
  cpu->Y=y_value;
  DEY();
  assert(cpu->Y==0 && cpu->status==NES_ZERO_FLAG);
}
void TAY_get_expected()
{
  printf("TAY_get_expected ");
  unsigned char expected_value = 128;
  cpu->A=expected_value;
  cpu->status=0;
  cpu->Y=0;
  TAY();
  assert(cpu->Y==expected_value && cpu->status==NES_NEGATIVE_FLAG);
}
void INY_get_expected()
{
  printf("INY_get_expected ");
  cpu->Y=0;
  cpu->status=0;
  INY();
  unsigned char y_1 = cpu->Y;
  unsigned char s_1 = cpu->status;
  cpu->Y=255;
  INY();
  unsigned char y_2 = cpu->Y;
  unsigned char s_2 = cpu->status; 
  assert(y_1==1 && s_1==0 && y_2==0 && s_2==NES_ZERO_FLAG);
}
void INX_get_expected()
{
  printf("INX_get_expected ");
  cpu->X=0;
  cpu->status=0;
  INX();
  unsigned char x_1 = cpu->X;
  unsigned char s_1 = cpu->status;
  cpu->X=255;
  INX();
  unsigned char x_2 = cpu->X;
  unsigned char s_2 = cpu->status; 
  assert(x_1==1 && s_1==0 && x_2==0 && s_2==NES_ZERO_FLAG);
}
void DEX_get_expected()
{
  printf("DEX_get_expected ");
  cpu->X=0;
  cpu->status=0;
  DEX();
  unsigned char x_1 = cpu->X;
  unsigned char s_1 = cpu->status;
  cpu->X=1;
  DEX();
  unsigned char x_2 = cpu->X;
  unsigned char s_2 = cpu->status; 
  assert(x_1==255 && s_1==NES_NEGATIVE_FLAG && x_2==0 && s_2==NES_ZERO_FLAG);
}
void CLC_get_expected()
{
  printf("CLC get expected ");
  cpu->status=NES_CARRY_FLAG;
  CLC();
  assert(cpu->status==0);
}
void SEC_get_expected()
{
  printf("SEC get expected ");
  cpu->status=0;
  SEC();
  assert(cpu->status==NES_CARRY_FLAG);
}
void CLI_get_expected()
{
  printf("CLI get expected ");
  cpu->status=NES_INTERRUPT_DISABLE_FLAG;
  CLI();
  assert(cpu->status==0);
}
void CLV_get_expected()
{
  printf("CLV get expected ");
  cpu->status=NES_OVERFLOW_FLAG;
  CLV();
  assert(cpu->status==0);
}
void CLD_get_expected()
{
  printf("CLD get expected ");
  cpu->status=NES_DECIMAL_MODE_FLAG;
  CLD();
  assert(cpu->status==0);
}
void SED_get_expected()
{
  printf("SED get expected ");
  cpu->status=0;
  SED();
  assert(cpu->status==NES_DECIMAL_MODE_FLAG);
}
void SEI_get_expected()
{
  printf("SEI get expected ");
  cpu->status=0;
  SEI();
  assert(cpu->status==NES_INTERRUPT_DISABLE_FLAG);
}
void TYA_get_expected()
{
  printf("TYA_get_expected ");
  unsigned char expected_value = 128;
  cpu->Y=expected_value;
  cpu->status=0;
  cpu->A=0;
  TYA();
  assert((unsigned char)cpu->A==expected_value && cpu->status==NES_NEGATIVE_FLAG);
}

void TXA_get_expected()
{
  printf("TXA_get_expected ");
  unsigned char expected_value = 128;
  cpu->X=expected_value;
  cpu->status=0;
  cpu->A=0;
  TXA();
  assert((unsigned char)cpu->A==expected_value && cpu->status==NES_NEGATIVE_FLAG);
}
void TXS_get_expected()
{
  printf("TXS_get_expected ");
  unsigned char expected_value = 128;
  cpu->X=expected_value;
  cpu->status=0;
  cpu->stack_pointer=0;
  TXS();
  assert((unsigned char)cpu->stack_pointer==expected_value && cpu->status==NES_NEGATIVE_FLAG);
}
void TAX_get_expected()
{
  printf("TAX_get_expected ");
  unsigned char expected_value = 128;
  cpu->A=expected_value;
  cpu->status=0;
  cpu->X=0;
  TAX();
  assert(cpu->X==expected_value && cpu->status==NES_NEGATIVE_FLAG);
}
void TSX_get_expected()
{
  printf("TSX_get_expected ");
  unsigned char expected_value = 128;
  cpu->stack_pointer=expected_value;
  cpu->status=0;
  cpu->X=0;
  TSX();
  assert(cpu->X==expected_value && cpu->status==NES_NEGATIVE_FLAG);
}

void STA_2006_write_get_expected()
{
  printf("STA_2006_write_get_expected ");
  unsigned short address = 0x2006;  
  cpu->A = 0x12;
  STA(address);
  cpu->A = 0x34;
  STA(address);
  assert(get_ppu_write_address()==0x1234);
}
int main(int argc, char* argv[])
{
  cpu = malloc(sizeof(struct NES_CPU));
  load_test_rom();
  test_get_absolute_address();
  test_get_absolute_address_X();
  test_get_absolute_address_Y();
  test_get_indexed_indirect_X();
  test_get_indirect_indexed_Y();
  ADC_carry_status_flagged_result_less();
  ADC_carry_status_flagged_result_greater();
  ADC_carry_status_flagged_result_equal();
  SBC_carry_status_flagged_result_less();
  SBC_carry_status_flagged_result_greater();
  SBC_carry_status_flagged_result_equal();

  ADC_zero_status_flagged();
  ADC_overflow_status_flagged();
  ADC_negative_status_flagged();
  ADC_get_expected_overflow();
  ADC_get_expected_happy_path();

  SBC_carry_status_flagged();
  SBC_zero_status_flagged();
  SBC_overflow_status_flagged();
  SBC_negative_status_flagged();
  SBC_get_expected_overflow();
  SBC_get_expected_happy_path();


  ORA_zero_status_flagged();
  ORA_negative_status_flagged();
  ORA_get_expected_happy_path();
  AND_get_expected_happy_path();
  EOR_get_expected_happy_path();
  LDA_get_expected_happy_path();
  STA_get_expected_happy_path();
  CMP_carry_status_flagged();
  CMP_zero_status_flagged();
  CMP_negative_status_flagged();
  shift_left_carry_status_flagged();
  shift_left_negative_status_flagged();
  shift_left_zero_status_flagged();
  shift_left_get_expected_happy_path();
  shift_left_zero_status_flagged();
  shift_right_carry_status_flagged();
  shift_right_negative_status_flagged();
  shift_right_zero_status_flagged();
  shift_right_get_expected_happy_path();
  shift_right_rotate_expected();
  STX_get_expected();
  LDX_get_expected();
  LDX_zero_flag();
  LDX_negative_flag();
  get_data_at_address_do_opcode_expected();
  INC_get_expected();
  DEC_get_expected();
  BIT_get_expected();
  JMP_get_expected();
  STY_get_expected();
  LDY_get_expected();
  LDY_zero_flag();
  LDY_negative_flag();
  CPX_carry_status_flagged();
  CPX_zero_status_flagged();
  CPX_negative_status_flagged();
  CPY_carry_status_flagged();
  CPY_zero_status_flagged();
  CPY_negative_status_flagged();
  BRK_pc_stack_push();
  JSR_get_expected();
  test_flag_and_branch_get_expected();
  RTI_get_expected();
  RTS_get_expected();
  PHP_get_expected();
  PLP_get_expected();
  PHA_get_expected();
  PLA_get_expected();
  DEY_get_expected();
  TAY_get_expected();
  INY_get_expected();
  INX_get_expected();
  CLC_get_expected();
  SEC_get_expected();
  CLI_get_expected();
  SEI_get_expected();
  TYA_get_expected();
  CLV_get_expected();
  CLD_get_expected();
  SED_get_expected();
  TXA_get_expected();
  TXS_get_expected();
  TAX_get_expected();
  TSX_get_expected();
  DEX_get_expected();
  STA_2006_write_get_expected();
}
