/*
*  Copyright 1998-2004 Russell Marks
 * Copyright 2018 Juraj Durech <durech.juraj@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 This file contains original conversion routine written by Russell Marks,
 for his `zmakebas` text to BASIC converter. The source code was released
 for public domain, so I hope that Apache license doesn't break that.
 */

#include "Double2Speccy.h"
#include <math.h>

namespace bastapir
{
namespace bas
{
	bool dbl2spec(double num, int & exp, long & man)
	{
		// check for small integers
		if (num == (long)num && num >= -65535.0 && num <= 65535.0)
		{
			// ignores sign - see below, which applies to ints too.
			long tmp = (long)fabs(num);
			exp = 0;
			man = ((tmp % 256) << 16) | ((tmp >> 8) << 8);
		}
		else
		{
			// It appears that the sign bit is always left as 0 when floating-point
			// numbers are embedded in programs, and the speccy appears to use the
			// '-' character to detemine negativity - tests confirm this.
			// As such, we *completely ignore* the sign of the number.
			// exp is 0x80+exponent.
			num = fabs(num);
			
			// binary standard form goes from 0.50000... to 0.9999...(dec), like
			// decimal which goes from        0.10000... to 0.9999....
			//
			// as such, if the number is >=1, it gets divided by 2, and exp++.
			// And if the number is <0.5, it gets multiplied by 2, and exp--.
			
			exp = 0;
			while(num >= 1.0)
			{
				num /= 2.0;
				exp++;
			}
			while(num < 0.5)
			{
				num*=2.0;
				exp--;
			}
			
			// so now the number is in binary standard form in exp and num.
			// we check the range of exp... -128 <= exp <= 127.
			// (if outside, we return error (i.e. 0))
			
			if (exp < -128 || exp > 127) {
				return false;
			}
			
			exp = 128 + exp;
			
			// so now all we need to do is roll the bits off the mantissa in `num'.
			// we start at the 0.5ths bit at bit 0, and shift left 1 each time
			// round the loop.
			
			num *= 2.0; // make it so that the 0.5ths bit is the integer part,
			// and the rest is the fractional (0.xxx) part.
			
			man = 0;
			for (int f = 0; f < 32; f++)
			{
				man <<= 1;
				man |= (int)num;
				num -= (int)num;
				num *= 2.0;
			}
			
			// Now, if (int)num is non-zero (well, 1) then we should generally
			// round up 1. We don't do this if it would cause an overflow in the
			// mantissa, though.
			
			if ((int)num && man != 0xFFFFFFFF) {
				man++;
			}
			
			// finally, zero out the top bit
			man &= 0x7FFFFFFF;
		}
		// done
		return true;
	}

} // bastapir::bas
} // bastapir
