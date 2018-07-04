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

#include <bastapir/bas/BasicTextParser.h>
#include "Double2Speccy.h"

namespace bastapir
{
namespace bas
{
	// MARK: - Class implementation -
	
	BasicTextParser::BasicTextParser(ErrorLogging * log, Keywords::Variant variant) :
		_log(log),
		_keywords(variant),
		_tokenizer(log)
	{
		assert(_log != nullptr);
	}
	
	bool BasicTextParser::setConstants(std::vector<Variable> &constants)
	{
		_constants.clear();
		for (auto && c: constants) {
			if (_constants.find(c.name) != _constants.end()) {
				_log->warning(errInfo(), "Constant `" + c.name + "` injected into BASIC source already exists. Ignoring new value.");
				continue;
			}
			_constants[c.name] = c;
		}
		return true;
	}
	
	std::tuple<bool, std::string> BasicTextParser::resolveVariable(const std::string & variable_name) const
	{
		auto var = findVariable(variable_name);
		if (var && var->isResolved) {
			return std::make_tuple(true, var->value);
		}
		return std::make_tuple(false, "");
	}
	
	bool BasicTextParser::parse(const std::string & source, const SourceFileInfo & source_info, Keywords::Variant variant)
	{
		// Prepare internal structures
		_sourceFileInfo = source_info;
		_tokenizer.setStopAtLineEnd(true);
		_tokenizer.resetTo(source.begin(), source.end());
		_keywords.setVariant(variant);
		
		// Clear variables & validate constants
		_variables.clear();
		for (auto && c: _constants) {
			if (!c.second.isResolved) {
				_log->error(errInfo(), "Constant `" + c.first + "` injected into BASIC has unresolved value.");
				return false;
			}
		}
		// Let's parse that string!!
		return doParse();
	}
	
	const ByteArray & BasicTextParser::programBytes() const {
		return _output;
	}
	
	
	// MARK: - Matching functions -
	
	static int match_VariableName(int c)
	{
		if (isalnum(c)) {
			return true;
		}
		return false;
	}
	
	static int match_BinaryNumber(int c)
	{
		return c == '1' || c == '0';
	}
	
	static int match_HexadecimalNumber(int c)
	{
		return (c >= '0' && c <= '9') ||
				(c >= 'a' && c <= 'f') ||
				(c >= 'A' && c <= 'F');
	}
	
	
	// MARK: - Private parser -
	
