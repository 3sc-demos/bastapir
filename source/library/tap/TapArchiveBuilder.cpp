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

#include <bastapir/tap/TapArchiveBuilder.h>

namespace bastapir
{
namespace tap
{
	// MARK: - Public methods
	
	TapArchiveBuilder::TapArchiveBuilder(ErrorLogging * log) :
		_logging(log)
	{
		assert(_logging != nullptr);
	}

	void TapArchiveBuilder::setSourceFileInfo(const SourceFileInfo & info) {
		_sourceFileInfo = info;
	}
	
	const SourceFileInfo & TapArchiveBuilder::sourceFileInfo() const {
		return _sourceFileInfo;
	}
	
	void TapArchiveBuilder::addFile(const FileEntry & entry) {
		_files.push_back(entry);
	}

	
	ByteArray TapArchiveBuilder::build() const
	{
		ByteArray out;
		for (auto && file: _files) {
			auto validationResult = file.validate();
			if (validationResult != FileEntry::OK) {
				reportError(file, validationResult);
				if (validationResult > FileEntry::ERR) {
					// TODO: throw exception...
					return ByteArray();
				}
			}
			auto headerBytes = serializeHeader(file);
			out.append(serializeTapeStream(headerBytes, true, true));
			out.append(serializeTapeStream(file.bytes(), false, true));
		}
		return out;
	}
	
	// MARK: - Low level methods
	
	ByteArray TapArchiveBuilder::serializeHeader(const FileEntry & file)
	{
		ByteArray header;
		header.reserve(17);
		
		// Crop name to 10 chars, or extend with blanks if name is shorter.
		ByteArray name(MakeRange(file.name()));
		name.resize(10, ' ');
		
		U16 length = file.bytes().size() & 0xFFFF;
		
		header.append((byte)file.type());						// 0.  type
		header.append(name);									// 1.  name                  (10 bytes)
		header.append(MakeRange(length));						// 11. length of data block  (2 bytes)
		header.append(MakeRange(file.params().generic.param1));	// 13. param1                (2 bytes)
		header.append(MakeRange(file.params().generic.param2)); // 15. param2                (2 bytes)
		
		assert(header.size() == 17);
		
		return header;
	}
	
	
	ByteArray TapArchiveBuilder::serializeTapeStream(const ByteRange & bytes, bool is_header, bool is_tap_block)
	{
		if (is_header && bytes.size() != 17) {
			assert(false);	// header must be 17 bytes long
			return ByteArray();
		}
		const size_t raw_size = bytes.size() + 2;
		if (raw_size > 65536) {
			assert(false);	// too many bytes
			return ByteArray();
		}

		ByteArray out;
		out.reserve(bytes.size() + 2 + 2);
		
		// Calculate checksum
		const byte leading_byte = is_header ? 0x00 : 0xFF;
		byte checksum = leading_byte;
		for (byte b: bytes) {
			checksum ^= b;
		}
		
		// Construct final tape stream
		if (is_tap_block) {
			U16 block_size = raw_size;
			out.append(MakeRange(block_size));
		}
		out.push_back(leading_byte);
		out.append(bytes);
		out.append(checksum);

		return out;
	}
	
	// MARK: - Private
	
	void TapArchiveBuilder::reportError(const FileEntry & entry, FileEntry::ValidationResult result) const
	{
		ErrorInfo ei;
		if (entry.sourceFileInfo().hasInfo()) {
			ei.sourceFile = entry.sourceFileInfo().path;
		} else {
			ei.sourceFile = _sourceFileInfo.path;
		}
		auto ename = entry.name();
		auto esize = std::to_string(entry.bytes().size());
		
		switch (result)
		{
			// Warnings
			case FileEntry::WARN_NameTooLong:
				_logging->warning(ei, "TAP file's name is too long: `" + ename + "`"); break;
			case FileEntry::WARN_TooManyBytes:
				_logging->warning(ei, "TAP file's size is longer than 48kB. File: `" + ename + "`, Size: " + esize); break;
			case FileEntry::WARN_CodeInROM:
				_logging->warning(ei, "TAP code block will be loaded to ROM. File: `" + ename + "`"); break;
				
			// Errors
			case FileEntry::ERR_TooManyBytes:
				_logging->error(ei, "TAP file's size is too long. File: `" + ename + "`, Size: " + esize); break;
			case FileEntry::ERR_BasicTooBig:
				_logging->error(ei, "BASIC file is too big. File: `" + ename + "`, Size: " + esize); break;
			case FileEntry::ERR_BasicWrongAutostart:
				_logging->error(ei, "Autostart for BASIC program is wrong. File: `" + ename + "`, Line: " + std::to_string(entry.params().program.autostartLine)); break;
			case FileEntry::ERR_BasicWrongVariableArea:
				_logging->error(ei, "Offset for BASIC variables area is wrong. File: `" + ename +"`"); break;
			case FileEntry::ERR_CodeHeader:
				_logging->error(ei, "Header for TAP code block is wrong. File: `" + ename +"`"); break;
				
			default: break;
		}
	}
	
} // bastapir::tap
} // bastapir

