/**
 * My bitwise operations.
 * "~" (tilde) = NOT ... "|" (pipe) = OR ... "^" (caret) = XOR ... "&" (ampersand) = AND
 * ">>" = right shift ... "<<" = left shift
 * See the Bitwise_operation Wiki for more info - http://en.wikipedia.org/wiki/Bitwise_operation
 */

#ifndef __MY_BIT
#define __MY_BIT

#define SET_ALL  |= 0xFF;
#define SET_BIT0 |= 0x01;
#define SET_BIT1 |= 0x02;
#define SET_BIT2 |= 0x04;
#define SET_BIT3 |= 0x08;
#define SET_BIT4 |= 0x10;
#define SET_BIT5 |= 0x20;
#define SET_BIT6 |= 0x40;
#define SET_BIT7 |= 0x80;

#define CLEAR_ALL  &= 0x00;
#define CLEAR_BIT0 &= 0xFE;
#define CLEAR_BIT1 &= 0xFD;
#define CLEAR_BIT2 &= 0xFB;
#define CLEAR_BIT3 &= 0xF7;
#define CLEAR_BIT4 &= 0xEF;
#define CLEAR_BIT5 &= 0xDF;
#define CLEAR_BIT6 &= 0xBF;
#define CLEAR_BIT7 &= 0x7F;


//TO_VAR, FROM_VAR unsigned char
//BIT_NUM the bit to get 0..7
// input byte is FROM_VAR
// return value: TO_VAR = 0 or TO_VAR = 1
// process: set TO = FROM; right shift it so the relevant bit is now bit 0;
// & with 1 to remove all other bits (TO_VAR can now only equal 1 or 0)
#define GET_BIT(TO_VAR, FROM_VAR, BIT_NUM)  {\
	TO_VAR=FROM_VAR;\
	TO_VAR>>=BIT_NUM;\
	TO_VAR&=0x01;\
}

// Some alternative macros for bit manipulation
// Usage eg: BIT_SET(ADMUX, BIT(REFS0));  // Set reference to AVcc
#define BIT(x)                (1 << (x))  // replacement for the poorly named _BV

#define SET_BIT(var, mask)    ((var) |= (unsigned char)(mask))
#define CLEAR_BIT(var, mask)  ((var) &= (unsigned char)~(mask))
#define TOGGLE_BIT(var, mask) ((var) ^= (unsigned char)(mask))
#define READ_BIT(var, mask)   ((var) &  (unsigned char)(mask))

#define ASSIGN_BIT(var, val, mask) ((var) = (((var)&~(unsigned char)(mask))|((val)&(unsigned char)(mask))))

#define	hi(Value) 	(Value >> 8)
#define lo(Value)	(Value & 0xFF)

#endif //__MY_BIT
