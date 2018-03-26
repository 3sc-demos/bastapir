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

#include <bastapir/tap/FileEntry.h>

namespace bastapir
{
namespace tap
{
	FileEntry::FileEntry(const std::string & name, Type type, const ByteRange & bytes) :
		_fileName(name),
		_fileType(type),
		_fileParams({0, 0}),
		_bytes(bytes)
	{
	}
	
	const std::string & FileEntry::name() const {
		return _fileName;
	}
	
	const FileEntry::Type FileEntry::type() const {
		return _fileType;
	}
	
	const ByteRange FileEntry::bytes() const {
		return _bytes.byteRange();
	}
	
	
	// MARK: - Additional properties
	
	const FileEntry::Params & FileEntry::params() const {
		return _fileParams;
	}
	
	FileEntry::Params & FileEntry::params() {
		return _fileParams;
	}
	
	void FileEntry::setParams(const Params params) {
		_fileParams = params;
	}
	
	void FileEntry::setSourceFileInfo(const SourceFileInfo & info) {
		_sourceFile = info;
	}
	
	const SourceFileInfo & FileEntry::sourceFileInfo() const {
		return _sourceFile;
	}
	
	
	// MARK: - Validation
	
	bool FileEntry::validate(std::vector<ValidationResult> & issues) const
	{
		issues.clear();
		
		if (name().size() > 10) {
			issues.push_back(WARN_NameTooLong);
		}
		if (bytes().size() > 65536) {
			issues.push_back(ERR_TooManyBytes);
		}
		switch (type()) {
			case Program:
			{
				auto program = params().program;
				if (program.autostartLine == 0 || program.autostartLine > 9999) {
					if (program.autostartLine != Params::NO_AUTOSTART) {
						issues.push_back(ERR_BasicWrongAutostart);
					}
				}
				if (program.variableArea > bytes().size()) {
					issues.push_back(ERR_BasicWrongVariableArea);
				}
				if (bytes().size() > 40000) {
					issues.push_back(ERR_BasicTooBig);	// 40k is my rough estimation
				}
				break;
			}
			case Code:
			{
				auto code = params().code;
				if (code.address < 16384 || (bytes().size() + code.address) > 65536) {
					issues.push_back(WARN_CodeInROM);
				}
				if (code.constValue != Params::NO_AUTOSTART) {
					issues.push_back(ERR_CodeHeader);
				}
				break;
			}
			default:
				break;
		}
		if (bytes().size() > 48*1024) {
			issues.push_back(WARN_TooManyBytes);
		}
		
		return issues.empty();
	}
	
} // bastapir::tap
} // bastapir
