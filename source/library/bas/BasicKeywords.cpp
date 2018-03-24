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

#include "BasicKeywords.h"

namespace bastapir
{
namespace bas
{
	// MARK: - Support functions

	static std::vector<Keyword> prepareKeywords();
	static std::vector<StrCode> prepareStringCodes();
	
	static bool matchString(const std::string & str, Tokenizer::iterator begin, Tokenizer::iterator end) {
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
	
	static bool matchStringCI(const std::string & str, Tokenizer::iterator begin, Tokenizer::iterator end) {
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
	
	BasicKeywords::BasicKeywords() :
		_keywords(prepareKeywords()),
		_stringCodes(prepareStringCodes())
	{
	}
	
	byte BasicKeywords::findKeyword(Tokenizer::iterator begin, Tokenizer::iterator end) const
	{
		for (auto && sc: _keywords) {
			if (matchStringCI(sc.primary, begin, end)) {
				return sc.code;
			}
			if (!sc.alternate.empty() && matchStringCI(sc.alternate, begin, end)) {
				return sc.code;
			}
		}
		return 0;
	}
	
	byte BasicKeywords::findEscapeCode(Tokenizer::iterator begin, Tokenizer::iterator end) const
	{
		for (auto && sc: _stringCodes) {
			if (matchString(sc.sequence, begin, end)) {
				return sc.code;
			}
		}
		return 0;
	}
	
	
	
	// MARK: - Keywords
	
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
	
	static std::vector<Keyword> prepareKeywords()
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
		return table;
	}
	
	
	// MARK: - Escape codes
	
	static const char * s_EscapeChars[] =
	{
		// block graphic
		"  ", " '", "' ", "''", " .", " :", "'.", "':",
		". ", ".'", ": ", ":'", "..", ".:", ":.", "::",
		// udg
		"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k",
		"l", "m", "n", "o", "p", "q", "r", "s", "t", "u",
		nullptr
	};
	
	static std::vector<StrCode> prepareStringCodes()
	{
		byte code = 0x80;
		
		std::vector<StrCode> table;
		table.reserve(0x100 - code);
		
		const char ** p = s_EscapeChars;
		while (true) {
			const char * sequence = *p++;
			if (!sequence) {
				break;
			}
			table.push_back(StrCode { sequence, code++ });
		}
		// copyright sign
		table.push_back(StrCode {"*", 0x7F});
		// pound sign
		table.push_back(StrCode {"`", 0x60});
		// backslash
		table.push_back(StrCode {"\\", '\\'});
		return table;
	}
	

} // bastapir::bas
} // bastapir
