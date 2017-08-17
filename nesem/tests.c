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

void ADC_carry_status_flagged()
{
  printf("test ADC_carry_status_flagged "); 
  cpu->A=3;
  char carry_flag = 1;
  ADC_update_status_register(250);
  char expect_carry = cpu->status;
  ADC_update_status_register(2);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&carry_flag)==0 && (expect_carry&carry_flag)==carry_flag); 
}

void ADC_zero_status_flagged()
{
  printf("test ADC_zero_status_flagged "); 
  cpu->A=0;
  char zero_flag = 2;
  ADC_update_status_register(5);
  char expect_zero = cpu->status;
  cpu->A=1;
  ADC_update_status_register(5);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&zero_flag)==0 && (expect_zero&zero_flag)==zero_flag); 
}

void ADC_overflow_status_flagged()
{
  printf("test ADC_carry_status_flagged "); 
  cpu->A=3;
  char overflow_flag = 64;
  ADC_update_status_register(250);
  char expect_overflow = cpu->status;
  ADC_update_status_register(2);
  char not_expect_overflow = cpu->status; 
  assert((not_expect_overflow&overflow_flag)==0 && (expect_overflow&overflow_flag)==overflow_flag); 
}

void ADC_negative_status_flagged()
{
  printf("test ADC_negative_status_flagged "); 
  cpu->A=-1;
  char negative_flag = 128;
  ADC_update_status_register(2);
  char expect_negative = cpu->status;
  cpu->A=1; 
  ADC_update_status_register(2);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&negative_flag)==0 && (expect_negative&negative_flag)==negative_flag); 
}

void ADC_get_expected_overflow()
{
  printf("ADC_get_expected_overflow ");
  cpu->A=126;
  ADC(3); 
  assert(cpu->A==-127);
}

void ADC_get_expected_happy_path()
{
  printf("ADC_get_expected_happy_path ");
  cpu->A=126;
  ADC(1); 
  assert(cpu->A==127);
}

void ORA_negative_status_flagged()
{
  printf("test ORA_negative_status_flagged "); 
  cpu->A=-1;
  char negative_flag = 128;
  ORA_update_status_register();
  char expect_negative = cpu->status;
  cpu->A=1; 
  ORA_update_status_register();
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&negative_flag)==0 && (expect_negative&negative_flag)==negative_flag); 
}

void ORA_zero_status_flagged()
{
  printf("test ORA_zero_status_flagged "); 
  cpu->A=0;
  char zero_flag = 2;
  ORA_update_status_register();
  char expect_zero = cpu->status;
  cpu->A=1;
  ORA_update_status_register();
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&zero_flag)==0 && (expect_zero&zero_flag)==zero_flag); 
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
  char carry_flag = 1;
  CMP_update_status_register(0);
  char expect_carry = cpu->status;
  CMP_update_status_register(2);
  char not_expect_carry = cpu->status; 
  assert((not_expect_carry&carry_flag)==0 && (expect_carry&carry_flag)==carry_flag); 
}

void CMP_zero_status_flagged()
{
  printf("test CMP_zero_status_flagged "); 
  cpu->A=1;
  char zero_flag = 2;
  CMP_update_status_register(1);
  char expect_zero = cpu->status;
  CMP_update_status_register(0);
  char not_expect_zero = cpu->status; 
  assert((not_expect_zero&zero_flag)==0 && (expect_zero&zero_flag)==zero_flag); 
}

void CMP_negative_status_flagged()
{
  printf("test CMP_negative_status_flagged "); 
  cpu->A=1;
  char negative_flag = 128;
  CMP_update_status_register(2);
  char expect_negative = cpu->status;
  CMP_update_status_register(0);
  char not_expect_negative = cpu->status; 
  assert((not_expect_negative&negative_flag)==0 && (expect_negative&negative_flag)==negative_flag); 
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
  ADC_carry_status_flagged();
  ADC_zero_status_flagged();
  ADC_overflow_status_flagged();
  ADC_negative_status_flagged();
  ADC_get_expected_overflow();
  ADC_get_expected_happy_path();
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
  check_adc_print();
}
