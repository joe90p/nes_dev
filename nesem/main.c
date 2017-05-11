#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
  FILE* nes_file_ptr;
  int nes_file_length;
  char* buffer;
  nes_file_ptr = fopen("/home/phil/test.nes", "r");
  fseek(nes_file_ptr,0,SEEK_END);
  nes_file_length = ftell(nes_file_ptr);  
  buffer = malloc(nes_file_length);
  fseek(nes_file_ptr,0,SEEK_SET);
  fread(buffer, 1, nes_file_length, nes_file_ptr); 
  
  fclose(nes_file_ptr); 
  for(int i=0; i<3; i++)
  {
    printf("%c\n", buffer[i]);
  }
 
  return 0;
}
