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
void test_get_zeropage_address(unsigned char get_address_input);
void test_get_zeropage_X_address(unsigned char get_address_input);
*/
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
}


