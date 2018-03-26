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

#include <bastapir/common/SourceFile.h>
#include <bastapir/common/ErrorLogging.h>

namespace bastapir
{
namespace tap
{
	class FileEntry
	{
	public:

		/// Type of file stored on tape.
		enum Type
		{
			/// BASIC program.
			Program = 0,
			/// Number array
			Numbers,
			/// Character array
			Characters,
			/// Sequence of bytes
			Code
		};
		
		/// Parameters for file header. There are always two U16 values
		/// serialized in the header. The Params structure is implemented as
		/// union with more convenient sub-types, declared for each important
		/// file type.
		struct Params
		{
			static const U16 NO_AUTOSTART = 32768;
		
			/// Generic struct, used for header serialization
			struct _generic
			{
				U16 param1;
				U16 param2;
			};
			/// Params for BASIC program.
			struct _program
			{
				/// Autostart line number for BASIC program.
				/// If number is >= 32768, proram will not automatically start after load
				U16 autostartLine;
				/// Relative offset to variables area. In our case it points to the end
				/// of BASIC program (e.g. is equal to length of BASIC program in bytes)
				U16 variableArea;
			};
			/// Params for bytes sequence.
			struct _code
			{
				/// Starting address for code block.
				U16 address;
				/// Constant, be always 32768
				U16 constValue;
			};

			union {
				_generic	generic;
				_program	program;
				_code	 	code;
			};
		};
		
		FileEntry(const std::string & name, Type type, const ByteRange & bytes);
		
		/// Returns name of file stored in this entry.
		const std::string & name() const;
		
		/// Returns type of file stored in this entry.
		const Type type() const;
		
		/// Returns reference to stored bytes.
		const ByteRange bytes() const;
		
		
		// MARK: - Additional properties
		
		/// Returns const reference to params object.
		const Params & params() const;
		
		/// Returns non-const reference to params object.
		Params & params();
		
		/// Changes whole Params structure at once.
		void setParams(const Params params);
		
		/// Sets associated source file
		void setSourceFileInfo(const SourceFileInfo & info);
		
		/// Returns associated source file
		const SourceFileInfo & sourceFileInfo() const;
		
		// MARK: - Validation
		
		/// Result of file entry header validation.
		enum ValidationResult
		{
			/// Success
			OK,
			
			/// File Name is too long.
			WARN_NameTooLong,
			/// File contains more than 48k bytes. This is warning, TAP still can
			/// be created, but it may cause problems during file loading
			WARN_TooManyBytes,
			/// Starting address, or start + size points to ROM
			WARN_CodeInROM,
			
			/// Everything above this value is error.
			ERR,
			
			/// File contains too many bytes (more than 64k)
			ERR_TooManyBytes,
			/// BASIC program is too big.
			ERR_BasicTooBig,
			/// Line for BASIC autostart is wrong (0 or more than 9999)
			ERR_BasicWrongAutostart,
			/// Variable area is out of range
			ERR_BasicWrongVariableArea,
			/// Problem with header for CODE (param2 != 32768)
			ERR_CodeHeader,
		};
		
		/// Validates file entry object and returns true if header is valid. If false is returned, then
		/// you need to iterate over |issues| vector and decide whether warnings or errors are critical
		/// for your purposes.
		bool validate(std::vector<ValidationResult> & issues) const;
		
	private:
		
		std::string		_fileName;
		Type	 		_fileType;
		Params			_fileParams;
		
		ByteArray		_bytes;
		
		SourceFileInfo	_sourceFile;
	};
	
} // bastapir::tap
} // bastapir
