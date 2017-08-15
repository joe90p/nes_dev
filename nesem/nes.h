#include <stdio.h>
#include <stdlib.h>

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

void run_rom();

unsigned short get_absolute_address(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_absolute_address_X(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_zeropage_address(unsigned char get_address_input);
unsigned short get_zeropage_X_address(unsigned char get_address_input);
unsigned short get_absolute_address_Y(unsigned char get_address_input_upper_byte, unsigned char get_address_input_lower_byte);
unsigned short get_indexed_indirect_X(unsigned char get_address_input);
unsigned short get_indirect_indexed_Y(unsigned char get_address_input);
void ADC_update_status_register(unsigned char oldA);
void ADC(unsigned char, unsigned char);