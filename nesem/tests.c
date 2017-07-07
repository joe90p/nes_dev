#include <stdio.h>
#include <stdlib.h>

void load_test_rom()
{
  unsigned char* cpu_memory = malloc(0x10000);
  //short rom_fileoffset=17;
  //char flags6 = ines_file_contents[6];
  /*if(flags6 & 8)
  {
    rom_fileoffset+=512;
  }i*/
  for(int i=0; i< 0x4000; i++)
  { 
    cpu_memory[0x8000 + i] = 0;//ines_file_contents[rom_fileoffset + i];
    cpu_memory[0xC000 + i] = 0;//ines_file_contents[rom_fileoffset + i];
  }
  cpu->cpu_memory=cpu_memory;
  cpu->X = 3;
  cpu->Y = 5;
}

void test_get_absolute_address()
{
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

/*void test_get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
void test_get_zeropage_address(unsigned char get_address_input);
void test_get_zeropage_X_address(unsigned char get_address_input);
void test_get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
void test_get_indexed_indirect_X(unsigned char get_address_input);
void test_get_indirect_indexed_Y(unsigned char get_address_input);*/

int main(int argc, char* argv[])
{
  cpu = malloc(sizeof(struct NES_CPU));
  load_test_rom();
  test_get_absolute_address();
  test_get_absolute_address_X();
  test_get_absolute_address_Y();

}


