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

#include <bastapir/types/Tokenizer.h>
#include <map>
#include "BasicKeywords.h"

namespace bastapir
{
namespace bas
{
	
	struct Token
	{
		enum Type
		{
			Comment,
			LineNumber,
			Keyword,
			Number,
			String,
		};
	};
	
	struct Variable
	{
		
	};
	
	struct Line
	{
		
	};
	
	class BasicTextParser
	{
	public:
		BasicTextParser();
		
		

	private:
		
		std::string _source_text;
		Tokenizer _tokenizer;
		
		std::map<std::string, Variable> _constants;
		std::map<std::string, Variable> _variables;
		BasicKeywords _keywords;
	};
	
} // bastapir::bas
} // bastapir
