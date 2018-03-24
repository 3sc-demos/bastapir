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

#include <bastapir/common/Types.h>

namespace bastapir
{
	class Tokenizer
	{
	public:
		
		// MARK: - Supporting types
		
		typedef std::string::const_iterator		iterator;
		typedef std::string::difference_type	difference;
		
		struct Range
		{
			iterator begin;
			iterator end;
			
			std::string content() {
				return std::string(begin, end);
			}
		};
		
		struct State
		{
			iterator	pos;
			Range		line;
			Range		capture;
			bool		updateLineEnd;
			size_t		lineNumber;
		};
		
		struct PositionInfo
		{
			size_t lineNumber = 0;
			size_t offsetAtLine = 0;
		};
		
		// MARK: - Constructor
		
		Tokenizer(const Range range);
		Tokenizer(const iterator begin, const iterator end);
		
		// MARK: - Config
		
		/// Resets position to the beginning of string
		void reset();
		
		/// Changes behavior of isEnd() method, which will report end at the end of line.
		/// In this mode, you need to call nextLine() to go to the next line
		void setStopAtLineEnd(bool stop);
		
		/// Returns value previously set in `setStopAtLineEnd()`
		bool isStopAtLineEnd() const;
		
		/// Stores a whole state of tokenizer to structure
		State state() const;
		
		/// Restore previously stored state
		void restoreState(const State & state);
		
		/// Returns information about current position
		PositionInfo positionInfo() const;
		
		// MARK: - Walking over string
		
		/// Returns true if tokenizer is at the end of line
		bool isEnd() const;
		
		/// Returns current position iterator.
		iterator position() const;
		
		/// Returns current limits. If `isStopAtLineEnd` is true then returns range of line, otherwise whole string.
		const Range & limit() const;
		
		/// Returns char at current position and moves to the next position.
		/// If 0 is returned, then we're at the end.
		char getChar();
		
		/// Returns char at offset to current position, or 0 if offset is out of range.
		char charAt(difference offset = 0) const;
		
		
		// MARK: - Move forwad / backward
		
		/// Moves currsor by given offset. Returns true if that operation is possible, or
		/// false if offset is out of range.
		bool movePosition(difference offset = 1);
		
		/// Go immediately to the next line
		bool nextLine();
		
		/// Returns range for current line.
		Range line();
		
		
		// MARK: - Search
		
		/// Skip whitespace characters. The current position will end at first non-whitespace character or at the end.
		bool skipWhitespace();
		
		/// Skips characters until function returns false or the end is reached. The function receives consecutive characters
		/// in its paramter.
		bool skipUntil(int (*function)(int));
		
		bool contains(const std::string & string, bool case_insensitive = false);
		
		
		// MARK: - Range capture
		
		/// Sets capture begin and end to current position.
		void resetCapture();
		
		/// Sets capture end to current position and returns reference to current range.
		Range capture();
		
		/// Returns current capture and then resets capture range to current position
		Range captureAndReset();
		
	private:

		bool isRealEnd() const;
		char realGetChar();
		char realCharAt(difference offset) const;
		
		void updateLineEnd();

		bool			_stop_at_lf;
		const Range		_str;
		State 			_state;
		
	};
}
