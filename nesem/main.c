#include <stdio.h>
#include <stdlib.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void handler(int sig) {
  void *array[10];
  size_t size;

     size = backtrace(array, 10);
  
       // print out all the frames to stderr
         fprintf(stderr, "Error: signal %d:\n", sig);
           backtrace_symbols_fd(array, size, STDERR_FILENO);
             exit(1);
             }
int main(int argc, char* argv[])
{
//  signal(SIGSEGV, handler);
  cpu = malloc(sizeof(struct NES_CPU)); 
  ppu = malloc(sizeof(struct NES_PPU)); 
  io = malloc(sizeof(struct NES_IO));
  FILE* nes_file_ptr;
  char is_test = 0;
  if(argc>=3)
  {
    
    char* test_mode_arg = argv[2];
    is_test = !strcmp(test_mode_arg, "test");
  }
  int nes_file_length; 
  nes_file_ptr = fopen(argv[1], "r");
  fseek(nes_file_ptr,0,SEEK_END);
  nes_file_length = ftell(nes_file_ptr);  
  ines_file_contents = malloc(nes_file_length);
  fseek(nes_file_ptr,0,SEEK_SET);
  fread(ines_file_contents, 1, nes_file_length, nes_file_ptr); 
  
  fclose(nes_file_ptr); 
  load_rom();
  run_rom(is_test);
  return 0;
}

