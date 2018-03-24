/*
 * Copyright 2016 Juraj Durech <durech.juraj@gmail.com>
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

#pragma once

#if !defined(BASTAPIR_NO_EXCEPTIONS)
// Exceptions are enabled
#include <stdexcept>
#include <new>
#endif

namespace bastapir
{
namespace detail
{
	//
	// MARK: Exceptions wrapper (grabbed from cc7)
	//

#if defined(BASTAPIR_NO_EXCEPTIONS)
	// Exceptions are disabled. All errors are treated as assert() for this build.
	#define __BASTAPIR_THROW_EXCEPTION(exception, text)		assert(false)
	#define __BASTAPIR_THROW_EXCEPTION_NOP(exception, text)	assert(false)
#else
	// Exeptions are enabled
	#define __BASTAPIR_THROW_EXCEPTION(exception, text)		throw exception(text)
	#define __BASTAPIR_THROW_EXCEPTION_NOP(exception, text)	throw exception()
#endif

	
	template <typename T> class ExceptionsWrapper {
	public:
		
		static T & out_of_range()
		{
			__BASTAPIR_THROW_EXCEPTION(std::out_of_range, "out of range");
			return _foo;
		}
		
		static T & invalid_argument()
		{
			__BASTAPIR_THROW_EXCEPTION(std::invalid_argument, "invalid argument");
			return _foo;
		}
		
		static T & length_error()
		{
			__BASTAPIR_THROW_EXCEPTION(std::length_error, "length error");
			return _foo;
		}
		
		static T & allocation_error()
		{
			__BASTAPIR_THROW_EXCEPTION_NOP(std::bad_alloc, "not enough memory");
			return _foo;
		}
		
		static T & not_implemented()
		{
			__BASTAPIR_THROW_EXCEPTION(std::logic_error, "not implemented yet");
			return _foo;
		}
		
		static T & forbidden_value()
		{
			assert(false);
			return _foo;
		}
		
	private:
		static T _foo;
	};
	
	template <typename T> T ExceptionsWrapper<T>::_foo;
	
	
} // bastapir::detail
} // bastapir
