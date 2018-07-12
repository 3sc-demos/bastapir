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
	/// The `Keywords` class is translating textual BASIC keywords to byte codes.
	/// The class also contains table for converting escape codes suppored in
	/// BASIC strings to byte codes. For example, you can use "\a" to generate
	/// UDG character "A".
	///
	/// The instance must be initialized for one of supported BASIC dialects.
	class Keywords
	{
	public:
		
		// MARK: - Public interface
		
		/// BASIC dialect
		enum Dialect
		{
			Dialect_48K,
			Dialect_128K
		};
		
		/// Constructs Keywords for given |dialect|
		Keywords(Dialect dialect);
		
		/// Changes dialect of keywords. The method also regenerates internal
		/// keywords & escape codes tables.
		void setDialect(Dialect dialect);
		
		/// Returns dialect assigned to Keywords object.
		Dialect dialect() const;
		
		/// Looks whether there's a BASIC keyword at |begin| begin position. The |end| parameter defines end of
		/// available string. Returns keyword's code or 0 if string at |begin| is not a keyword.
		byte findKeyword(const Tokenizer::iterator begin, const Tokenizer::iterator end, size_t & out_matched_size) const;
		
		/// Looks whether there's an string escape code at |begin| position. The |end| parameter defines end of
		/// available string. Returns escape code or 0 if escape sequence is unknown.
		byte findEscapeCode(const Tokenizer::iterator begin, const Tokenizer::iterator end, size_t & out_matched_size) const;
		
		static const byte Code_BIN;		// code for "BIN" keyword
		static const byte Code_REM;		// code for "REM" keyword
		static const byte Code_NUM;		// escape code for float number representation
		static const byte Code_ENT;		// newline character
		
	private:
		
		// MARK: - Private interface
		
		/// Internal structure representing BASIC keyword
		struct Keyword
		{
			std::string keyword;		// an actual keyword
			bool special;				// if true, keyword contains non-alpha characters
			byte code;					// translated code
		};
		
		/// Sort predicate for sorting Keywords by length. Longer keywords are first.
		struct LongerKeywordPredicate
		{
			inline bool operator() (const Keyword & k1, const Keyword & k2)
			{
				return k1.keyword.size() > k2.keyword.size();
			}
		};
		
		/// Internal structure representing escaped string sequence.
		/// For example, \t is translated to UDG character T
		struct EscapeCode
		{
			std::string sequence;		// sequence of characters after backslash.
			byte code;					// translated code
		};
		
		/// Stored BASIC dialect
		Dialect					_dialect;
		/// All BASIC keywords
		std::vector<Keyword>	_keywords;
		/// Unique first characters from each keyword. The string helps
		/// with keywords search.
		std::string 			_keywordsFirstChars;
		/// All escape codes
		std::vector<EscapeCode>	_escapeCodes;
		
		/// Setups internal structures for given BASIC dialect.
		void setupStructures(Dialect d);
		
		/// Prepares & returns list of Keyword structures for given BASIC dialect.
		static std::vector<Keyword> prepareKeywords(Dialect d);
		
		/// Prepares & returns list of EscapeCode structures for given BASIC dialect.
		static std::vector<EscapeCode> prepareEscapeCodes(Dialect d);
	};
	
	
} // bastapir::bas
} // bastapir
