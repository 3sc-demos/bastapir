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
	
	
	// MARK: - Class implementation
	
	const byte Keywords::Code_BIN = 0xC4;
	const byte Keywords::Code_REM = 0xEA;
	const byte Keywords::Code_NUM = 0x0E;
	const byte Keywords::Code_ENT = 0x0D;
	
	Keywords::Keywords(Variant v) :
		_keywords(prepareKeywords(v)),
		_keywordsFirstChars(prepareKeywordsFirstChars(_keywords)),
		_escapeCodes(prepareEscapeCodes(v)),
		_variant(v)
	{
	}
	
	void Keywords::setVariant(Variant variant) {
		if (variant != _variant) {
			_keywords = prepareKeywords(variant);
			_keywordsFirstChars = prepareKeywordsFirstChars(_keywords);
			_escapeCodes = prepareEscapeCodes(variant);
			_variant = variant;
		}
	}
	
	Keywords::Variant Keywords::variant() const {
		return _variant;
	}
	
	byte Keywords::findKeyword(Tokenizer::iterator begin, Tokenizer::iterator end, size_t & out_matched_size) const
	{
		if (begin != end) {
			// Look if first char is in keywords 
			if (_keywordsFirstChars.find(tolower(*begin)) != std::string::npos) {
				for (auto && sc: _keywords) {
					if (matchStringCI(sc.primary, begin, end)) {
						out_matched_size = sc.primary.size();
						return sc.code;
					}
					if (!sc.alternate.empty() && matchStringCI(sc.alternate, begin, end)) {
						out_matched_size = sc.alternate.size();
						return sc.code;
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
		"rnd"       , "",
		"inkey$"    , "",
		"pi"        , "",
		"fn"        , "",
		"point"     , "",
		"screen$"   , "",
		"attr"      , "",
		"at"        , "",
		"tab"       , "",
		"val$"      , "",
		"code"      , "",
		"val"       , "",
		"len"       , "",
		"sin"       , "",
		"cos"       , "",
		"tan"       , "",
		"asn"       , "",
		"acs"       , "",
		"atn"       , "",
		"ln"        , "",
		"exp"       , "",
		"int"       , "",
		"sqr"       , "",
		"sgn"       , "",
		"abs"       , "",
		"peek"      , "",
		"in"        , "",
		"usr"       , "",
		"str$"      , "",
		"chr$"      , "",
		"not"       , "",
		"bin"       , "",
		"or"        , "",
		"and"       , "",
		"<="        , "",
		">="        , "",
		"<>"        , "",
		"line"      , "",
		"then"      , "",
		"to"        , "",
		"step"      , "",
		"def fn"    , "deffn",
		"cat"       , "",
		"format"    , "",
		"move"      , "",
		"erase"     , "",
		"open #"    , "open#",
		"close #"   , "close#",
		"merge"     , "",
		"verify"    , "",
		"beep"      , "",
		"circle"    , "",
		"ink"       , "",
		"paper"     , "",
		"flash"     , "",
		"bright"    , "",
		"inverse"   , "",
		"over"      , "",
		"out"       , "",
		"lprint"    , "",
		"llist"     , "",
		"stop"      , "",
		"read"      , "",
		"data"      , "",
		"restore"   , "",
		"new"       , "",
		"border"    , "",
		"continue"  , "",
		"dim"       , "",
		"rem"       , "",
		"for"       , "",
		"go to"     , "goto",
		"go sub"    , "gosub",
		"input"     , "",
		"load"      , "",
		"list"  	, "",
		"let"       , "",
		"pause"     , "",
		"next"      , "",
		"poke"      , "",
		"print"     , "",
		"plot"      , "",
		"run"       , "",
		"save"      , "",
		"randomize" , "randomise",
		"if"        , "",
		"cls"       , "",
		"draw"      , "",
		"clear"     , "",
		"return"    , "",
		"copy"      , "",
		// End of table
		nullptr     , nullptr
	};
	
	std::vector<Keywords::Keyword> Keywords::prepareKeywords(Variant v)
	{
		byte code = 0xA5;	// first code - RND
		
		std::vector<Keyword> table;
		table.reserve(0x100 - code);
		
		const char ** p = s_keywordsTable;
		while (true) {
			const char * primary = *p++;
			const char * secondary = *p++;
			if (!primary || !secondary) {
				break;
			}
			table.push_back(Keyword { primary, secondary, code++ });
		}
		if (v == Variant_128K) {
			table.push_back(Keyword { "spectrum", "", 0xA3 });
			table.push_back(Keyword { "play", "", 0xA4 });
		}
		return table;
	}
	
	std::string Keywords::prepareKeywordsFirstChars(const std::vector<Keyword> & keywords)
	{
		std::set<char> chars;
		for (auto && keyword: keywords) {
			chars.insert(*keyword.primary.begin());
		}
		std::string result;
		result.reserve(chars.size());
		for (char c: chars) {
			result.push_back(c);
		}
		return result;
	}
	
	// MARK: - Escape codes
	
	//
	// Following table contains string escape codes,
	// compatible with `zmakebas` program.
	//
	
	static const char * s_EscapeChars[] =
	{
		// block graphic
		"  ", " '", "' ", "''", " .", " :", "'.", "':",
		". ", ".'", ": ", ":'", "..", ".:", ":.", "::",
		// udg
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
		"l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
		// End of table
		nullptr
	};
	
	std::vector<Keywords::EscapeCode> Keywords::prepareEscapeCodes(Variant v)
	{
		byte code = 0x80;
		
		std::vector<EscapeCode> table;
		table.reserve(0x100 - code);
		
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
