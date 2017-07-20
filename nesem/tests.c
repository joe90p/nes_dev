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

void test_get_absolute_address()
{
  printf("test test_get_absolute_address ");
  unsigned short address = get_absolute_address(0x21, 0x45);
  if(address==0x2145)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }
}

void test_get_absolute_address_X()
{
  printf("test test_get_absolute_address_X ");
  unsigned short address = get_absolute_address_X(0x21, 0x45);
  if(address==0x2148)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }
}

void test_get_absolute_address_Y()
{
  printf("test test_get_absolute_address_Y ");
  unsigned short address = get_absolute_address_Y(0x21, 0x45);
  if(address==0x214a)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }
}



/*
void ADC_update_status_register(unsigned char oldA)
{
  cpu->status|=cpu->A&128;
  cpu->status|=((cpu->A&128)^(oldA&128))>>1;
  cpu->status|=(cpu->A==0)<<1;
  cpu->status|=((cpu->A&128)^(oldA&128))>>7;

}
*/

void ADC_overflow_status_flagged()
{
  printf("test test_ADC_overflow_status_flagged ");
  cpu->status=0;
  cpu->A=3;
  char overflow_flag = 1;
  ADC_update_status_register(250);
  char expect_overflow = cpu->status;
  ADC_update_status_register(2);
  char not_expect_overflow = cpu->status; 
  if((not_expect_overflow&overflow_flag)==0 && (expect_overflow&overflow_flag)==overflow_flag)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }

}
void test_get_indexed_indirect_X()
{
  printf("test test_get_indexed_indirect_X ");
  cpu->cpu_memory[0x4A]=0x83;
  unsigned short address = get_indexed_indirect_X(0x47);
  if(address==0x0083)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }    
}

void test_get_indirect_indexed_Y()
{
  printf("test test_getindirect_indexed_Y ");
  cpu->cpu_memory[0x23]=0x87;
  unsigned short address = get_indirect_indexed_Y(0x23);
  if(address==0x008C)
  {
    printf("PASSED\n");
  }
  else
  {
    printf("FAILED\n");
  }
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
  ADC_overflow_status_flagged();

}


