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

#include "Keywords.h"
#include <set>

namespace bastapir
{
namespace bas
{
	// MARK: - Support functions
	
	static bool matchString(const std::string & str, Tokenizer::iterator begin, Tokenizer::iterator end)
	{
		for (char c: str) {
			if (begin == end) {
				return false;
			}
			if (c != *begin++) {
				return false;
			}
		}
		return true;
	}

	// Match string case-insensitive
	static bool matchStringCI(const std::string & str, Tokenizer::iterator begin, Tokenizer::iterator end)
	{
		for (char c: str) {
			if (begin == end) {
				return false;
			}
			char d = *begin++;
			if (tolower(c) != tolower(d)) {
				return false;
			}
		}
		return true;
	}
	
	// Returns true if provided string contains at least one non-alpha character.
	// Function is optimized for BASIC keywords (e.g. validates only first and last char)
	static bool findSpecialChar(const std::string & str)
	{
		if (str.begin() != str.end()) {
			bool first_special = !isalpha(*str.begin());
			bool last_special  = !isalpha(*(str.end() - 1));
			return first_special || last_special;
		}
		return false;
	}
	
	
	// MARK: - Class implementation
	
	const byte Keywords::Code_BIN = 0xC4;
	const byte Keywords::Code_REM = 0xEA;
	const byte Keywords::Code_NUM = 0x0E;
	const byte Keywords::Code_ENT = 0x0D;
	
	Keywords::Keywords(Dialect dialect) :
		_dialect(dialect)
	{
		setupStructures(dialect);
	}
	
	void Keywords::setDialect(Dialect dialect) {
		if (dialect != _dialect) {
			setupStructures(dialect);
			_dialect = dialect;
		}
	}
	
	Keywords::Dialect Keywords::dialect() const {
		return _dialect;
	}
	
	byte Keywords::findKeyword(Tokenizer::iterator begin, Tokenizer::iterator end, size_t & out_matched_size) const
	{
		if (begin != end) {
			// Look if first char is in keywords
			auto max_distance = std::distance(begin, end);
			if (_keywordsFirstChars.find(tolower(*begin)) != std::string::npos) {
				for (auto && kw: _keywords) {
					if (matchStringCI(kw.keyword, begin, end)) {
						size_t matched_size = kw.keyword.size();
						// If a whole special keyword is matched, then we're pretty sure
						// that a whole keyword is matched.
						// If there's no special character in the keyword, then we have to
						// look behind the matched sequence. If there's alphanumeric or underscore,
						// then this is not a keyword and we have to continue with search.
						if (!kw.special && max_distance > matched_size) {
							const char c = *(begin + matched_size);
							if (isalnum(c) || c == '_') {
								continue;
							}
						}
						// Matched, return the byte representation & matched size.
						out_matched_size = matched_size;
						return kw.code;
					}
				}
			}
		}
		return 0;
	}
	
	byte Keywords::findEscapeCode(Tokenizer::iterator begin, Tokenizer::iterator end, size_t & out_matched_size) const
	{
		if (begin != end) {
			for (auto && sc: _escapeCodes) {
				if (matchString(sc.sequence, begin, end)) {
					out_matched_size = sc.sequence.size();
					return sc.code;
				}
			}
		}
		return 0;
	}
	
	
	
	// MARK: - Keywords
	
	//
	// 48k BASIC keywords.
	//
	// The table also contains alternate syntax, so
	// you can type "goto" instead of "go to"
	//
	
