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

namespace bastapir
{
namespace bas
{
	class Keywords
	{
	public:
		
		enum Variant
		{
			Variant_48K,
			Variant_128K
		};
		
		Keywords(Variant variant);
		
		void setVariant(Variant variant);
		Variant variant() const;
		
		/// Looks whether there's a BASIC keyword at |begin| begin position. The |end| parameter defines end of
		/// available string. Returns keyword's code or 0 if string at |begin| is unknown.
		byte findKeyword(const Tokenizer::iterator begin, const Tokenizer::iterator end, size_t & out_matched_size) const;
		
		/// Looks whether there's an string escape code at |begin| position. The |end| parameter defines end of
		/// available string. Returns escape code or 0 if escape sequence is unknown.
		byte findEscapeCode(const Tokenizer::iterator begin, const Tokenizer::iterator end, size_t & out_matched_size) const;
		
		static const byte Code_BIN;		// code for "BIN" keyword
		static const byte Code_REM;		// code for "REM" keyword
		static const byte Code_NUM;		// escape code for float number representation
		static const byte Code_ENT;		// newline character
		
	private:
		
		struct Keyword
		{
			std::string primary;
			std::string alternate;
			byte code;
		};
		
		struct EscapeCode
		{
			std::string sequence;
			byte code;
		};
		
		std::vector<Keyword>	_keywords;
		std::string 			_keywordsFirstChars;
		std::vector<EscapeCode>	_escapeCodes;
		Variant					_variant;
		
		
		
		static std::vector<Keyword> prepareKeywords(Variant v);
		static std::vector<EscapeCode> prepareEscapeCodes(Variant v);
		static std::string prepareKeywordsFirstChars(const std::vector<Keyword> & keywords);
	};
	
	
} // bastapir::bas
} // bastapir
