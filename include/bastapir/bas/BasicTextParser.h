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
	
		struct Variable
		{
			std::string name;
			std::string value;
			bool isResolved;
			
			static Variable variable(const std::string & name) {
				return Variable { name, "", false };
			}
			
			static Variable constant(const std::string & name, const std::string & value) {
				return Variable { name, value, true };
			}
			
			void setValue(const std::string & v) {
				value = v;
				isResolved = true;
			}
		};
		
		
		BasicTextParser(ErrorLogging * log, Keywords::Variant variant = Keywords::Variant_48K);
		
		bool setConstants(std::vector<Variable> & constants);
		
		std::tuple<bool, std::string> resolveVariable(const std::string & variable_name) const;
		
		bool parse(const std::string & source, const SourceFileInfo & source_info, Keywords::Variant variant = Keywords::Variant_48K);
		
		const ByteArray & programBytes() const;

	private:

		// MARK: - Private parser
		
		bool doParse();
		bool doParseLine();
		
		bool doParseVariable(bool is_line_begin);
		bool doParseLineNumber();
		bool doParseNumber(bool as_binary);
		bool doParseString();
		
		Tokenizer::Range captureVariableName();
		Tokenizer::Range captureNumber();
		Tokenizer::Range captureHexadecimalNumber();
		Tokenizer::Range captureBinaryNumber();
		
		// MARK: - Write bytes to stream.
		
		bool writeLineNumber(int n);
		bool writeNumber(double n, const std::string & textual_representation);
		U16 nextLineNumber();
		
		// MARK: - Variable management
		
		/// Variable lookup (const version). Function search for name in constants & vars maps.
		const Variable * findVariable(const std::string & name) const;
		/// Variable lookup. Function search for name in constants & vars maps.
		Variable * findVariable(const std::string & name);
		
		/// Adds new variable.
		bool addVariable(const Variable & var, bool is_line_number);
		
		/// Returns true if all variables are resolved and have value.
		bool isAllVariablesResolved(bool dump_error) const;
		
		// MARK: - Helpers
		
		/// Returns simple ErrorInfo structure.
		ErrorInfo errInfo() const {
			return MakeError(_sourceFileInfo);
		}
		
		/// Returns ErrorInfo structure with current line & column.
		ErrorInfo errInfoLC() const {
			auto pos_info = _tokenizer.positionInfo();
			pos_info.lineNumber++;
			pos_info.offsetAtLine++;
			return MakeError(_sourceFileInfo, pos_info.lineNumber, pos_info.offsetAtLine);
		}
		
		// MARK: - Members
		
		typedef std::map<std::string, Variable> VarMap;
		
		
		ErrorLogging * _log;
		SourceFileInfo _sourceFileInfo;
		
		VarMap _constants;
		VarMap _variables;
		Keywords _keywords;
		
		Tokenizer _tokenizer;
		
		struct CTX
		{
			U16 pass = 0;
			U16 basicLineNumber = 0;
			U16 basicLineStep = 2;
			U16 processedLines = 0;
			bool doNotIncrementNextLine = false;
			bool inREM = false;
		};
		CTX _ctx;
		ByteArray _output;
	};
	
} // bastapir::bas
} // bastapir
