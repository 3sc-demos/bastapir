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

#include <bastapir/tap/TapArchiveBuilder.h>
#include <bastapir/bas/BasicTextParser.h>
#include <bastapir/common/SourceFile.h>

namespace bastapir
{
	class BastapirDocument {
	public:
		
		BastapirDocument(ErrorLogging * log);
		
		bool processDocument(const SourceTextFile & file);
		const ByteRange archiveBytes() const;
		
	private:
		
		bool doParseDocument();
		bool doParseLine();
		
		bool doParseCmdProgram();
		bool doParseCmdCode();
		
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
		
		std::string 	captureWord(bool lowercase);
		bool		 	captureNumber(long & value);
		bool		 	captureString(std::string & captured);
		
		// Members
		
		ErrorLogging * _log;
		tap::TapArchiveBuilder _tapBuilder;
		SourceFileInfo _sourceFileInfo;
		
		Tokenizer _tokenizer;
		ByteArray _archiveBytes;
	};
}