	bool BasicTextParser::doParse()
	{
		for (U16 pass = 1; pass <= 2; ++pass) {
			// Prepare ctx
			_ctx = CTX();
			_ctx.pass = pass;
			_output.clear();
			_tokenizer.reset();
			//
			while (true) {
				if (!doParseLine()) {
					return false;
				}
				if (!_tokenizer.nextLine()) {
					break;
				}
			}
			//
			if (pass == 1) {
				if (isAllVariablesResolved(true) == false) {
					return false;
				}
			} else {
				if (_ctx.processedLines == 0) {
					_log->error(errInfo(), "BASIC program is empty.");
					return false;
				}
				writeLastLineBytes();
				if (_output.empty()) {
					_log->error(errInfo(), "No bytes were generated from BASIC program.");
					return false;
				}
			}
		}
		return true;
	}
	
	
	bool BasicTextParser::doParseLine()
	{
		_ctx.lineBegin = true;
		
		while (true) {
			// Is this first character in line?
			bool is_line_begin = _ctx.lineBegin;
			_ctx.lineBegin = false;
			
			// Skip leading whitespace
			_tokenizer.skipWhitespace();
			char c = _tokenizer.charAt();
			if (0 == c) {
				// End of line, store new line character...
				writeByte(Keywords::Code_ENT);
				return true;
			}
			
			// #### Comment
			if ('#' == c) {
				// ignore rest of the line. This is source code comment
				return true;
			}
			
			// #### Escape to next line
			if ('\\' == c) {
				if (!doParseLineEscape(is_line_begin)) {
					return false;
				}
				continue;
			}
			
			// #### Variables
			if ('@' == c) {
				if (!doParseVariable(is_line_begin)) {
					return false;
				}
				continue;
			}
			
			// #### Numbers
			if (isnumber(c)) {
				if (is_line_begin) {
					if (!doParseLineNumber()) {
						return false;
					}
				} else {
					if (!doParseNumber(false)) {
						return false;
					}
				}
				continue;
			}
			if ('.' == c) {
				// Check if we're at the beginnig of line.
				if (is_line_begin) {
					_log->error(errInfoLC(), "Wrong line number.");
					return false;
				}
				// Floating point number starting with dot.
				if (!doParseNumber(false)) {
					return false;
				}
				continue;
			}
			
			// #### String
			if ('"' == c) {
				// String should not be at the beginning of line.
				if (is_line_begin) {
					_log->error(errInfoLC(), "Nonsense in BASIC.");
					return false;
				}
				if (!doParseString()) {
					return false;
				}
				continue;
			}
			
			// #### Keywords
			if (!doParseKeywords(is_line_begin)) {
				return false;
			}
			
			// End of mail loop. We can process next character now.
		}
		return true;
	}
	
	
	bool BasicTextParser::doParseLineNumber()
	{
		auto line_range = captureNumber();
		char c_after = _tokenizer.charAt();
		// There must be space character after line number.
		if (line_range.empty() || c_after == 0 || !isspace(c_after)) {
			_log->error(errInfoLC(), "Wrong line number.");
			return false;
		}
		int number = std::stoi(line_range.content());
		return writeLineNumber(number);
	}
	
	
	bool BasicTextParser::doParseNumber(bool as_binary)
	{
		// Regular number
		char c1 = _tokenizer.charAt();
		if (c1 == '0' || as_binary) {
			char c2 = _tokenizer.charAt(1);
			// Hexa
			if (c2 == 'x' || c2 == 'X') {
				// hexadecimal, as 0x or 0X
				_tokenizer.movePosition(2);
				auto hexadecimal = captureHexadecimalNumber();
				if (hexadecimal.empty()) {
					_log->error(errInfoLC(), "Invalid hexadecimal number.");
					return false;
				}
				int number = std::stoi(hexadecimal.content(), nullptr, 16);
				if (number > 0xFFFF) {
					_log->error(errInfoLC(), "Hexadecimal number is too big.");
					return false;
				}
				return writeNumber((double)number, std::to_string(number));
				
			} else if (c2 == 'b' || c2 == 'B' || as_binary) {
				// 0b... or 0B... or direct request for binary number
				if (c1 == '0' && c2 == 'b') {
					_tokenizer.movePosition(2);
				}
				auto binary = captureBinaryNumber();
				if (binary.empty()) {
					_log->error(errInfoLC(), "Invalid binary number.");
					return false;
				}
				int number = std::stoi(binary.content(), nullptr, 2);
				if (number > 0xFFFF) {
					_log->error(errInfoLC(), "Binary number is too big.");
					return false;
				}
				// As an optimization, we completely ignore binary numbers and write
				// them as regular numbers.
				return writeNumber((double)number, std::to_string(number));
			}
		}
			
		// Not binary, not hexadecimal... We need to parse number more deeply.
		_tokenizer.resetCapture();
		if (c1 != '.') {
			// IIII
			_tokenizer.skipWhile(isnumber);
		}
		c1 = _tokenizer.charAt();
		if (c1 == '.') {
			// .FFF
			_tokenizer.movePosition();
			_tokenizer.skipWhile(isnumber);
		}
		c1 = _tokenizer.charAt();
		if (c1 == 'E' || c1 == 'e') {
			// eMMM or EMMM
			_tokenizer.movePosition();
			_tokenizer.skipWhile(isnumber);
		}
		auto any_number = _tokenizer.capture();
		if (any_number.empty()) {
			_log->error(errInfoLC(), "Invalid number.");
			return false;
		}
		auto textual_repr = any_number.content();
		double number = std::stod(textual_repr);
		return writeNumber(number, textual_repr);
	}
	
	
	bool BasicTextParser::doParseVariable(bool is_line_begin)
	{
		// Start of label or variable? Skip `@` at first...
		_tokenizer.movePosition();
		auto variable_name = captureVariableName().content();
		if (is_line_begin) {
			if (_tokenizer.getChar() != ':') {
				_log->error(errInfoLC(), "Invalid symbolic line number.");
				return false;
			}
		}
		if (variable_name.empty()) {
			if (is_line_begin) {
				_log->error(errInfoLC(), "Invalid symbolic line number.");
			} else {
				_log->error(errInfoLC(), "Invalid usage of sybolic line number.");
			}
			return false;
		}
		
		if (_ctx.pass == 1) {
			// first pass, we're declaring stuff
			auto variable = Variable::variable(variable_name);
			if (is_line_begin) {
				// This is line number, we need to generate a next number & mark that
				// next real line should not increase line number.
				variable.setValue(std::to_string(nextLineNumber()));
				_ctx.doNotIncrementNextLine = true;
			}
			// Register variable.
			if (!addVariable(variable, true)) {
				return false;
			}
		} else {
			// 2nd pass is different. We need to use stored variables.
			if (is_line_begin) {
				// We already have value for this variable. Just consume line number
				// and mark "do not increment" flag as we did in first pass.
				nextLineNumber();
				_ctx.doNotIncrementNextLine = true;
				//
			} else {
				// Resolve variable. Currently only numeric variables are supported.
				bool resolved; std::string value;
				std::tie(resolved, value) = resolveVariable(variable_name);
				if (!resolved) {
					_log->error(errInfoLC(), "Unable to resolve value of variable `" + variable_name + "`. It looks like an internal error :(");
					return false;
				}
				double dbl_value = std::stod(value);
				return writeNumber(dbl_value, value);
			}
		}
		return true;
	}
	
	
	bool BasicTextParser::doParseString()
	{
		// We're still at `"` character, so reset the capture and move forward.
		_tokenizer.resetCapture();
		_tokenizer.movePosition();
		
		while (true) {
			char c1 = _tokenizer.getChar();
			if (0 == c1) {
				// End of line / End of file and string has not been closed.
				_log->error(errInfoLC(), "Unexpected end of string.");
				return false;
			}
			if ('"' == c1) {
				// End of string, doublequote escape in string.
				if (_tokenizer.isCharAt('"')) {
					// It's double quote escape, we should consume one more character
					// and continue within the string
					_tokenizer.movePosition();
					continue;
				}
				break;
			}
			if ('\\' == c1) {
				// Possible escape sequence.
				size_t captured_size;
				byte code = _keywords.findEscapeCode(_tokenizer.position(), _tokenizer.line().end, captured_size);
				if (code == 0) {
					_log->error(errInfoLC(), "Invalid charcter escape sequence in string.");
					return false;
				}
				// Go back in string, we don't want to capture `\`
				_tokenizer.movePosition(-1);
				writeRange(MakeRange(_tokenizer.capture().content()));
				_tokenizer.movePosition(1 + captured_size);
				_tokenizer.resetCapture();
				continue;
			}
		}
		// Write captured region and return with success
		writeRange(MakeRange(_tokenizer.capture().content()));
		return true;
	}
	
	
	bool BasicTextParser::doParseLineEscape(bool is_line_begin)
	{
		bool next_is_end = _tokenizer.charAt(1) == 0;
		if (!next_is_end) {
			_log->warning(errInfoLC(), "Characters after line escape (\\) will be ignored.");
		}
		_tokenizer.nextLine();
		return true;
	}
	
	
	bool BasicTextParser::doParseKeywords(bool is_line_begin)
	{
		char c = _tokenizer.charAt();
		
		size_t matched_size;
		byte code = _keywords.findKeyword(_tokenizer.position(), _tokenizer.line().end, matched_size);
		if (is_line_begin) {
			// Only keywords are allowed at the beginning of line
			if (code == 0) {
				_log->error(errInfoLC(), "Nonsense in BASIC.");
				return false;
			}
			// Now we know that code is keyword and we're at the beginning of line,
			// so we have to store line number.
			U16 line = nextLineNumber();
			if (!writeLineNumber(line)) {
				return false;
			}
		}
		if (code != 0) {
			// Keyword, we need to move cursor forward
			_tokenizer.movePosition(matched_size);
			if (code == Keywords::Code_BIN) {
				// BIN keyword requires a special handling. We're skipping BIN
				// completely
				if (is_line_begin) {
					_log->error(errInfoLC(), "Nonsense in BASIC.");
					return false;
				}
				_tokenizer.skipWhitespace();
				if (!doParseNumber(true)) {
					return false;
				}
				return true;
			}
			if (code == Keywords::Code_REM) {
				writeByte(code);
				return doParseREM();
			}
			
		} else if (isalpha(c)) {
			// Not a keyword, but regular character. Try to match regular BASIC variable
			writeRange(MakeRange(captureVariableName().content()));
			return true;
			
		} else {
			// Not a keyword, not a word, write that character to the output
			_tokenizer.movePosition();
			code = c;
		}
		// Looks good, write code to the stream.
		writeByte(code);
		return true;
	}
	
