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
#include <bastapir/common/ErrorLogging.h>

namespace bastapir
{
	class Tokenizer
	{
	public:
		
		// MARK: - Supporting types
		
		typedef std::string::const_iterator		iterator;
		typedef std::string::difference_type	difference;
		
		/// The `Range` structure contains `begin` & `end` of string.
		struct Range
		{
			iterator begin;
			iterator end;
			
			/// Returns string between begin & end.
			std::string content() {
				return std::string(begin, end);
			}
			
			/// Returns true if `begin` is equal to `end`
			bool empty() {
				return begin == end;
			}
		};
		
		/// The `State` structure contains a full state of tokenizer.
		struct State
		{
			iterator	pos;
			Range		line;
			Range		capture;
			size_t		lineNumber;
			bool		updateLineEnd;
		};
		
		/// The `PositionInfo` structure contains an information about current
		/// position in the source string.
		struct PositionInfo
		{
			/// Current line number
			size_t lineNumber = 0;
			/// Offset at current line
			size_t offsetAtLine = 0;
		};
		
		// MARK: - Constructor
		
		/// Constructs Tokenizer object with optional error logging.
		Tokenizer(ErrorLogging * log = nullptr);
		
		// MARK: - Config
		
		/// Assing (optional) error logging to the tokenizer. Setting nullptr will reset
		/// the currently assigned logger.
		void setErrorLogging(ErrorLogging * logger);
		
		/// Resets tokenizer for a new string processing.
		void resetTo(const Range range);
		
		/// Resets tokenizer for a new string processing.
		void resetTo(const iterator begin, const iterator end);
		
		/// Resets position to the beginning of string
		void reset();
		
		/// Resets position to the beginning of current line.
		void resetLine();
		
		/// Changes behavior of isEnd() method, which will report end at the end of line.
		/// In this mode, you need to call nextLine() to move to the next line.
		//
		/// Note that this information is not a part of the `State` stucture.
		void setStopAtLineEnd(bool stop);
		
		/// Returns current mode of `isEnd()` method. If true, then Tokenizer reports
		/// end at the end of current line.
		bool isStopAtLineEnd() const;
		
		/// Stores a whole state of tokenizer to structure
		State state() const;
		
		/// Restore previously stored state.
		void restoreState(const State & state);
		
		/// Returns information about current position.
		PositionInfo positionInfo() const;
		
		/// Returns information about current position where `lineNumber` and `offsetAtLine` are increased by 1.
		PositionInfo positionInfoForLog() const;
		
		// MARK: - Walking over string
		
		/// Returns true if tokenizer is at the end of string or at the end of line. The behavior of this
		/// method depends on mode changed in `setStopAtLineEnd()`.
		bool isEnd() const;
		
		/// Returns current position iterator.
		iterator position() const;
		
		/// Returns current limits. If `isStopAtLineEnd` is true then returns range of line, otherwise a whole string.
		const Range & limit() const;
		
		/// Returns char at current position and moves to the next position.
		/// If 0 is returned, then we're at the end.
		char getChar();
		
		/// Returns char at offset to current position, or 0 if offset is out of range.
		char charAt(difference offset = 0) const;
		
		/// Returns true if char at given |offset| to current position is equal to |c|
		char isCharAt(char c, difference offset = 0) const;
		
		
		// MARK: - Move forwad / backward
		
		/// Moves currsor by given offset. Returns true if that operation is possible, or
		/// false if offset is out of range.
		bool movePosition(difference offset = 1);
		
		/// Go immediately to the next line. Returns false if we're at the end of string.
		bool nextLine();
		
		/// Returns range for current line.
		Range line();
		
		
		// MARK: - Search
		
		/// Skip whitespace characters. The current position will end at first non-whitespace character or at the end.
		bool skipWhitespace();
		
		/// Skip characters while provided |match_function| returns true. The current position will end at the last
		/// matched character.
		bool skipWhile(int (*match_function)(int));
		
		/// Iterates over the string, while |match_fucntion| returns false and stops iterations when function returns true.
		/// The current position will end at the next to matched character.
		bool searchFor(int (*match_function)(int));
				
		// MARK: - Range capture
		
		/// Sets capture begin and end to current position.
		void resetCapture();
		
		/// Sets capture end to current position and returns reference to current range.
		Range capture();
		
		/// Returns current capture and then resets capture range to current position
		Range captureAndReset();
		
		// MARK: - Other
		
		/// Prints debug information to stdout.
		void _debugInfo() const;
		
	private:

		/// Returns true if position is at the end of string.
		bool isRealEnd() const;
		
		/// Returns character at current position and moves position to the next character.
		/// This function ignores mode set in setStopAtLineEnd() method.
		char realGetChar();
		
		/// Returns character at current+offset position. If position is out of range, then returns 0.
		/// This function ignores mode set in setStopAtLineEnd() method.
		char realCharAt(difference offset) const;
		
		/// Updates internal line end pointer.
		void updateLineEnd();

		bool	_stop_at_lf;
		Range	_str;
		State	_state;
		
		ErrorLogging * _log;
		
	};
}
