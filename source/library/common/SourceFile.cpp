/**
 * Copyright 2018 Lime - HighTech Solutions s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <bastapir/common/SourceFile.h>
#include <stdio.h>
#include <errno.h>

namespace bastapir
{
	// MARK: - Generic source file -
	
	SourceFile::SourceFile(const SourceFileInfo & info) :
		_info(info),
		_is_valid(false)
	{
	}
	
	SourceFile::~SourceFile()
	{
	}
	
	SourceFile SourceFile::open(const std::string & path, SourceFileInfo::Mode mode)
	{
		if (mode == SourceFileInfo::Text) {
			return SourceTextFile(path);
		}
		return SourceBinaryFile(path);
	}
	
	// MARK: Properties
	
	const SourceFileInfo & SourceFile::info() const
	{
		return _info;
	}
	
	const std::string & SourceFile::error() const
	{
		return _error;
	}
	
	bool SourceFile::isValid() const
	{
		return _is_valid && _error.empty();
	}
	
	
	// MARK: Private
	
	void SourceFile::open(std::string * out_str, ByteArray * out_bin)
	{
		const bool is_text = _info.mode == SourceFileInfo::Text;
		
		if (is_text) assert(out_str != nullptr && out_bin == nullptr);
		if (!is_text) assert(out_str == nullptr && out_bin != nullptr);
		
		_is_valid = false;
		_error.clear();

		// Open file
		FILE * f = fopen(_info.path.c_str(), is_text ? "r" : "rb");
		if (!f) {
			_error = "Unable to open file (" + std::string(strerror(errno)) + ")";
			return;
		}
		// Determine length
		long length = fseek(f, 0, SEEK_END);
		if (length < 0) {
			_error = "Unable to determine length of the file (" + std::string(strerror(errno)) + ")";
			return;
		}
		if (fseek(f, 0, SEEK_SET) < 0) {
			_error = "Cannot rewind file (" + std::string(strerror(errno)) + ")";
			return;
		}
		
		// Allocate buffers
		if (_info.mode == SourceFileInfo::Text) {
			out_str->clear();
			out_str->reserve(length);
		} else {
			out_bin->clear();
			out_bin->reserve(length);
		}
		ByteArray buffer;
		buffer.resize(std::min((long)65536, length));
		
		// Read data
		
		bool result = true;
		while (!feof(f)) {
			size_t rc = fread(buffer.data(), buffer.size(), 1, f);
			if (rc == 0) {
				_error = "Cannot read from file (" + std::string(strerror(errno)) + ")";
				result = false;
				break;
			}
			if (is_text) {
				out_str->append((const char*)buffer.data(), rc);
			} else {
				out_bin->append(buffer.data(), rc);
			}
		}
		fclose(f);
		_is_valid = result;
	}
	
	
	// MARK: - Text file -
	
	SourceTextFile::SourceTextFile(const std::string & path) :
		SourceFile(SourceFileInfo { path, SourceFileInfo::Text })
	{
		open(&_content, nullptr);
	}
	
	SourceTextFile::~SourceTextFile()
	{
	}
	
	const std::string & SourceTextFile::string() const
	{
		return _content;
	}
	
	
	// MARK: - Binary file -
	
	SourceBinaryFile::SourceBinaryFile(const std::string & path) :
		SourceFile(SourceFileInfo { path, SourceFileInfo::Binary })
	{
		open(nullptr, &_content);
	}
	
	SourceBinaryFile::~SourceBinaryFile()
	{
	}
	
	const ByteArray & SourceBinaryFile::bytes() const
	{
		return _content;
	}
	
} // bastapir

