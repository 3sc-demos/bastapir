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

#include <bastapir/types/StringTokenizer.h>
#include <assert.h>
#include <ctype.h>

namespace bastapir
{

#define _CHECK_LE() assert(_state.updateLineEnd == false);

	// MARK: - Public methods
	StringTokenizer::StringTokenizer(const iterator begin, const iterator end) :
		_str_begin(begin),
		_str_end(end),
		_stop_at_lf(false),
		_state({
			begin,
			{begin, begin},
			{begin, begin},
			0
		})
	{
	}
	
	StringTokenizer::StringTokenizer(const Range range) :
		StringTokenizer(range.begin, range.end)
	{
	}
	
	void StringTokenizer::reset()
	{
		_state.pos 				= _str_begin;
		_state.line.begin 		= _str_begin;
		_state.line.end 		= _str_begin;
		_state.updateLineEnd	= true;
		_state.lineNumber		= 0;
		
		if (_stop_at_lf) {
			updateLineEnd();
		}
	}
	
	void StringTokenizer::setStopAtLineEnd(bool stop)
	{
		_stop_at_lf = stop;
	}
	
	bool StringTokenizer::isStopAtLineEnd() const
	{
		return _stop_at_lf;
	}
	
	bool StringTokenizer::isEnd() const
	{
		if (!_stop_at_lf) {
			return isRealEnd();
		}
		_CHECK_LE();
		return _state.pos == _state.line.end;
	}
	
	char StringTokenizer::getChar()
	{
		if (!isEnd()) {
			return *(_state.pos++);
		}
		return 0;
	}
	
	char StringTokenizer::charAt(difference offset) const
	{
		auto begin = _stop_at_lf ? _state.line.begin : _str_begin;
		auto end   = _stop_at_lf ? _state.line.end : _str_end;
		if (offset > 0) {
			if (offset >= std::distance(_state.pos, end)) {
				return 0;
			}
		} else {
			if (offset < std::distance(_state.pos, begin)) {
				return 0;
			}
		}
		return *(_state.pos + offset);
	}
	
	char StringTokenizer::realCharAt(difference offset) const
	{
		if (offset > 0) {
			if (offset >= std::distance(_state.pos, _str_end)) {
				return 0;
			}
		} else {
			if (offset < std::distance(_state.pos, _str_begin)) {
				return 0;
			}
		}
		return *(_state.pos + offset);
	}

	//
	
	bool StringTokenizer::isRealEnd() const
	{
		return _state.pos == _str_end;
	}
	
	char StringTokenizer::realGetChar()
	{
		if (!isRealEnd()) {
			return *(_state.pos++);
		}
		return 0;
	}
	
	bool StringTokenizer::nextLine()
	{
		if (!_stop_at_lf) {
			updateLineEnd();
		} else {
			_CHECK_LE();
		}

		char c = realGetChar();
		if (c != 0) {
			if (c == '\n') {
				// UNIX, do nothing...
			} else if (c == '\r') {
				// Handle crappy MS-DOS & Descendants
				c = realGetChar();
				if (c != '\n') {
					// TODO: throw an error, this is not right combination
					fprintf(stderr, "Invalid CR-LF sequence\n");
					_state.pos--;
				}
			}
			_state.lineNumber++;
		}
		
		_state.updateLineEnd = true;
		if (_stop_at_lf) {
			updateLineEnd();
			resetCapture();
		}
		
		return isRealEnd();
	}
	
	bool StringTokenizer::skipWhitespace()
	{
		return skipUntil(isspace);
	}
	
	bool StringTokenizer::skipUntil(int (*function)(int))
	{
		assert(function != nullptr);
		while (true) {
			char c = getChar();
			if (c == 0) {
				return false;
			}
			if (!function(c)) {
				return true;
			}
		}
	}
	
	StringTokenizer::Range StringTokenizer::line()
	{
		updateLineEnd();
		return _state.line;
	}
	
	void StringTokenizer::resetCapture()
	{
		_state.capture.begin 	= _state.pos;
		_state.capture.end 		= _state.pos;
	}
	
	StringTokenizer::Range StringTokenizer::capture()
	{
		_state.capture.end = _state.pos;
		return _state.capture;
	}
	
	StringTokenizer::Range StringTokenizer::captureAndReset()
	{
		auto c = capture();
		_state.capture.begin = _state.capture.end;
		return c;
	}
	
	// MARK: - Private methods
	
	void StringTokenizer::updateLineEnd()
	{
		if (_state.updateLineEnd) {
			_state.line.end = _state.line.begin;
			while (_state.line.end != _str_end) {
				auto c = *_state.line.end;
				if (c == '\n' || c == '\r') {
					break;
				}
				++_state.line.end;
			}
			_state.updateLineEnd = false;
		}
	}
	
} // bastap

