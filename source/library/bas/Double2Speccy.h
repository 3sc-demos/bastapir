/*
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

#include <bastapir/common/Types.h>

namespace bastapir
{
namespace bas
{
	//
	// dbl2spec() converts a double to an inline-basic-style speccy FP number.
	//
	// usage: dbl2spec(num,&exp,&man);
	//
	// num  is double to convert.
	// pexp is an int * to where to return the exponent byte.
	// pman is an unsigned long * to where to return the 4 mantissa bytes.
	//      bit 31 is bit 7 of the 0th (first) mantissa byte, and bit 0 is bit 0 of
	//      the 3rd (last). As such, the unsigned long returned *must* be written
	//      to whatever file in big-endian format to make any sense to a speccy.
	//
	// returns true if ok, false if exponent too big.
	//
	bool dbl2spec(double num, int & exp, long & man);
}
}
