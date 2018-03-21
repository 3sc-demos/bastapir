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

#include <bastap/ByteArray.h>

namespace bastap
{
namespace tap
{
	class FileEntry
	{
	public:

		enum Type
		{
			Program = 0,
			Numbers,
			Characters,
			Code
		};
		
		struct Params
		{
			struct _generic
			{
				U16 param1;
				U16 param2;
			};
			
			struct _program
			{
				U16 startLine;
				U16 variableArea;
			};
			
			struct _code
			{
				U16 address;
				U16 const32768;
			};

			union {
				_generic	generic;
				_program	program;
				_code	 	code;
			};
		};
		
		FileEntry(const std::string & name, Type type, const ByteRange & bytes) :
			_fileName(name),
			_fileType(type),
			_fileParams({0, 0}),
			_bytes(bytes)
		{
		}
		
		const std::string & name() const {
			return _fileName;
		}
		
		const Type type() const {
			return _fileType;
		}
		
		const ByteRange bytes() const {
			return _bytes.byteRange();
		}
		
		const Params & params() const {
			return _fileParams;
		}
		
		Params & params() {
			return _fileParams;
		}
		
		void setParams(const Params params) {
			_fileParams = params;
		}
		
	private:
		
		std::string		_fileName;
		Type	 		_fileType;
		Params			_fileParams;
		
		ByteArray		_bytes;
	};
	
} // bastap::tap
} // bastap
