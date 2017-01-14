/*
 * Binary representation of 8 bit number
 */
public class BinaryNumber
{
	/*
	 * boolean array (length 8) representing bits. 
	 * bool at index 0=2^7, 1=2^6,..., 7=2^0
	 */
	private boolean[] _number;
	
	public BinaryNumber()
	{
		_number = new boolean[8];
		
	}
	
	public BinaryNumber(boolean[] num)
	{
		_number = num;
		
	}
	
	public BinaryNumber GetCopy()
	{
		boolean[] numberCopy = new boolean[8];
		
		for(int i=0; i<8; i++)
		{
			numberCopy[i] = _number[i];
		}
		return new BinaryNumber(numberCopy);
	}
	
	
	public boolean ShiftLeft(boolean carry)
	{
		boolean[] numberCopy = new boolean[8];
		
		for(int i=0; i<8; i++)
		{
			numberCopy[i] = _number[i];
		}
		boolean carryOver = _number[0];
		for(int i=7; i>=0; i--)
		{
			_number[i] = i==7 ? carry : numberCopy[i+1];
		}
		return carryOver;
	}
	
	public int ConvertToInt()
	{
		int result = 0;
		int twoPower = 1;
		for(int i=7; i>=0; i--)
		{
			twoPower = i==7 ?  1 : 2 * twoPower;
			if(_number[i])
			{
				result+=twoPower;
			}
			
		}
		return result;
	}
	
	public static BinaryNumber ConvertFromInt(int toConvert)
	{
		//handle 0 case
		if(toConvert==0)
		{
			return new BinaryNumber();
		}
		else
		{
			boolean[] number = new boolean[8];
			int i=0;
			int twoPower = 1;
			while(twoPower * 2 <= toConvert)
			{
				twoPower = i==0 ?  1 : 2 * twoPower;
				i++;
			}
			i=7-i + 1;
			do
			{
				if(toConvert>=twoPower)
				{
					number[i] = true;
					toConvert-=twoPower;
				}
				twoPower/=2;
				i++;
			}while(toConvert!=0);
		
			return new BinaryNumber(number);
		}
	}
	
	public String toString()
	{
		char[] charArray = new char[8];
		
		for(int i=0; i<8; i++)
		{
			charArray[i] = _number[i] ? '1' : '0';
		}
		return new String(charArray);
	}
	
}
