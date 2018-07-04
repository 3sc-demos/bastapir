/*
 * Copyright 2018 Juraj Durech <durech.juraj@gmail.com>
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

#pragma once

#include <bastapir/common/ByteArray.h>
#include <bastapir/common/ErrorInfo.h>
#include <bastapir/common/Path.h>

namespace bastapir
{
	struct SourceFileInfo
	{
		/// Type of source file: Textual or Binary
		enum Mode
		{
			Text,
			Binary
		};
		
		/// Path to the file
		std::string path;
		/// Opening mode
		Mode mode;
		
		/// Returns true if info structure has defined path.
		bool hasInfo() const {
			return !path.empty();
		}
	};

	inline ErrorInfo MakeError(const SourceFileInfo & info, size_t line = 0, size_t column = 0)
	{
		ErrorInfo ei;
		ei.sourceFile = info.path;
		ei.line = line;
		ei.column = column;
		return ei;
	}
	
	
	// MARK: - Generic source file
	
	class SourceFile
	{
	public:
		/// Opens a source file defined in |info| structure. Method always
		/// returns object, so you need to investigate whetner is valid afterwards.
		static SourceFile open(const Path & path, SourceFileInfo::Mode mode);
		
		virtual ~SourceFile();
		
		/// Returns information about file.
		const SourceFileInfo & info() const;
		
		/// Returns reference to error string. The value is updated when IO operation in
		/// the underlying implementation is
		const std::string &	error() const;
		
		/// Returns true if file has valid content.
		bool isValid() const;
		
	protected:
		
		/// Direct construction is not allowed.
		SourceFile(const SourceFileInfo & info);
		
		/// Internal open function.
		void open(std::string * out_str, ByteArray * out_bin);
		
	private:
		
		const SourceFileInfo _info;
		bool _is_valid;
		std::string _error;
	};

	
	
	// MARK: - Text file
	
	class SourceTextFile: public SourceFile
	{
	public:
		SourceTextFile(const Path & path);
		~SourceTextFile();
		
		/// Returns reference to string representation of file content.
		const std::string & string() const;

	private:
		std::string _content;
	};
	
	
	
	// MARK: - Binary file
	
	class SourceBinaryFile: public SourceFile
	{
	public:
		SourceBinaryFile(const Path & path);
		~SourceBinaryFile();
		
		/// Returns bytes representation of file content.
		const ByteArray & bytes() const;
		
	private:
		ByteArray _content;
	};
	
	
}
