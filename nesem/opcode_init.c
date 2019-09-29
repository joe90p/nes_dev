#include <stdlib.h>
#include </home/phil/git/nes_dev/nesem/opcode_init.h>


void set_opcodes(struct opcode** opcodes_ptr, struct address** addresses_ptr)
{
  (*opcodes_ptr) = malloc(256 * sizeof(struct opcode));
  (*addresses_ptr) = malloc(15 * sizeof(struct address));
  unsigned char IMMEDIATE_ADDRESS_MODE = 0;
  unsigned char ZEROPAGE_ADDRESS_MODE = 1;
  unsigned char ACCUMULATOR_ADDRESS_MODE = 2;
  unsigned char ABSOLUTE_ADDRESS_MODE = 3;
  unsigned char ZEROPAGE_X_ADDRESS_MODE = 4;
  unsigned char ABSOLUTE_X_ADDRESS_MODE = 5;
  unsigned char INDEXED_INDIRECT_X_ADDRESS_MODE = 6;
  unsigned char INDIRECT_INDEXED_Y_ADDRESS_MODE = 7; 
  unsigned char ABSOLUTE_Y_ADDRESS_MODE = 8;
  unsigned char ZEROPAGE_Y_ADDRESS_MODE = 9;
  unsigned char NONE_ADDRESS_MODE = 10;
  unsigned char ABSOLUTE_INDIRECT_ADDRESS_MODE = 11;
  unsigned char BRANCH_DUMMY = 12;
  unsigned char JUMP_DUMMY = 13;
  unsigned char JSR_DUMMY = 14;
  
  (*addresses_ptr)[0].program_counter_increment = 2;
  (*addresses_ptr)[0].get_operand_ptr = get_immediate_operand_ptr;
  (*addresses_ptr)[0].address_info = "#%02x";
  (*addresses_ptr)[0].inc_pc = 1;

  (*addresses_ptr)[1].program_counter_increment = 2;
  (*addresses_ptr)[1].get_operand_ptr = get_zeropage_operand_ptr;
  (*addresses_ptr)[1].address_info = "$%02x";
  (*addresses_ptr)[1].inc_pc = 1;
 
  (*addresses_ptr)[2].program_counter_increment = 1;
  (*addresses_ptr)[2].get_operand_ptr = get_accumulator_operand_ptr;
  (*addresses_ptr)[2].address_info = "A";
  (*addresses_ptr)[2].inc_pc = 1;

  (*addresses_ptr)[3].program_counter_increment = 3;
  (*addresses_ptr)[3].get_operand_ptr = get_absolute_operand_ptr;
  (*addresses_ptr)[3].address_info = "$%02x%02x";
  (*addresses_ptr)[3].inc_pc = 1;
 
  (*addresses_ptr)[4].program_counter_increment = 2;
  (*addresses_ptr)[4].get_operand_ptr = get_zeropage_X_operand_ptr;
  (*addresses_ptr)[4].address_info = "$%02x,X";
  (*addresses_ptr)[4].inc_pc = 1;

  (*addresses_ptr)[5].program_counter_increment = 3;
  (*addresses_ptr)[5].get_operand_ptr = get_absolute_X_operand_ptr;
  (*addresses_ptr)[5].address_info = "$%02x%02x,X";
  (*addresses_ptr)[5].inc_pc = 1;

  (*addresses_ptr)[6].program_counter_increment = 2;
  (*addresses_ptr)[6].get_operand_ptr = get_indexed_indirect_X_operand_ptr;
  (*addresses_ptr)[6].address_info = "($%02x,X)";
  (*addresses_ptr)[6].inc_pc = 1;

  (*addresses_ptr)[7].program_counter_increment = 2;
  (*addresses_ptr)[7].get_operand_ptr = get_indirect_indexed_Y_operand_ptr;
  (*addresses_ptr)[7].address_info = "($%02x),Y";
  (*addresses_ptr)[7].inc_pc = 1;

  (*addresses_ptr)[8].program_counter_increment = 3;
  (*addresses_ptr)[8].get_operand_ptr = get_absolute_Y_operand_ptr;
  (*addresses_ptr)[8].address_info = "$%02x%02x,Y";
  (*addresses_ptr)[8].inc_pc = 1;

  (*addresses_ptr)[9].program_counter_increment = 2;
  (*addresses_ptr)[9].get_operand_ptr = get_zeropage_Y_operand_ptr;
  (*addresses_ptr)[9].address_info = "$%02x,Y";
  (*addresses_ptr)[9].inc_pc = 1;

  (*addresses_ptr)[10].program_counter_increment = 1;
  (*addresses_ptr)[10].get_operand_ptr = get_dummy_operand_ptr;
  (*addresses_ptr)[10].address_info = "";
  (*addresses_ptr)[10].inc_pc = 1;

  (*addresses_ptr)[11].program_counter_increment = 3;
  (*addresses_ptr)[11].get_operand_ptr = get_absolute_indirect_operand_ptr;
  (*addresses_ptr)[11].address_info = "($%02x%02x)";
  (*addresses_ptr)[11].inc_pc = 0;

  (*addresses_ptr)[12].program_counter_increment = 2;
  (*addresses_ptr)[12].get_operand_ptr = get_zeropage_operand_ptr;
  (*addresses_ptr)[12].address_info = "$%02x";
  (*addresses_ptr)[12].inc_pc = 0;
 
  (*addresses_ptr)[13].program_counter_increment = 3;
  (*addresses_ptr)[13].get_operand_ptr = get_absolute_operand_ptr;
  (*addresses_ptr)[13].address_info = "$%02x%02x";
  (*addresses_ptr)[13].inc_pc = 0;

  (*addresses_ptr)[14].program_counter_increment = 1;
  (*addresses_ptr)[14].get_operand_ptr = get_absolute_operand_ptr;
  (*addresses_ptr)[14].address_info = "";
  (*addresses_ptr)[14].inc_pc = 0;

  for(int i=0; i<256; i++)
  {
    (*opcodes_ptr)[i].name = 0;
    (*opcodes_ptr)[i].action = 0;
  }

  (*opcodes_ptr)[0x00].name = "BRK";
  (*opcodes_ptr)[0x00].action = BRK;
  (*opcodes_ptr)[0x00].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x01].name = "ORA";
  (*opcodes_ptr)[0x01].action = ORA_ptr;
  (*opcodes_ptr)[0x01].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x02].name = "STP";
  (*opcodes_ptr)[0x02].action = STP;
  (*opcodes_ptr)[0x02].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x03].name = "SLO";
  (*opcodes_ptr)[0x03].action = SLO;
  (*opcodes_ptr)[0x03].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x04].name = "NOP";
  (*opcodes_ptr)[0x04].action = NOP;
  (*opcodes_ptr)[0x04].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x05].name = "ORA";
  (*opcodes_ptr)[0x05].action = ORA_ptr;
  (*opcodes_ptr)[0x05].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x06].name = "ASL";
  (*opcodes_ptr)[0x06].action = ASL;
  (*opcodes_ptr)[0x06].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x07].name = "SLO";
  (*opcodes_ptr)[0x07].action = SLO;
  (*opcodes_ptr)[0x07].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x08].name = "PHP";
  (*opcodes_ptr)[0x08].action = PHP;
  (*opcodes_ptr)[0x08].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x09].name = "ORA";
  (*opcodes_ptr)[0x09].action = ORA_ptr;
  (*opcodes_ptr)[0x09].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x0a].name = "ASL";
  (*opcodes_ptr)[0x0a].action = ASL;
  (*opcodes_ptr)[0x0a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  (*opcodes_ptr)[0x0b].name = "ANC";
  (*opcodes_ptr)[0x0b].action = ANC;
  (*opcodes_ptr)[0x0b].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x0c].name = "NOP";
  (*opcodes_ptr)[0x0c].action = NOP;
  (*opcodes_ptr)[0x0c].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x0d].name = "ORA";
  (*opcodes_ptr)[0x0d].action = ORA_ptr;
  (*opcodes_ptr)[0x0d].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x0e].name = "ASL";
  (*opcodes_ptr)[0x0e].action = ASL;
  (*opcodes_ptr)[0x0e].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x0f].name = "SLO";
  (*opcodes_ptr)[0x0f].action = SLO;
  (*opcodes_ptr)[0x0f].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x10].name = "BPL";
  (*opcodes_ptr)[0x10].action = BPL;
  (*opcodes_ptr)[0x10].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0x11].name = "ORA";
  (*opcodes_ptr)[0x11].action = ORA_ptr;
  (*opcodes_ptr)[0x11].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x12].name = "STP";
  (*opcodes_ptr)[0x12].action = STP;
  (*opcodes_ptr)[0x12].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x13].name = "SLO";
  (*opcodes_ptr)[0x13].action = SLO;
  (*opcodes_ptr)[0x13].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x14].name = "NOP";
  (*opcodes_ptr)[0x14].action = NOP;
  (*opcodes_ptr)[0x14].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x15].name = "ORA";
  (*opcodes_ptr)[0x15].action = ORA_ptr;
  (*opcodes_ptr)[0x15].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x16].name = "ASL";
  (*opcodes_ptr)[0x16].action = ASL;
  (*opcodes_ptr)[0x16].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x17].name = "SLO";
  (*opcodes_ptr)[0x17].action = SLO;
  (*opcodes_ptr)[0x17].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x18].name = "CLC";
  (*opcodes_ptr)[0x18].action = CLC;
  (*opcodes_ptr)[0x18].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x19].name = "ORA";
  (*opcodes_ptr)[0x19].action = ORA_ptr;
  (*opcodes_ptr)[0x19].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x1a].name = "NOP";
  (*opcodes_ptr)[0x1a].action = NOP;
  (*opcodes_ptr)[0x1a].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x1b].name = "SLO";
  (*opcodes_ptr)[0x1b].action = SLO;
  (*opcodes_ptr)[0x1b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x1c].name = "NOP";
  (*opcodes_ptr)[0x1c].action = NOP;
  (*opcodes_ptr)[0x1c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x1d].name = "ORA";
  (*opcodes_ptr)[0x1d].action = ORA_ptr;
  (*opcodes_ptr)[0x1d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x1e].name = "ASL";
  (*opcodes_ptr)[0x1e].action = ASL;
  (*opcodes_ptr)[0x1e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x1f].name = "SLO";
  (*opcodes_ptr)[0x1f].action = SLO;
  (*opcodes_ptr)[0x1f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x1f].name = "SLO";
  (*opcodes_ptr)[0x1f].action = SLO;
  (*opcodes_ptr)[0x1f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x1f].name = "SLO";
  (*opcodes_ptr)[0x1f].action = SLO;
  (*opcodes_ptr)[0x1f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x20].name = "JSR";
  (*opcodes_ptr)[0x20].action = JSR;
  (*opcodes_ptr)[0x20].address_mode = JSR_DUMMY;

  (*opcodes_ptr)[0x21].name = "AND";
  (*opcodes_ptr)[0x21].action = AND_ptr;
  (*opcodes_ptr)[0x21].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x22].name = "STP";
  (*opcodes_ptr)[0x22].action = STP;
  (*opcodes_ptr)[0x22].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x23].name = "RLA";
  (*opcodes_ptr)[0x23].action = RLA;
  (*opcodes_ptr)[0x23].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x24].name = "BIT";
  (*opcodes_ptr)[0x24].action = BIT;
  (*opcodes_ptr)[0x24].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x25].name = "AND";
  (*opcodes_ptr)[0x25].action = AND_ptr;
  (*opcodes_ptr)[0x25].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x26].name = "ROL";
  (*opcodes_ptr)[0x26].action = ROL;
  (*opcodes_ptr)[0x26].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x27].name = "RLA";
  (*opcodes_ptr)[0x27].action = RLA;
  (*opcodes_ptr)[0x27].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x28].name = "PLP";
  (*opcodes_ptr)[0x28].action = PLP;
  (*opcodes_ptr)[0x28].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x29].name = "AND";
  (*opcodes_ptr)[0x29].action = AND_ptr;
  (*opcodes_ptr)[0x29].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x2a].name = "ROL";
  (*opcodes_ptr)[0x2a].action = ROL;
  (*opcodes_ptr)[0x2a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  (*opcodes_ptr)[0x2b].name = "ANC";
  (*opcodes_ptr)[0x2b].action = ANC;
  (*opcodes_ptr)[0x2b].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x2c].name = "BIT";
  (*opcodes_ptr)[0x2c].action = BIT;
  (*opcodes_ptr)[0x2c].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x2d].name = "AND";
  (*opcodes_ptr)[0x2d].action = AND_ptr;
  (*opcodes_ptr)[0x2d].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x2e].name = "ROL";
  (*opcodes_ptr)[0x2e].action = ROL;
  (*opcodes_ptr)[0x2e].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x2f].name = "RLA";
  (*opcodes_ptr)[0x2f].action = RLA;
  (*opcodes_ptr)[0x2f].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x30].name = "BMI";
  (*opcodes_ptr)[0x30].action = BMI;
  (*opcodes_ptr)[0x30].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0x31].name = "AND";
  (*opcodes_ptr)[0x31].action = AND_ptr;
  (*opcodes_ptr)[0x31].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x32].name = "STP";
  (*opcodes_ptr)[0x32].action = STP;
  (*opcodes_ptr)[0x32].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x33].name = "RLA";
  (*opcodes_ptr)[0x33].action = RLA;
  (*opcodes_ptr)[0x33].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x34].name = "NOP";
  (*opcodes_ptr)[0x34].action = NOP;
  (*opcodes_ptr)[0x34].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x35].name = "AND";
  (*opcodes_ptr)[0x35].action = AND_ptr;
  (*opcodes_ptr)[0x35].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x36].name = "ROL";
  (*opcodes_ptr)[0x36].action = ROL;
  (*opcodes_ptr)[0x36].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x37].name = "RLA";
  (*opcodes_ptr)[0x37].action = RLA;
  (*opcodes_ptr)[0x37].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x38].name = "SEC";
  (*opcodes_ptr)[0x38].action = SEC;
  (*opcodes_ptr)[0x38].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x39].name = "AND";
  (*opcodes_ptr)[0x39].action = AND_ptr;
  (*opcodes_ptr)[0x39].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x3a].name = "NOP";
  (*opcodes_ptr)[0x3a].action = NOP;
  (*opcodes_ptr)[0x3a].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x3b].name = "RLA";
  (*opcodes_ptr)[0x3b].action = RLA;
  (*opcodes_ptr)[0x3b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x3c].name = "NOP";
  (*opcodes_ptr)[0x3c].action = NOP;
  (*opcodes_ptr)[0x3c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x3d].name = "AND";
  (*opcodes_ptr)[0x3d].action = AND_ptr;
  (*opcodes_ptr)[0x3d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x3e].name = "ROL";
  (*opcodes_ptr)[0x3e].action = ROL;
  (*opcodes_ptr)[0x3e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x3f].name = "RLA";
  (*opcodes_ptr)[0x3f].action = RLA;
  (*opcodes_ptr)[0x3f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x40].name = "RTI";
  (*opcodes_ptr)[0x40].action = RTI;
  (*opcodes_ptr)[0x40].address_mode = JSR_DUMMY;

  (*opcodes_ptr)[0x41].name = "EOR";
  (*opcodes_ptr)[0x41].action = EOR_ptr;
  (*opcodes_ptr)[0x41].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x42].name = "STP";
  (*opcodes_ptr)[0x42].action = STP;
  (*opcodes_ptr)[0x42].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x43].name = "SRE";
  (*opcodes_ptr)[0x43].action = SRE;
  (*opcodes_ptr)[0x43].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x44].name = "NOP";
  (*opcodes_ptr)[0x44].action = NOP;
  (*opcodes_ptr)[0x44].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x45].name = "EOR";
  (*opcodes_ptr)[0x45].action = EOR_ptr;
  (*opcodes_ptr)[0x45].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x46].name = "LSR";
  (*opcodes_ptr)[0x46].action = LSR;
  (*opcodes_ptr)[0x46].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x47].name = "SRE";
  (*opcodes_ptr)[0x47].action = SRE;
  (*opcodes_ptr)[0x47].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x48].name = "PHA";
  (*opcodes_ptr)[0x48].action = PHA;
  (*opcodes_ptr)[0x48].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x49].name = "EOR";
  (*opcodes_ptr)[0x49].action = EOR_ptr;
  (*opcodes_ptr)[0x49].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x4a].name = "LSR";
  (*opcodes_ptr)[0x4a].action = LSR;
  (*opcodes_ptr)[0x4a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  (*opcodes_ptr)[0x4b].name = "ALR";
  (*opcodes_ptr)[0x4b].action = ALR;
  (*opcodes_ptr)[0x4b].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x4c].name = "JMP";
  (*opcodes_ptr)[0x4c].action = JMP;
  (*opcodes_ptr)[0x4c].address_mode = JUMP_DUMMY;

  (*opcodes_ptr)[0x4d].name = "EOR";
  (*opcodes_ptr)[0x4d].action = EOR_ptr;
  (*opcodes_ptr)[0x4d].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x4e].name = "LSR";
  (*opcodes_ptr)[0x4e].action = LSR;
  (*opcodes_ptr)[0x4e].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x4f].name = "SRE";
  (*opcodes_ptr)[0x4f].action = SRE;
  (*opcodes_ptr)[0x4f].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x50].name = "BVC";
  (*opcodes_ptr)[0x50].action = BVC;
  (*opcodes_ptr)[0x50].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0x51].name = "EOR";
  (*opcodes_ptr)[0x51].action = EOR_ptr;
  (*opcodes_ptr)[0x51].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x52].name = "STP";
  (*opcodes_ptr)[0x52].action = STP;
  (*opcodes_ptr)[0x52].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x53].name = "SRE";
  (*opcodes_ptr)[0x53].action = SRE;
  (*opcodes_ptr)[0x53].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;


  (*opcodes_ptr)[0x54].name = "NOP";
  (*opcodes_ptr)[0x54].action = NOP;
  (*opcodes_ptr)[0x54].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x55].name = "EOR";
  (*opcodes_ptr)[0x55].action = EOR_ptr;
  (*opcodes_ptr)[0x55].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x56].name = "LSR";
  (*opcodes_ptr)[0x56].action = LSR;
  (*opcodes_ptr)[0x56].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x57].name = "SRE";
  (*opcodes_ptr)[0x57].action = SRE;
  (*opcodes_ptr)[0x57].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x58].name = "CLI";
  (*opcodes_ptr)[0x58].action = CLI;
  (*opcodes_ptr)[0x58].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x59].name = "EOR";
  (*opcodes_ptr)[0x59].action = EOR_ptr;
  (*opcodes_ptr)[0x59].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x5a].name = "NOP";
  (*opcodes_ptr)[0x5a].action = NOP;
  (*opcodes_ptr)[0x5a].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x5b].name = "SRE";
  (*opcodes_ptr)[0x5b].action = SRE;
  (*opcodes_ptr)[0x5b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x5c].name = "NOP";
  (*opcodes_ptr)[0x5c].action = NOP;
  (*opcodes_ptr)[0x5c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x5d].name = "EOR";
  (*opcodes_ptr)[0x5d].action = EOR_ptr;
  (*opcodes_ptr)[0x5d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x5e].name = "LSR";
  (*opcodes_ptr)[0x5e].action = LSR;
  (*opcodes_ptr)[0x5e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x5f].name = "SRE";
  (*opcodes_ptr)[0x5f].action = SRE;
  (*opcodes_ptr)[0x5f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x60].name = "RTS";
  (*opcodes_ptr)[0x60].action = RTS;
  (*opcodes_ptr)[0x60].address_mode = JSR_DUMMY;

  (*opcodes_ptr)[0x61].name = "ADC";
  (*opcodes_ptr)[0x61].action = ADC_ptr;
  (*opcodes_ptr)[0x61].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x62].name = "STP";
  (*opcodes_ptr)[0x62].action = STP;
  (*opcodes_ptr)[0x62].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x63].name = "RRA";
  (*opcodes_ptr)[0x63].action = RRA;
  (*opcodes_ptr)[0x63].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x64].name = "NOP";
  (*opcodes_ptr)[0x64].action = NOP;
  (*opcodes_ptr)[0x64].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x65].name = "ADC";
  (*opcodes_ptr)[0x65].action = ADC_ptr;
  (*opcodes_ptr)[0x65].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x66].name = "ROR";
  (*opcodes_ptr)[0x66].action = ROR;
  (*opcodes_ptr)[0x66].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x67].name = "RRA";
  (*opcodes_ptr)[0x67].action = RRA;
  (*opcodes_ptr)[0x67].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x68].name = "PLA";
  (*opcodes_ptr)[0x68].action = PLA;
  (*opcodes_ptr)[0x68].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x69].name = "ADC";
  (*opcodes_ptr)[0x69].action = ADC_ptr;
  (*opcodes_ptr)[0x69].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x6a].name = "ROR";
  (*opcodes_ptr)[0x6a].action = ROR;
  (*opcodes_ptr)[0x6a].address_mode = ACCUMULATOR_ADDRESS_MODE;

  (*opcodes_ptr)[0x6b].name = "ARR";
  (*opcodes_ptr)[0x6b].action = ARR;
  (*opcodes_ptr)[0x6b].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x6c].name = "JMP";
  (*opcodes_ptr)[0x6c].action = JMP;
  (*opcodes_ptr)[0x6c].address_mode = ABSOLUTE_INDIRECT_ADDRESS_MODE;

  (*opcodes_ptr)[0x6d].name = "ADC";
  (*opcodes_ptr)[0x6d].action = ADC_ptr;
  (*opcodes_ptr)[0x6d].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x6e].name = "ROR";
  (*opcodes_ptr)[0x6e].action = ROR;
  (*opcodes_ptr)[0x6e].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x6f].name = "RRA";
  (*opcodes_ptr)[0x6f].action = RRA;
  (*opcodes_ptr)[0x6f].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x70].name = "BVS";
  (*opcodes_ptr)[0x70].action = BVS;
  (*opcodes_ptr)[0x70].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0x71].name = "ADC";
  (*opcodes_ptr)[0x71].action = ADC_ptr;
  (*opcodes_ptr)[0x71].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x72].name = "STP";
  (*opcodes_ptr)[0x72].action = STP;
  (*opcodes_ptr)[0x72].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x73].name = "RRA";
  (*opcodes_ptr)[0x73].action = RRA;
  (*opcodes_ptr)[0x73].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x74].name = "NOP";
  (*opcodes_ptr)[0x74].action = NOP;
  (*opcodes_ptr)[0x74].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x75].name = "ADC";
  (*opcodes_ptr)[0x75].action = ADC_ptr;
  (*opcodes_ptr)[0x75].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x76].name = "ROR";
  (*opcodes_ptr)[0x76].action = ROR;
  (*opcodes_ptr)[0x76].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x77].name = "RRA";
  (*opcodes_ptr)[0x77].action = RRA;
  (*opcodes_ptr)[0x77].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x78].name = "SEI";
  (*opcodes_ptr)[0x78].action = SEI;
  (*opcodes_ptr)[0x78].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x79].name = "ADC";
  (*opcodes_ptr)[0x79].action = ADC_ptr;
  (*opcodes_ptr)[0x79].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x7a].name = "NOP";
  (*opcodes_ptr)[0x7a].action = NOP;
  (*opcodes_ptr)[0x7a].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x7b].name = "RRA";
  (*opcodes_ptr)[0x7b].action = RRA;
  (*opcodes_ptr)[0x7b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x7c].name = "NOP";
  (*opcodes_ptr)[0x7c].action = NOP;
  (*opcodes_ptr)[0x7c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x7d].name = "ADC";
  (*opcodes_ptr)[0x7d].action = ADC_ptr;
  (*opcodes_ptr)[0x7d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x7e].name = "ROR";
  (*opcodes_ptr)[0x7e].action = ROR;
  (*opcodes_ptr)[0x7e].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x7f].name = "RRA";
  (*opcodes_ptr)[0x7f].action = RRA;
  (*opcodes_ptr)[0x7f].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x80].name = "NOP";
  (*opcodes_ptr)[0x80].action = NOP;
  (*opcodes_ptr)[0x80].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x81].name = "STA";
  (*opcodes_ptr)[0x81].action = STA_ptr;
  (*opcodes_ptr)[0x81].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x82].name = "NOP";
  (*opcodes_ptr)[0x82].action = NOP;
  (*opcodes_ptr)[0x82].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x83].name = "SAX";
  (*opcodes_ptr)[0x83].action = SAX;
  (*opcodes_ptr)[0x83].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x84].name = "STY";
  (*opcodes_ptr)[0x84].action = STY;
  (*opcodes_ptr)[0x84].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x85].name = "STA";
  (*opcodes_ptr)[0x85].action = STA_ptr;
  (*opcodes_ptr)[0x85].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x86].name = "STX";
  (*opcodes_ptr)[0x86].action = STX;
  (*opcodes_ptr)[0x86].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x87].name = "SAX";
  (*opcodes_ptr)[0x87].action = SAX;
  (*opcodes_ptr)[0x87].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0x88].name = "DEY";
  (*opcodes_ptr)[0x88].action = DEY;
  (*opcodes_ptr)[0x88].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x89].name = "NOP";
  (*opcodes_ptr)[0x89].action = NOP;
  (*opcodes_ptr)[0x89].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x8a].name = "TXA";
  (*opcodes_ptr)[0x8a].action = TXA;
  (*opcodes_ptr)[0x8a].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x8b].name = "XAA";
  (*opcodes_ptr)[0x8b].action = XAA;
  (*opcodes_ptr)[0x8b].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0x8c].name = "STY";
  (*opcodes_ptr)[0x8c].action = STY;
  (*opcodes_ptr)[0x8c].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x8d].name = "STA";
  (*opcodes_ptr)[0x8d].action = STA_ptr;
  (*opcodes_ptr)[0x8d].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x8e].name = "STX";
  (*opcodes_ptr)[0x8e].action = STX;
  (*opcodes_ptr)[0x8e].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x8f].name = "SAX";
  (*opcodes_ptr)[0x8f].action = SAX;
  (*opcodes_ptr)[0x8f].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0x90].name = "BCC";
  (*opcodes_ptr)[0x90].action = BCC;
  (*opcodes_ptr)[0x90].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0x91].name = "STA";
  (*opcodes_ptr)[0x91].action = STA_ptr;
  (*opcodes_ptr)[0x91].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x92].name = "STP";
  (*opcodes_ptr)[0x92].action = STP;
  (*opcodes_ptr)[0x92].address_mode = NONE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0x93].name = "AHX";
  (*opcodes_ptr)[0x93].action = AHX;
  (*opcodes_ptr)[0x93].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;
  
  (*opcodes_ptr)[0x94].name = "STY";
  (*opcodes_ptr)[0x94].action = STY;
  (*opcodes_ptr)[0x94].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x95].name = "STA";
  (*opcodes_ptr)[0x95].action = STA_ptr;
  (*opcodes_ptr)[0x95].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x96].name = "STX";
  (*opcodes_ptr)[0x96].action = STX;
  (*opcodes_ptr)[0x96].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x97].name = "SAX";
  (*opcodes_ptr)[0x97].action = SAX;
  (*opcodes_ptr)[0x97].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x98].name = "TYA";
  (*opcodes_ptr)[0x98].action = TYA;
  (*opcodes_ptr)[0x98].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x99].name = "STA";
  (*opcodes_ptr)[0x99].action = STA_ptr;
  (*opcodes_ptr)[0x99].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x9a].name = "TXS";
  (*opcodes_ptr)[0x9a].action = TXS;
  (*opcodes_ptr)[0x9a].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0x9b].name = "TAS";
  (*opcodes_ptr)[0x9b].action = TAS;
  (*opcodes_ptr)[0x9b].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x9c].name = "SHY";
  (*opcodes_ptr)[0x9c].action = SHY;
  (*opcodes_ptr)[0x9c].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x9d].name = "STA";
  (*opcodes_ptr)[0x9d].action = STA_ptr;
  (*opcodes_ptr)[0x9d].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0x9e].name = "SHX";
  (*opcodes_ptr)[0x9e].action = SHX;
  (*opcodes_ptr)[0x9e].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0x9f].name = "AHX";
  (*opcodes_ptr)[0x9f].action = AHX;
  (*opcodes_ptr)[0x9f].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xa0].name = "LDY";
  (*opcodes_ptr)[0xa0].action = LDY;
  (*opcodes_ptr)[0xa0].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa1].name = "LDA";
  (*opcodes_ptr)[0xa1].action = LDA_ptr;
  (*opcodes_ptr)[0xa1].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xa2].name = "LDX";
  (*opcodes_ptr)[0xa2].action = LDX;
  (*opcodes_ptr)[0xa2].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa3].name = "LAX";
  (*opcodes_ptr)[0xa3].action = LAX;
  (*opcodes_ptr)[0xa3].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xa4].name = "LDY";
  (*opcodes_ptr)[0xa4].action = LDY;
  (*opcodes_ptr)[0xa4].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa5].name = "LDA";
  (*opcodes_ptr)[0xa5].action = LDA_ptr;
  (*opcodes_ptr)[0xa5].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa6].name = "LDX";
  (*opcodes_ptr)[0xa6].action = LDX;
  (*opcodes_ptr)[0xa6].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa7].name = "LAX";
  (*opcodes_ptr)[0xa7].action = LAX;
  (*opcodes_ptr)[0xa7].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa8].name = "TAY";
  (*opcodes_ptr)[0xa8].action = TAY;
  (*opcodes_ptr)[0xa8].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xa9].name = "LDA";
  (*opcodes_ptr)[0xa9].action = LDA_ptr;
  (*opcodes_ptr)[0xa9].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xaa].name = "TAX";
  (*opcodes_ptr)[0xaa].action = TAX;
  (*opcodes_ptr)[0xaa].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xab].name = "LAX";
  (*opcodes_ptr)[0xab].action = LAX;
  (*opcodes_ptr)[0xab].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xac].name = "LDY";
  (*opcodes_ptr)[0xac].action = LDY;
  (*opcodes_ptr)[0xac].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xad].name = "LDA";
  (*opcodes_ptr)[0xad].action = LDA_ptr;
  (*opcodes_ptr)[0xad].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xae].name = "LDX";
  (*opcodes_ptr)[0xae].action = LDX;
  (*opcodes_ptr)[0xae].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xaf].name = "LAX";
  (*opcodes_ptr)[0xaf].action = LAX;
  (*opcodes_ptr)[0xaf].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xb0].name = "BCS";
  (*opcodes_ptr)[0xb0].action = BCS;
  (*opcodes_ptr)[0xb0].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0xb1].name = "LDA";
  (*opcodes_ptr)[0xb1].action = LDA_ptr;
  (*opcodes_ptr)[0xb1].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xb2].name = "STP";
  (*opcodes_ptr)[0xb2].action = STP;
  (*opcodes_ptr)[0xb2].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xb3].name = "LAX";
  (*opcodes_ptr)[0xb3].action = LAX;
  (*opcodes_ptr)[0xb3].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xb4].name = "LDY";
  (*opcodes_ptr)[0xb4].action = LDY;
  (*opcodes_ptr)[0xb4].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xb5].name = "LDA";
  (*opcodes_ptr)[0xb5].action = LDA_ptr;
  (*opcodes_ptr)[0xb5].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xb6].name = "LDX";
  (*opcodes_ptr)[0xb6].action = LDX;
  (*opcodes_ptr)[0xb6].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xb7].name = "LAX";
  (*opcodes_ptr)[0xb7].action = LAX;
  (*opcodes_ptr)[0xb7].address_mode = ZEROPAGE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xb8].name = "CLV";
  (*opcodes_ptr)[0xb8].action = CLV;
  (*opcodes_ptr)[0xb8].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xb9].name = "LDA";
  (*opcodes_ptr)[0xb9].action = LDA_ptr;
  (*opcodes_ptr)[0xb9].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xba].name = "TSX";
  (*opcodes_ptr)[0xba].action = TSX;
  (*opcodes_ptr)[0xba].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xbb].name = "LAS";
  (*opcodes_ptr)[0xbb].action = LAS;
  (*opcodes_ptr)[0xbb].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xbc].name = "LDY";
  (*opcodes_ptr)[0xbc].action = LDY;
  (*opcodes_ptr)[0xbc].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xbd].name = "LDA";
  (*opcodes_ptr)[0xbd].action = LDA_ptr;
  (*opcodes_ptr)[0xbd].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xbe].name = "LDX";
  (*opcodes_ptr)[0xbe].action = LDX;
  (*opcodes_ptr)[0xbe].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xbf].name = "LAX";
  (*opcodes_ptr)[0xbf].action = LAX;
  (*opcodes_ptr)[0xbf].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xc0].name = "CPY";
  (*opcodes_ptr)[0xc0].action = CPY;
  (*opcodes_ptr)[0xc0].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc1].name = "CMP";
  (*opcodes_ptr)[0xc1].action = CMP;
  (*opcodes_ptr)[0xc1].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xc2].name = "NOP";
  (*opcodes_ptr)[0xc2].action = NOP;
  (*opcodes_ptr)[0xc2].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc3].name = "DCP";
  (*opcodes_ptr)[0xc3].action = DCP;
  (*opcodes_ptr)[0xc3].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xc4].name = "CPY";
  (*opcodes_ptr)[0xc4].action = CPY;
  (*opcodes_ptr)[0xc4].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc5].name = "CMP";
  (*opcodes_ptr)[0xc5].action = CMP;
  (*opcodes_ptr)[0xc5].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc6].name = "DEC";
  (*opcodes_ptr)[0xc6].action = DEC;
  (*opcodes_ptr)[0xc6].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc7].name = "DCP";
  (*opcodes_ptr)[0xc7].action = DCP;
  (*opcodes_ptr)[0xc7].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc8].name = "INY";
  (*opcodes_ptr)[0xc8].action = INY;
  (*opcodes_ptr)[0xc8].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xc9].name = "CMP";
  (*opcodes_ptr)[0xc9].action = CMP;
  (*opcodes_ptr)[0xc9].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xca].name = "DEX";
  (*opcodes_ptr)[0xca].action = DEX;
  (*opcodes_ptr)[0xca].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xcb].name = "AXS";
  (*opcodes_ptr)[0xcb].action = AXS;
  (*opcodes_ptr)[0xcb].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xcc].name = "CPY";
  (*opcodes_ptr)[0xcc].action = CPY;
  (*opcodes_ptr)[0xcc].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xcd].name = "CMP";
  (*opcodes_ptr)[0xcd].action = CMP;
  (*opcodes_ptr)[0xcd].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xce].name = "DEC";
  (*opcodes_ptr)[0xce].action = DEC;
  (*opcodes_ptr)[0xce].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xcf].name = "DCP";
  (*opcodes_ptr)[0xcf].action = DCP;
  (*opcodes_ptr)[0xcf].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xd0].name = "BNE";
  (*opcodes_ptr)[0xd0].action = BNE;
  (*opcodes_ptr)[0xd0].address_mode = BRANCH_DUMMY;

  (*opcodes_ptr)[0xd1].name = "CMP";
  (*opcodes_ptr)[0xd1].action = CMP;
  (*opcodes_ptr)[0xd1].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xd2].name = "STP";
  (*opcodes_ptr)[0xd2].action = STP;
  (*opcodes_ptr)[0xd2].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xd3].name = "DCP";
  (*opcodes_ptr)[0xd3].action = DCP;
  (*opcodes_ptr)[0xd3].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xd4].name = "NOP";
  (*opcodes_ptr)[0xd4].action = NOP;
  (*opcodes_ptr)[0xd4].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xd5].name = "CMP";
  (*opcodes_ptr)[0xd5].action = CMP;
  (*opcodes_ptr)[0xd5].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xd6].name = "DEC";
  (*opcodes_ptr)[0xd6].action = DEC;
  (*opcodes_ptr)[0xd6].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xd7].name = "DCP";
  (*opcodes_ptr)[0xd7].action = DCP;
  (*opcodes_ptr)[0xd7].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xd8].name = "CLD";
  (*opcodes_ptr)[0xd8].action = CLD;
  (*opcodes_ptr)[0xd8].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xd9].name = "CMP";
  (*opcodes_ptr)[0xd9].action = CMP;
  (*opcodes_ptr)[0xd9].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xda].name = "NOP";
  (*opcodes_ptr)[0xda].action = NOP;
  (*opcodes_ptr)[0xda].address_mode = NONE_ADDRESS_MODE;

  (*opcodes_ptr)[0xdb].name = "DCP";
  (*opcodes_ptr)[0xdb].action = DCP;
  (*opcodes_ptr)[0xdb].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xdc].name = "NOP";
  (*opcodes_ptr)[0xdc].action = NOP;
  (*opcodes_ptr)[0xdc].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xdd].name = "CMP";
  (*opcodes_ptr)[0xdd].action = CMP;
  (*opcodes_ptr)[0xdd].address_mode = ABSOLUTE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xde].name = "DEC";
  (*opcodes_ptr)[0xde].action = DEC;
  (*opcodes_ptr)[0xde].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xdf].name = "DCP";
  (*opcodes_ptr)[0xdf].action = DCP;
  (*opcodes_ptr)[0xdf].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe0].name = "CPX";
  (*opcodes_ptr)[0xe0].action = CPX;
  (*opcodes_ptr)[0xe0].address_mode = IMMEDIATE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe1].name = "SBC";
  (*opcodes_ptr)[0xe1].action = SBC_ptr;
  (*opcodes_ptr)[0xe1].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xe2].name = "NOP";
  (*opcodes_ptr)[0xe2].action = NOP;
  (*opcodes_ptr)[0xe2].address_mode = IMMEDIATE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe3].name = "ISC";
  (*opcodes_ptr)[0xe3].action = ISC;
  (*opcodes_ptr)[0xe3].address_mode = INDEXED_INDIRECT_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xe4].name = "CPX";
  (*opcodes_ptr)[0xe4].action = CPX;
  (*opcodes_ptr)[0xe4].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe5].name = "SBC";
  (*opcodes_ptr)[0xe5].action = SBC_ptr;
  (*opcodes_ptr)[0xe5].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe6].name = "INC";
  (*opcodes_ptr)[0xe6].action = INC;
  (*opcodes_ptr)[0xe6].address_mode = ZEROPAGE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe7].name = "ISC";
  (*opcodes_ptr)[0xe7].action = ISC;
  (*opcodes_ptr)[0xe7].address_mode = ZEROPAGE_ADDRESS_MODE;

  (*opcodes_ptr)[0xe8].name = "INX";
  (*opcodes_ptr)[0xe8].action = INX;
  (*opcodes_ptr)[0xe8].address_mode = NONE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xe9].name = "SBC";
  (*opcodes_ptr)[0xe9].action = SBC_ptr;
  (*opcodes_ptr)[0xe9].address_mode = IMMEDIATE_ADDRESS_MODE;

  (*opcodes_ptr)[0xea].name = "NOP";
  (*opcodes_ptr)[0xea].action = NOP;
  (*opcodes_ptr)[0xea].address_mode = NONE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xeb].name = "SBC";
  (*opcodes_ptr)[0xeb].action = SBC_ptr;
  (*opcodes_ptr)[0xeb].address_mode = IMMEDIATE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xec].name = "CPX";
  (*opcodes_ptr)[0xec].action = CPX;
  (*opcodes_ptr)[0xec].address_mode = ABSOLUTE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xed].name = "SBC";
  (*opcodes_ptr)[0xed].action = SBC_ptr;
  (*opcodes_ptr)[0xed].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xee].name = "INC";
  (*opcodes_ptr)[0xee].action = INC;
  (*opcodes_ptr)[0xee].address_mode = ABSOLUTE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xef].name = "ISC";
  (*opcodes_ptr)[0xef].action = ISC;
  (*opcodes_ptr)[0xef].address_mode = ABSOLUTE_ADDRESS_MODE;

  (*opcodes_ptr)[0xf0].name = "BEQ";
  (*opcodes_ptr)[0xf0].action = BEQ;
  (*opcodes_ptr)[0xf0].address_mode = BRANCH_DUMMY;
  
  (*opcodes_ptr)[0xf1].name = "SBC";
  (*opcodes_ptr)[0xf1].action = SBC_ptr;
  (*opcodes_ptr)[0xf1].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xf2].name = "STP";
  (*opcodes_ptr)[0xf2].action = STP;
  (*opcodes_ptr)[0xf2].address_mode = NONE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xf3].name = "ISC";
  (*opcodes_ptr)[0xf3].action = ISC;
  (*opcodes_ptr)[0xf3].address_mode = INDIRECT_INDEXED_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xf4].name = "NOP";
  (*opcodes_ptr)[0xf4].action = NOP;
  (*opcodes_ptr)[0xf4].address_mode = ZEROPAGE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xf5].name = "SBC";
  (*opcodes_ptr)[0xf5].action = SBC_ptr;
  (*opcodes_ptr)[0xf5].address_mode = ZEROPAGE_X_ADDRESS_MODE;

  (*opcodes_ptr)[0xf6].name = "INC";
  (*opcodes_ptr)[0xf6].action = INC;
  (*opcodes_ptr)[0xf6].address_mode = ZEROPAGE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xf7].name = "ISC";
  (*opcodes_ptr)[0xf7].action = ISC;
  (*opcodes_ptr)[0xf7].address_mode = ZEROPAGE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xf8].name = "SED";
  (*opcodes_ptr)[0xf8].action = SED;
  (*opcodes_ptr)[0xf8].address_mode = NONE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xf9].name = "SBC";
  (*opcodes_ptr)[0xf9].action = SBC_ptr;
  (*opcodes_ptr)[0xf9].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xfa].name = "NOP";
  (*opcodes_ptr)[0xfa].action = NOP;
  (*opcodes_ptr)[0xfa].address_mode = NONE_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xfb].name = "ISC";
  (*opcodes_ptr)[0xfb].action = ISC;
  (*opcodes_ptr)[0xfb].address_mode = ABSOLUTE_Y_ADDRESS_MODE;

  (*opcodes_ptr)[0xfc].name = "NOP";
  (*opcodes_ptr)[0xfc].action = NOP;
  (*opcodes_ptr)[0xfc].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xfd].name = "SBC";
  (*opcodes_ptr)[0xfd].action = SBC_ptr;
  (*opcodes_ptr)[0xfd].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xfe].name = "INC";
  (*opcodes_ptr)[0xfe].action = INC;
  (*opcodes_ptr)[0xfe].address_mode = ABSOLUTE_X_ADDRESS_MODE;
  
  (*opcodes_ptr)[0xff].name = "ISC";
  (*opcodes_ptr)[0xff].action = ISC;
  (*opcodes_ptr)[0xff].address_mode = ABSOLUTE_X_ADDRESS_MODE;

}

