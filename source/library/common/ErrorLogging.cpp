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

#include <bastapir/common/ErrorLogging.h>

namespace bastapir
{
	// MARK: - File looger
	
	FileErrorLogger::FileErrorLogger(FILE * std_out, FILE * err_out, bool close_streams) :
		_out(std_out),
		_err(err_out),
		_close_streams(close_streams)
	{
	}
	
	FileErrorLogger::~FileErrorLogger()
	{
		if (_close_streams) {
			fclose(_out);
			if (_out != _err) {
				fclose(_err);
			}
		}
	}
	
	void FileErrorLogger::setLogPrefix(const std::string & prefix)
	{
		_prefix = prefix;
	}
	
	const std::string & FileErrorLogger::prefix() const
	{
		return _prefix;
	}
	
	void FileErrorLogger::setBasePathForSourceFiles(const std::string & base_path)
	{
		_base_path = base_path;
	}
	
	const std::string & FileErrorLogger::basePathForSourceFiles() const
	{
		return _base_path;
	}
	
	// MARK: - ErrorLogging interface
	
	void FileErrorLogger::error(const std::string & message)
	{
		dump(_err, SevError, ErrorInfo(), message);
	}
	
	void FileErrorLogger::error(const ErrorInfo & info, const std::string & message)
	{
		dump(_err, SevError, info, message);
	}
	
	void FileErrorLogger::warning(const std::string & message)
	{
		dump(_err, SevWarning, ErrorInfo(), message);
	}
	
	void FileErrorLogger::warning(const ErrorInfo & info, const std::string & message)
	{
		dump(_err, SevWarning, info, message);
	}
	
	void FileErrorLogger::info(const std::string & message)
	{
		dump(_out, SevInfo, ErrorInfo(), message);
	}
	
	void FileErrorLogger::info(const ErrorInfo & info, const std::string & message)
	{
		dump(_out, SevInfo, info, message);
	}
	
	// MARK: - Private
	
	void FileErrorLogger::dump(FILE *stream, Severity severity, const ErrorInfo & ei, const std::string & message)
	{
		std::string out = _prefix;
		
		if (ei.hasInfo()) {
			out += ei.sourceFile;
			if (ei.line > 0) {
				out += ":";
				out += ei.line;
				if (ei.column > 0) {
					out += ":";
					out += ei.column;
				}
			}
			out += ": ";
		}
		
		switch (severity) {
			case SevError: out += "error: ";
				break;
			case SevWarning: out += "warning: ";
				break;
			case SevInfo:
				break;
		}
		out += message;
		fputs(out.c_str(), stream);
	}

} // bastapir
