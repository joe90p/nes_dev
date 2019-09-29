#define NES_CARRY_FLAG 1
#define NES_ZERO_FLAG 2
#define NES_INTERRUPT_DISABLE_FLAG 4
#define NES_DECIMAL_MODE_FLAG 8
#define NES_BREAK_FLAG 16
#define NES_OVERFLOW_FLAG 64
#define NES_NEGATIVE_FLAG 128
#define STACK_BOTTOM 256
#define PPU_CONTROL_CPU_ADDRESS 0x2000
#define PPU_MASK_CPU_ADDRESS 0x2001
#define PPU_STATUS_CPU_ADDRESS 0x2002
#define PPU_OAM_ADDRESS_CPU_ADDRESS 0x2003
#define PPU_OAM_DATA_CPU_ADDRESS 0x2004
#define PPU_SCROLL_CPU_ADDRESS 0x2005
#define PPU_ADDRESS_CPU_ADDRESS 0x2006
#define PPU_DATA_CPU_ADDRESS 0x2007
#define PPU_OAM_DMA_CPU_ADDRESS 0x4014

unsigned short get_ppu_write_address();
void set_ppu_write_address(unsigned short address);
void ADC_update_status_register(signed char oldA, unsigned char overflow);
void SBC_update_status_register(signed char oldA);
void ORA_update_status_register();
void CMP_update_status_register(unsigned char data);
void ADC(unsigned char);
void ORA(unsigned char);
void AND(unsigned char);
void EOR(unsigned char);
void LDA(unsigned char);
void STA(unsigned short);
void SBC(unsigned char);
void shift_left(unsigned char*, char);
void shift_right(unsigned char*, char);
void STX(unsigned char*);
void LDX(unsigned char*);
void INC(unsigned char*);
void DEC(unsigned char*);
void BIT(unsigned char*);
void JMP(unsigned char*);
void STY(unsigned char*);
void LDY(unsigned char*);
void STP(unsigned char*);
void SLO(unsigned char*);
void ASL(unsigned char*);
void ANC(unsigned char*);
void BPL(unsigned char*);
void RLA(unsigned char*);
void ROL(unsigned char*);
void BMI(unsigned char*);
void SRE(unsigned char*);
void LSR(unsigned char*);
void ALR(unsigned char*);
void BVC(unsigned char*);
void RRA(unsigned char*);
void ROR(unsigned char*);
void ARR(unsigned char*);
void BVS(unsigned char*);
void SAX(unsigned char*);
void XAA(unsigned char*);
void BCC(unsigned char*);
void AHX(unsigned char*);
void TAS(unsigned char*);
void SHY(unsigned char*);
void SHX(unsigned char*);
void LAX(unsigned char*);
void BCS(unsigned char*);
void LAS(unsigned char*);
void CPY(unsigned char*);
void CMP(unsigned char*);
void DCP(unsigned char*);
void AXS(unsigned char*);
void BNE(unsigned char*);
void CPX(unsigned char*);
void ISC(unsigned char*);
void ORA_ptr(unsigned char*); 
void AND_ptr(unsigned char*);
void EOR_ptr(unsigned char*);
void ADC_ptr(unsigned char*);
void STA_ptr(unsigned char*);
void LDA_ptr(unsigned char*);
void SBC_ptr(unsigned char*);
void BEQ(unsigned char*);
void get_data_at_address_do_opcode(short address, opcode_action_type);
void set_negative_zero_flag(unsigned char);
void CPX_update_status_register(unsigned char data);
void CPY_update_status_register(unsigned char data);
//unsigned char test_flag_and_branch(unsigned char flag, unsigned char equalTo, unsigned char offset);
void BRK();
void JSR();
void RTI();
void RTS();
void PHP();
void PHA();
void PLA();
void PLP();
void DEY();
void TAY();
void INX();
void INY();
void CLC();
void SEC();
void CLI();
void SEI();
void TAY();
void TYA();
void CLV();
void CLD();
void SED();
void TXS();
void TXA();
void TAX();
void TSX();
void DEX();
void NOP();
void stack_push_char(unsigned char);
unsigned short get_short_from_cpu_memory(unsigned short);
void switch_status_flag(char, char);
void JMP_ind();
void NMI();
