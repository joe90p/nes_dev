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

void ASL_carry_status_flagged()
{
  unsigned char operand_1 = 129;
  unsigned char operand_2 = 1;
  printf("test ASL_carry_status_flagged "); 

  ASL(&operand_1);
  char expect_carry = cpu->status;
  ASL(&operand_2);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&NES_CARRY_FLAG)==0 && (expect_carry&NES_CARRY_FLAG)==NES_CARRY_FLAG); 
}

void ASL_zero_status_flagged()
{
  unsigned char operand_1 = 129;
  printf("test ASL_zero_status_flagged "); 
  cpu->A=0;
  ASL(&operand_1); 
  char expect_zero = cpu->status;
 
  cpu->A=1; 
  ASL(&operand_1);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&NES_ZERO_FLAG)==0 && (expect_zero&NES_ZERO_FLAG)==NES_ZERO_FLAG); 
}

void ASL_negative_status_flagged()
{
  unsigned char operand_1 = 64;
  unsigned char operand_2 = 129;
  printf("test ASL_negative_status_flagged "); 

  ASL(&operand_1);
  char expect_negative = cpu->status;
  ASL(&operand_2);
  
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&NES_NEGATIVE_FLAG)==0 && (expect_negative&NES_NEGATIVE_FLAG)==NES_NEGATIVE_FLAG); 
}

void ASL_get_expected_happy_path()
{
  unsigned char operand_1 = 64;
  unsigned char operand_2 = 129;
  printf("test ASL_get_expected_happy_path "); 

  ASL(&operand_1);
  ASL(&operand_2);
  
  assert(operand_1==128 && operand_2==2); 
}

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
  ASL_carry_status_flagged();
  ASL_negative_status_flagged();
  ASL_zero_status_flagged();
  ASL_get_expected_happy_path();
}
