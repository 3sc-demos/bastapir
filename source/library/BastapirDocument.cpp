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

#include <bastapir/BastapirDocument.h>

namespace bastapir
{
	// MARK: - Class implementation
	
	BastapirDocument::BastapirDocument(ErrorLogging * log) :
		_log(log),
		_tapBuilder(log)
	{
		assert(_log != nullptr);
	}
	
	bool BastapirDocument::processDocument(const SourceTextFile & file)
	{
		_archiveBytes.clear();
		if (!file.isValid()) {
			return false;
		}
		// Reset tokenizer to new content
		_tokenizer.setStopAtLineEnd(true);
		_tokenizer.resetTo(file.string().begin(), file.string().end());
		_sourceFileInfo = file.info();
		_tapBuilder.setSourceFileInfo(file.info());
		
		bool result = doParseDocument();
		if (result) {
			_archiveBytes = _tapBuilder.build();
			result = !_archiveBytes.empty();
		}
		return result;
	}
	
	const ByteRange BastapirDocument::archiveBytes() const
	{
		return _archiveBytes.byteRange();
	}
	
	
	// MARK: - Parser
	
	bool BastapirDocument::doParseDocument()
	{
		while (true) {
			if (!doParseLine()) {
				return false;
			}
			if (!_tokenizer.nextLine()) {
				break;
			}
		}
		return true;
	}
	
	bool BastapirDocument::doParseLine()
	{
		_tokenizer._debugInfo();
		_tokenizer.skipWhitespace();
		char c = _tokenizer.charAt();
		if (c == 0) {
			// end of line
			return true;
		}
		if (c == '#') {
			// comment, skip rest of the line
			return true;
		}
		auto command = captureWord(true);
		if (command.empty()) {
			return false;
		}
		if (command == "basic") {
			if (!doParseCmdProgram()) {
				return false;
			}
		} else if (command == "code") {
			if (!doParseCmdCode()) {
				return false;
			}
		} else {
			if (isalpha(_tokenizer.charAt())) {
				_log->error(errInfoLC(), "Unknown command `" + command + "`");
			} else {
				_log->error(errInfoLC(), "Unexpected character in document.");
			}
			return false;
		}
		return true;
	}
	
	bool BastapirDocument::doParseCmdProgram()
	{
		// basic "path/to/basic" [ProgramName]
		std::string path;
		if (!captureString(path)) {
			return false;
		}
		_tokenizer.skipWhitespace();
		std::string programName;
		if (!captureWordOrString(programName)) {
			return false;
		}
		if (programName.empty()) {
			// get name from file
			programName = Path::components(path).fileNameNoExt;
		}
		
		// Load & parse basic file
		SourceTextFile file = SourceTextFile(path);
		if (!file.isValid()) {
			_log->error(errInfoLC(), "Unable to open BASIC program file: " + path);
			return false;
		}
		bas::BasicTextParser parser(_log);
		if (!parser.parse(file.string(), file.info())) {
			return false;
		}
		printf("Crappy length %lu\n", parser.programBytes().size());
		std::string autostart_var;
		bool resolved;
		std::tie(resolved, autostart_var) = parser.resolveVariable("autostart");
		long autostart_line = tap::FileEntry::Params::NO_AUTOSTART;
		if (resolved) {
			autostart_line = std::stol(autostart_var);
		}
		
		auto entry = tap::FileEntry(programName, tap::FileEntry::Program, parser.programBytes());
		auto entry_params = tap::FileEntry::Params();
		entry_params.program.autostartLine = autostart_line;
		entry_params.program.variableArea = parser.programBytes().size();
		entry.setParams(entry_params);
		
		_tapBuilder.addFile(entry);
		
		return true;
	}
	
	bool BastapirDocument::doParseCmdCode()
	{
		// code "path/to/bytes" Address [BytesName]
		std::string path;
		if (!captureString(path)) {
			return false;
		}
		_tokenizer.skipWhitespace();
		long address;
		if (!captureNumber(address)) {
			return false;
		}
		_tokenizer.skipWhitespace();
		std::string codeName;
		if (!captureWordOrString(codeName)) {
			return false;
		}
		if (codeName.empty()) {
			// get name from file
			codeName = Path::components(path).fileNameNoExt;
		}
		
		SourceBinaryFile file = SourceBinaryFile(path);
		if (!file.isValid()) {
			_log->error(errInfoLC(), "Unable to open CODE bytes file: " + path);
			return false;
		}
		
		auto entry = tap::FileEntry(codeName, tap::FileEntry::Code, file.bytes());
		auto entry_params = tap::FileEntry::Params();
		entry_params.code.address = address;
		entry_params.code.constValue = 32768;
		entry.setParams(entry_params);
		
		_tapBuilder.addFile(entry);
		
		return true;
	}
	
	
	std::string BastapirDocument::captureWord(bool lowercase)
	{
		_tokenizer.resetCapture();
		_tokenizer.skipWhile(isalpha);
		auto word = _tokenizer.capture().content();
		if (lowercase) {
			std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		}
		return word;
	}
	
	
	bool BastapirDocument::captureNumber(long & value)
	{
		_tokenizer.skipWhitespace();
		
		if (_tokenizer.charAt() == '0') {
			if (_tokenizer.charAt(1) == 'x' || _tokenizer.charAt(1) == 'X') {
				// Hexadecimal number
				_tokenizer.movePosition(2);
				_tokenizer.resetCapture();
				_tokenizer.skipWhile([] (int c) -> int {
					return (c >= '0' && c <= '9') ||
							(c >= 'a' && c <= 'f') ||
							(c >= 'A' && c <= 'F');
				});
				value = std::stol(_tokenizer.capture().content(), nullptr, 16);
				return true;
			}
		}
		// Decimal number
		_tokenizer.resetCapture();
		_tokenizer.skipWhile(isnumber);
		auto word = _tokenizer.capture().content();
		value = std::stol(word);
		if (std::to_string(value) == word) {
			_log->error(errInfoLC(), "Decimal number is expected.");
			return false;
		}
		return true;
	}
	
	bool BastapirDocument::captureString(std::string & captured)
	{
		_tokenizer.skipWhitespace();
		
		char c = _tokenizer.getChar();
		if (c != '"') {
			_log->error(errInfoLC(), "A double quoted string is expected.");
			return false;
		}
		_tokenizer.resetCapture();
		bool found = _tokenizer.searchFor([] (int c)->int { return c == '"'; });
		if (!found) {
			_log->error(errInfoLC(), "End of double quoted string is expected.");
			return false;
		}
		// Go back for one character, we don't want to capture `"`
		_tokenizer.movePosition(-1);
		captured = _tokenizer.capture().content();
		_tokenizer.movePosition(1);
		return true;
	}
	
	bool BastapirDocument::captureWordOrString(std::string & captured)
	{
		char c = _tokenizer.charAt();
		if (c == '"') {
			return captureString(captured);
		}
		captured = captureWord(false);
		return true;
	}
	
} // bastapir