	bool BasicTextParser::doParseREM()
	{
		bool was_space = false;
		_tokenizer.skipWhitespace();
		while (!_tokenizer.isEnd()) {
			char c = _tokenizer.charAt();
			if (c == '\\') {
				was_space = false;
				if (_tokenizer.charAt(1) == 0) {
					// line escape...
					_tokenizer.nextLine();
					continue;
				}
				// Escape sequence in REM statement
				_tokenizer.movePosition();
				size_t matched_size;
				auto code = _keywords.findEscapeCode(_tokenizer.position(), _tokenizer.line().end, matched_size);
				if (code == 0) {
					_log->error(errInfoLC(), "Invalid escaped character in REM statement.");
					return false;
				}
				_tokenizer.movePosition(matched_size);
				writeByte(code);
				
			} else if (isspace(c)) {
				if (!was_space) {
					writeByte(' ');
					was_space = true;
				}
				_tokenizer.movePosition();
				
			} else if (isalnum(c)) {
				// Try to match whole words
				was_space = false;
				writeRange(MakeRange(captureVariableName().content()));
			
			} else {
				was_space = false;
				_tokenizer.movePosition();
				writeByte(c);
			}
		}
		// Success escape from REM means that we need to process next line
		// as new one.
		_ctx.lineBegin = true;
		return true;
	}
	
