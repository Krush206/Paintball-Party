


#define USE_ASM_FLOAT_CONVERSION
//#define USE_BINARY_FLOAT_CONVERSION
//#define USE_C99_FLOAT_CONVERSION
#define USE_STDLIB_FLOAT_CONVERSION



#define	_ISOC9X_SOURCE	1
#define _ISOC99_SOURCE	1

#include <math.h>
#include <float.h>

#include "round.h"


#if 0
#elif (defined USE_ASM_FLOAT_CONVERSION) && (defined _MSC_VER)
	int iround(double a) {
		int r; 
		__asm fld a   ;
		__asm fistp r ;
		return r;
	}
	int round_down(double a) {return iround(floor(a));}
	int round_up  (double a) {return iround(ceil(a));}	
#elif (defined USE_ASM_FLOAT_CONVERSION) && (defined __GNUC__) && (defined __i386__)
	int iround( double a ) {
		int r;
		__asm__ __volatile__( 
			"fldl %1    \n\t"
			"fistpl %0  \n\t"
			: "=m" (r)
			: "m" (a)
		);
		return r;
	}
	int round_down(double a) {return iround(floor(a));}
	int round_up  (double a) {return iround(ceil(a));}	
#elif defined USE_BINARY_FLOAT_CONVERSION
	//bigdouble is 2**52+2**51
	//it's that size to force any sane number x to satisfy
	//the condition: 2**52 <= x < 2**53
	//2**52 is important because the floating point 
	//format gives doubles 52 bits of significand
#	define BIGDOUBLE 6755399441055744.0
	int iround(double a) {
		int i;
		a = a + (BIGDOUBLE + 0);
		i = *((Uint32*)&a);
		return i;
	}
	int round_down(double a) {
		int i;
		a = a + 6755399441055743.5;
	//	a = a + (BIGDOUBLE - 0.5);//subtracting the 0.5 inside the paranthesis shouldn't work, but since it does we'll use it
		i = *((Uint32*)&a);
		return i;
	}
	int round_up  (double a) {
		int i;
		a = a + 6755399441055744.5;
		i = *((Uint32*)&a);
		return i;
	}
#elif defined USE_C99_FLOAT_CONVERSION
	int iround(double a) {
		return rint(a);
	}
	int round_down(double a) {return rint(floor(a));}
	int round_up  (double a) {return rint(ceil(a));}
#elif defined USE_STDLIB_FLOAT_CONVERSION
	int iround     (double a) {return (int)floor(a+0.5);}
	int round_down(double a) {return (int)floor(a);}
	int round_up  (double a) {return (int)ceil(a);}
#else
#	error No rounding implementation used
#endif

