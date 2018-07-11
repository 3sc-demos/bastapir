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

#include <bastapir/common/Tokenizer.h>
#include <bastapir/common/ErrorLogging.h>
#include <bastapir/common/SourceFile.h>
#include <bastapir/bas/Keywords.h>
#include <map>
#include <tuple>

namespace bastapir
{
namespace bas
{
	class BasicTextParser
	{
	public:
		/// MARK: - Public interface
		
		/// The `Variable` structure represents variable or constant injected into the BASIC parser.
		/// The structure is also used for symbolic line numbers, so you can
		struct Variable
		{
			/// Variable's symbolic name.
			std::string name;
			/// Variable' value.
			std::string value;
			/// If true, value property is valid and this Variable object has its value.
			bool isResolved;
			
			/// Static method returns structure representing unresolved variable.
			static Variable variable(const std::string & name) {
				return Variable { name, "", false };
			}
			
			/// Static method returns structure representing resolved constant.
			static Variable constant(const std::string & name, const std::string & value) {
				return Variable { name, value, true };
			}
			
			/// Assigns a new value to the variable. The function also sets `isResolved` property to true.
			void setValue(const std::string & v) {
				value = v;
				isResolved = true;
			}
		};
		
		/// The `Options` structure contains various parameters configurable in the parser.
		struct Options
		{
			/// First line number in case that automatic line numbering is used.
			U16 	initialLineNumber = 10;
			/// Line number increment in case that automatic line numbering is used.
			U16		lineNumberIncrement = 2;
			/// (not implemented yet) If true, then all serialized numbers will have "0"
			/// in textual representation.
			bool	shadowNumbers = false;
		};
		
		/// Construcst BasicTextParser object. Parameter |log| is required and you have to provide
		/// error logging facility. You can also specify a dialect of BASIC dialect.
		BasicTextParser(ErrorLogging * log, Keywords::Dialect dialect = Keywords::Dialect_48K);
		
		/// Sets options structure to the parser.
		void setOptions(const Options & options);
		
		/// Returns constant reference to internal Options structure
		const Options & options() const;
		
		/// Returns mutable reference to internal Options structure.
		Options & options();
		
		/// Assign constants which can be referenced from BASIC source code as `@ConstantName`. Only numeric
		/// values are supported at this time. All provided `Variable` ojects must be resolved (e.g. has to contain
		/// a value);
		/// Returns true if each constant has a value and false if some Variable is not resolved.
		bool setConstants(std::vector<Variable> & constants);
		
		/// Resolves variable or constant with given |name|. The function returns tuple, where the first parameter
		/// is boolean determining whether variable was found and second is actual resolved value.
		std::tuple<bool, std::string> resolveVariable(const std::string & variable_name) const;
		
		/// Parses provided |source| and generates final program bytes. You have to specify |source_info| which may contain
		/// an information about source code. Optionally, you can change |variant| of Spectrun BASIC.
		/// Returns true if succeeded, false otherwise.
		bool parse(const std::string & source, const SourceFileInfo & source_info, Keywords::Dialect dialect = Keywords::Dialect_48K);
		
		/// Returns generated BASIC program bytes. The returned bytes are valid only when last `parse()` returned true.
		const ByteArray & programBytes() const;

	private:

		// MARK: - Private parser
		
		bool doParse();
		bool doParseLine();
		
		bool doParseLineNumber();
		bool doParseNumber(bool as_binary);
		bool doParseVariable(bool is_line_begin);
		bool doParseString();
		bool doParseLineEscape(bool is_line_begin);
		bool doParseKeywords(bool is_line_begin);
		bool doParseREM();

		/// Returns current BASIC line number.
		U16 currentBasicLineNumber();
		
		Tokenizer::Range captureVariableName();
		Tokenizer::Range captureNumber();
		Tokenizer::Range captureHexadecimalNumber();
		Tokenizer::Range captureBinaryNumber();
		
		
		// MARK: - Write bytes to stream.
		
		/// Writes one byte to the output stream.
		void writeByte(byte b);
		
		/// Writes range of bytes to the output stream.
		void writeRange(const ByteRange & range);
		
		/// Writes line number and reserved bytes for line length, to the output stream.
		/// If |automatic| parameter is true, then line number is automatically calculated.
		/// Returns false in case of error.
		bool writeLineNumber(int n, bool automatic);
		
		/// Closes currently processed line. The function writes length of the line at the beginning
		/// of its byte representation.
		/// Returns false in case of error.
		bool writeLastLineBytes();
		
		/// Writes floating point number |n| with its |textual_representation| to the output stream.
		/// Returns false if number |n| cannot be serialized.
		bool writeNumber(double n, const std::string & textual_representation);
		
		
		// MARK: - Variable management
		
		/// Variable lookup (const version). Function search for name in constants & vars maps.
		const Variable * findVariable(const std::string & name) const;
		
		/// Variable lookup. Function search for name in constants & vars maps.
		Variable * findVariable(const std::string & name);
		
		/// Adds new variable.
		bool addVariable(const Variable & var, bool is_line_number);
		
		/// Returns true if all variables are resolved and have value.
		/// If |dump_error| parameter is true, then an appropriate error is generated.
		bool isAllVariablesResolved(bool dump_error) const;
		
		
		// MARK: - Helpers
		
		/// Returns simple ErrorInfo structure.
		ErrorInfo errInfo() const {
			return MakeError(_sourceFileInfo);
		}
		
		/// Returns ErrorInfo structure with current line & column.
		ErrorInfo errInfoLC() const {
			auto pos_info = _tokenizer.positionInfoForLog();
			return MakeError(_sourceFileInfo, pos_info.lineNumber, pos_info.offsetAtLine);
		}
		
		// MARK: - Members
		
		typedef std::map<std::string, Variable> VarMap;
		
		ErrorLogging * _log;
		SourceFileInfo _sourceFileInfo;
		
		Options _options;
		VarMap _constants;
		VarMap _variables;
		Keywords _keywords;
		
		Tokenizer _tokenizer;
		
		/// Private context structure.
		struct CTX
		{
			U16 pass = 0;
			U16 basicLineNumber = 0;
			U16 basicLastLineNumber = 0;
			U16 processedLines = 0;
			size_t beginLineBytesOffset = 0;
			bool lineBegin = true;
			bool lineContainsBytes = false;
		};
		CTX _ctx;
		
		/// Returns new context for given pass.
		CTX makeContext(U16 pass) const {
			CTX ctx = CTX();
			ctx.pass = pass;
			return ctx;
		}
		
		/// Output bytes.
		ByteArray _output;
	};
	
} // bastapir::bas
} // bastapir
