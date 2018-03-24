//
// Copyright 2018 Juraj Durech <durech.juraj@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include <bastapir/common/ByteRange.h>

namespace bastapir
{
	class ByteArray : public std::vector<byte>
	{
	public:
		
		typedef std::vector<byte> parent_class;
		
		using parent_class::parent_class;
		using parent_class::assign;
		using parent_class::insert;
		
		ByteArray()
		{
		}
		
		//
		// Interaction with ByteRange class
		//
		ByteArray(const ByteRange & range) : ByteArray(range.begin(), range.end())
		{
		}
		
		ByteArray& operator=(const ByteRange& range)
		{
			parent_class::assign(range.begin(), range.end());
			return *this;
		}
		
		void assign(const ByteRange & range)
		{
			parent_class::assign(range.begin(), range.end());
		}
		
		ByteArray & append(const ByteRange & range)
		{
			parent_class::insert(end(), range.begin(), range.end());
			return *this;
		}
		
		iterator insert(const_iterator position, const ByteRange & range)
		{
			return parent_class::insert(position, range.begin(), range.end());
		}
		
		ByteRange byteRange() const
		{
			return ByteRange(data(), size());
		}
		
		// dirty.. automatic casting to ByteRange
		operator ByteRange () const
		{
			return byteRange();
		}
		
		
		//
		// Appending
		//
		
		// single element, the same as push_back()
		ByteArray & append(const value_type& val)
		{
			parent_class::push_back(val);
			return *this;
		}
		
		// fill
		ByteArray & append(size_type n, const value_type& val)
		{
			parent_class::insert(end(), n, val);
			return *this;
		}
		
		// range
		template <class InputIterator>
		ByteArray & append(InputIterator first, InputIterator last)
		{
			parent_class::insert(end(), first, last);
			return *this;
		}
		
		// initializer list
		ByteArray & append(std::initializer_list<value_type> il)
		{
			parent_class::insert(end(), il);
			return *this;
		}
		
		// append [pointer, size]
		ByteArray & append(const_pointer p, size_type size)
		{
			parent_class::insert(end(), p, p + size);
			return *this;
		}
	};
}
