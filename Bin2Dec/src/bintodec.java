
public class bintodec {
	
	
	private static boolean carryOver = false;
	private static BinaryNumber[] binaryTable;
	public static void main(String[] args)
	{
		int input = Integer.parseInt(args[0]);
		BinaryNumber tempBinary = BinaryNumber.ConvertFromInt(input);
		
		binaryTable = new BinaryNumber[10];
		binaryTable[0] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, false, false});
		binaryTable[1] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, false, true});
		binaryTable[2] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, true, false});
		binaryTable[3] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, true, true});
		binaryTable[4] = new BinaryNumber(new boolean[]{ false, false, false, false, false, true, false, false});
		binaryTable[5] = new BinaryNumber(new boolean[]{ true, false, false, false, false, false, false, false});
		binaryTable[6] = new BinaryNumber(new boolean[]{ true, false, false, false, false, false, false, true});
		binaryTable[7] = new BinaryNumber(new boolean[]{ true, false, false, false, false, false, true, false});
		binaryTable[8] = new BinaryNumber(new boolean[]{ true, false, false, false, false, false, true, true});
		binaryTable[9] = new BinaryNumber(new boolean[]{ true, false, false, false, false, true, false, false});
		
		//BinaryNumber tempBinary = new BinaryNumber(new boolean[]{ false, false, false, true, false, false, false, false});
		
		BinaryNumber[] decimalResult = new BinaryNumber[3]; 
		decimalResult[0] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, false, false});
		decimalResult[1] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, false, false});
		decimalResult[2] = new BinaryNumber(new boolean[]{ false, false, false, false, false, false, false, false});
		
		for(int i=0; i < 8; i++)
		{
			int y = 0;
			carryOver = tempBinary.ShiftLeft(carryOver);
			
			
			y = decimalResult[0].ConvertToInt();
			decimalResult[0] = binaryTable[y].GetCopy();
			carryOver = decimalResult[0].ShiftLeft(carryOver);
			
			y = decimalResult[1].ConvertToInt();
			decimalResult[1] = binaryTable[y].GetCopy();
			carryOver = decimalResult[1].ShiftLeft(carryOver);
			
			carryOver = decimalResult[2].ShiftLeft(carryOver);
		}
		for(int i=2; i >= 0; i--)
		{
			System.out.print(decimalResult[i].ConvertToInt());
		}
		
		
		
	}
}



/*
BinaryToDecimal:
   lda #$00 
   sta decimalResult+0
   sta decimalResult+1
   sta decimalResult+2
   sta decimalResult+3
   sta decimalResult+4
   ldx #$10 
BitLoop: 
   asl tempBinary+0 
   rol tempBinary+1
   ldy decimalResult+0
   lda BinTable, y 
   rol a
   sta decimalResult+0
   ldy decimalResult+1
   lda BinTable, y 
   rol a
   sta decimalResult+1
   ldy decimalResult+2
   lda BinTable, y 
   rol a
   sta decimalResult+2
   ldy decimalResult+3
   lda BinTable, y 
   rol a
   sta decimalResult+3
   rol decimalResult+4
   dex 
   bne BitLoop 
   rts 
BinTable:
   .db $00, $01, $02, $03, $04, $80, $81, $82, $83, $84
*/