	//
	
	U16 BasicTextParser::nextLineNumber()
	{
		U16 next_line = _ctx.basicLineNumber;
		if (next_line == 0) {
			next_line = 10;	// defaulting to 10
		} else {
			if (!_ctx.doNotIncrementNextLine) {
				next_line += _ctx.basicLineStep;
			} else {
				_ctx.doNotIncrementNextLine = false;
			}
		}
		return next_line;
	}
	
	
	// MARK: - Capture sequences
	
	Tokenizer::Range BasicTextParser::captureNumber()
	{
		_tokenizer.resetCapture();
		_tokenizer.skipWhile(isnumber);
		return _tokenizer.capture();
	}

	Tokenizer::Range BasicTextParser::captureVariableName()
	{
		_tokenizer.resetCapture();
		_tokenizer.skipWhile(match_VariableName);
		return _tokenizer.capture();
	}
	
	Tokenizer::Range BasicTextParser::captureHexadecimalNumber()
	{
		_tokenizer.resetCapture();
		_tokenizer.skipWhile(match_HexadecimalNumber);
		return _tokenizer.capture();
	}

	Tokenizer::Range BasicTextParser::captureBinaryNumber()
	{
		_tokenizer.resetCapture();
		_tokenizer.skipWhile(match_BinaryNumber);
		return _tokenizer.capture();
	}
	

	// MARK: - Write bytes to stream.
	
	void BasicTextParser::writeByte(byte b)
	{
		if (_ctx.pass > 1) {
			_output.push_back(b);
			printf(">>> %02x    %lu\n", b, _output.size());
		}
	}
	
	void BasicTextParser::writeRange(const ByteRange & range)
	{
		if (_ctx.pass > 1) {
			_output.append(range);
			printf(">>> ");
			for (auto b: range) {
				printf("%02x ", b);
			}
			printf("     %lu\n", _output.size());
		}
	}
	