	static const char * s_keywordsTable[] =
	{
		"rnd"       ,
		"inkey$"    ,
		"pi"        ,
		"fn"        ,
		"point"     ,
		"screen$"   ,
		"attr"      ,
		"at"        ,
		"tab"       ,
		"val$"      ,
		"code"      ,
		"val"       ,
		"len"       ,
		"sin"       ,
		"cos"       ,
		"tan"       ,
		"asn"       ,
		"acs"       ,
		"atn"       ,
		"ln"        ,
		"exp"       ,
		"int"       ,
		"sqr"       ,
		"sgn"       ,
		"abs"       ,
		"peek"      ,
		"in"        ,
		"usr"       ,
		"str$"      ,
		"chr$"      ,
		"not"       ,
		"bin"       ,
		"or"        ,
		"and"       ,
		"<="        ,
		">="        ,
		"<>"        ,
		"line"      ,
		"then"      ,
		"to"        ,
		"step"      ,
		"deffn"     ,
		"cat"       ,
		"format"    ,
		"move"      ,
		"erase"     ,
		"open#"     ,
		"close#"    ,
		"merge"     ,
		"verify"    ,
		"beep"      ,
		"circle"    ,
		"ink"       ,
		"paper"     ,
		"flash"     ,
		"bright"    ,
		"inverse"   ,
		"over"      ,
		"out"       ,
		"lprint"    ,
		"llist"     ,
		"stop"      ,
		"read"      ,
		"data"      ,
		"restore"   ,
		"new"       ,
		"border"    ,
		"continue"  ,
		"dim"       ,
		"rem"       ,
		"for"       ,
		"goto"      ,
		"gosub"     ,
		"input"     ,
		"load"      ,
		"list"  	,
		"let"       ,
		"pause"     ,
		"next"      ,
		"poke"      ,
		"print"     ,
		"plot"      ,
		"run"       ,
		"save"      ,
		"randomize" ,
		"if"        ,
		"cls"       ,
		"draw"      ,
		"clear"     ,
		"return"    ,
		"copy"      ,
		// End of table
		nullptr
	};

	void Keywords::setupStructures(Dialect dialect)
	{
		// Prepare basic structures
		_keywords = prepareKeywords(dialect);
		_escapeCodes = prepareEscapeCodes(dialect);
		
		// Create a set of beginning characters from all keywords.
		std::set<char> chars;
		for (auto && keyword: _keywords) {
			chars.insert(*keyword.keyword.begin());
		}
		_keywordsFirstChars.clear();
		_keywordsFirstChars.reserve(chars.size());
		for (char c: chars) {
			_keywordsFirstChars.push_back(c);
		}
	}

	std::vector<Keywords::Keyword> Keywords::prepareKeywords(Dialect dialect)
	{
		byte code = 0xA5;	// first code - RND
		
		std::vector<Keyword> table;
		table.reserve(0x100 - code);
		
		const char ** p = s_keywordsTable;
		while (true) {
			const char * kw = *p++;
			if (!kw) {
				break;
			}
			auto kws = std::string(kw);
			auto special = findSpecialChar(kws);
			table.push_back(Keyword { kws, special, code++ });
		}
		if (dialect == Dialect_128K) {
			table.push_back(Keyword { "spectrum", false, 0xA3 });
			table.push_back(Keyword { "play", false, 0xA4 });
		}
		// We have to sort keywords table from longer to shorter, to prevent
		// the shorter keyword be matched as substring to longer ones.
		// For example, "IN" in "INK"
		std::sort(table.begin(), table.end(), LongerKeywordPredicate());
		return table;
	}
	
	// MARK: - Escape codes
	
	//
	// Following table contains string escape codes,
	// compatible with `zmakebas` program.
	//
	
	static const char * s_EscapeChars[] =
	{
		// Block graphic
		"  ", " '", "' ", "''", " .", " :", "'.", "':",
		". ", ".'", ": ", ":'", "..", ".:", ":.", "::",
		// UDG
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
		"l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
		// End of table
		nullptr
	};
	
	std::vector<Keywords::EscapeCode> Keywords::prepareEscapeCodes(Dialect dialect)
	{
		byte code = 0x80;
		
		std::vector<EscapeCode> table;
		table.reserve(0xA5 - code);
		
		const char ** p = s_EscapeChars;
		while (true) {
			const char * sequence = *p++;
			if (!sequence) {
				break;
			}
			table.push_back(EscapeCode { sequence, code++ });
		}
		table.push_back(EscapeCode {"*", 0x7F});		// copyright sign
		table.push_back(EscapeCode {"`", 0x60});		// pound sign
		table.push_back(EscapeCode {"\\", '\\'});		// backslash
		table.push_back(EscapeCode {"@", '@'});			// @
		return table;
	}
	

} // bastapir::bas
} // bastapir