	bool BasicTextParser::writeLineNumber(int number)
	{
		if (number < 1 || number > 9999) {
			_log->error(errInfoLC(), "Wrong line number.");
			return false;
		}
		U16 n = number & 0xFFFF;
		if (n <= _ctx.basicLineNumber) {
			if (n == _ctx.basicLineNumber) {
				_log->error(errInfoLC(), "Line number is equal to previous one.");
			} else {
				_log->error(errInfoLC(), "Line number is lesser than previous one.");
			}
			return false;
		}
		
		_ctx.basicLineNumber = n;
		_ctx.processedLines++;
		
		if (_ctx.pass > 1) {
			// Serialize line number...
			// Write line number in BE
			auto end_last_line = _output.size();
			byte line_bytes[4] = { (byte)(_ctx.basicLineNumber >> 8), (byte)(_ctx.basicLineNumber & 0xff), 0, 0 };
			writeRange(MakeRange(line_bytes));
			// Capture point, where we need to write back size of this just started line
			auto begin_last_line = _ctx.beginLineBytesOffset;
			_ctx.beginLineBytesOffset = _output.size();
			if (end_last_line > 0 && begin_last_line >= 4) {
				U16 line_size = end_last_line - begin_last_line;
				// we need to write back size of current line.
				_output.at(begin_last_line - 2) =  line_size       & 0xFF;
				_output.at(begin_last_line - 1) = (line_size >> 8) & 0xFF;
			}
		}
		return true;
	}
	
	bool BasicTextParser::writeLastLineBytes()
	{
		if (_ctx.pass > 1) {
			auto end_last_line = _output.size();
			auto begin_last_line = _ctx.beginLineBytesOffset;
			if (end_last_line > 0 && begin_last_line >= 4) {
				U16 line_size = end_last_line - begin_last_line;
				// we need to write back size of current line.
				_output.at(begin_last_line - 2) =  line_size       & 0xFF;
				_output.at(begin_last_line - 1) = (line_size >> 8) & 0xFF;
			}
		}
		return true;
	}
	
	bool BasicTextParser::writeNumber(double n, const std::string & textual_representation)
	{
		int exponent;
		long mantissa;
		if (!dbl2spec(n, exponent, mantissa)) {
			_log->error(errInfoLC(), "Exponent out of range (number is too big)");
			return false;
		}
		// Text representation
		writeRange(MakeRange(textual_representation));
		
		// Binary representation
		byte b_repr[6];
		b_repr[0] = Keywords::Code_NUM;
		b_repr[1] = exponent;
		b_repr[2] = (mantissa >> 24) & 0xFF;
		b_repr[3] = (mantissa >> 16) & 0xFF;
		b_repr[4] = (mantissa >> 8 ) & 0xFF;
		b_repr[5] =  mantissa        & 0xFF;
		writeRange(MakeRange(b_repr));
		
		return true;
	}
	
	// MARK: - Variable management
	
	const BasicTextParser::Variable * BasicTextParser::findVariable(const std::string & name) const
	{
		auto it = _constants.find(name);
		if (it != _constants.end()) {
			return &it->second;
		}
		it = _variables.find(name);
		if (it != _variables.end()) {
			return &it->second;
		}
		return nullptr;
	}
	
	BasicTextParser::Variable * BasicTextParser::findVariable(const std::string & name)
	{
		auto it = _constants.find(name);
		if (it != _constants.end()) {
			return &it->second;
		}
		it = _variables.find(name);
		if (it != _variables.end()) {
			return &it->second;
		}
		return nullptr;
	}
	
	bool BasicTextParser::addVariable(const Variable & var, bool is_line_number)
	{
		auto current = findVariable(var.name);
		if (current != nullptr) {
			if (current->isResolved) {
				// Current is already resolved
				if (var.isResolved) {
					// If new one is also resolved, this means that we have duplicit symbol.
					if (is_line_number) {
						_log->error(errInfoLC(), "Duplicit symbolic line number `" + var.name + "` detected in BASIC file.");
					} else {
						_log->error(errInfoLC(), "Duplicit variable `" + var.name + "` injected into BASIC.");
					}
					return false;
				}
			} else {
				// current is not resolved, but if new one has value, then
				// we can update value of already stored one.
				if (var.isResolved) {
					current->setValue(var.value);
				}
			}
		} else {
			// New variable, store to database
			_variables[var.name] = var;
		}
		return true;
	}
	
	bool BasicTextParser::isAllVariablesResolved(bool dump_error) const
	{
		bool result = true;
		for (auto && var: _variables) {
			if (!var.second.isResolved) {
				result = false;
				if (dump_error) {
					_log->error(errInfo(), "Variable `" + var.first + "` injected into BASIC has unresolved value.");
				} else {
					break;
				}
			}
		}
		return result;
	}
	
} // bastapir::bas
} // bastapir
