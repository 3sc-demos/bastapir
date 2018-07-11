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

#include <bastapir/common/ErrorLogging.h>

namespace bastapir
{
	// MARK: - File looger -
	
	FileErrorLogger::FileErrorLogger(FILE * std_out, FILE * err_out, bool close_streams) :
		_out(std_out),
		_err(err_out),
		_min_severity(SevInfo),
		_close_streams(close_streams),
		_info({0,0})
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
	
	void FileErrorLogger::setLogPrefix(const std::string & prefix) {
		_prefix = prefix;
	}
	
	const std::string & FileErrorLogger::prefix() const {
		return _prefix;
	}
	
	void FileErrorLogger::setBasePathForSourceFiles(const std::string & base_path) {
		_base_path = base_path;
	}
	
	const std::string & FileErrorLogger::basePathForSourceFiles() const {
		return _base_path;
	}
	
	void FileErrorLogger::setMinimumDisplayedSeverity(Severity severity) {
		_min_severity = severity;
	}
	
	FileErrorLogger::Severity FileErrorLogger::minimumDisplayedSeverity() const {
		return _min_severity;
	}
	
	// MARK: - ErrorLogging interface
	
	void FileErrorLogger::error(const std::string & message) {
		_info.errorsCount++;
		dump(_err, SevError, ErrorInfo(), message);
	}
	void FileErrorLogger::error(const ErrorInfo & info, const std::string & message) {
		_info.errorsCount++;
		dump(_err, SevError, info, message);
	}
	void FileErrorLogger::warning(const std::string & message) {
		_info.warningsCount++;
		dump(_err, SevWarning, ErrorInfo(), message);
	}
	void FileErrorLogger::warning(const ErrorInfo & info, const std::string & message) {
		_info.warningsCount++;
		dump(_err, SevWarning, info, message);
	}
	void FileErrorLogger::info(const std::string & message) {
		dump(_out, SevInfo, ErrorInfo(), message);
	}
	void FileErrorLogger::info(const ErrorInfo & info, const std::string & message) {
		dump(_out, SevInfo, info, message);
	}
	void FileErrorLogger::debug(const std::string & message) {
		dump(_out, SevDebug, ErrorInfo(), message);
	}
	void FileErrorLogger::debug(const ErrorInfo & info, const std::string & message) {
		dump(_out, SevDebug, info, message);
	}
	ErrorLogging::Info FileErrorLogger::getInfo() const {
		return _info;
	}
	void FileErrorLogger::resetInfo() {
		_info = {0, 0};
	}
	
	
	// MARK: - Private
	
	void FileErrorLogger::dump(FILE *stream, Severity severity, const ErrorInfo & ei, const std::string & message)
	{
		if (severity > _min_severity) {
			return;
		}
		
		std::string out = _prefix;
		
		if (ei.hasInfo()) {
			out += ei.sourceFile;
			if (ei.line > 0) {
				out += ":";
				out += std::to_string(ei.line);
				if (ei.column > 0) {
					out += ":";
					out += std::to_string(ei.column);
				}
			}
			out += ": ";
		}
		
		switch (severity) {
			case SevError: out += "error: ";
				break;
			case SevWarning: out += "warning: ";
				break;
			case SevDebug: out += "debug: ";
				break;
			case SevInfo:
				break;
		}
		out += message;
		out += "\n";
		fputs(out.c_str(), stream);
	}

	
	
	// MARK: - Redirecting logger -
	
	RedirectingErrorLogger::RedirectingErrorLogger() :
		_info({0,0})
	{
	}
	
	RedirectingErrorLogger::~RedirectingErrorLogger()
	{
	}
	
	void RedirectingErrorLogger::addChildLogger(ErrorLogging * logger)
	{
		assert(std::find(_loggers.begin(), _loggers.end(), logger) == _loggers.end());
		_loggers.push_back(logger);
	}
	
	void RedirectingErrorLogger::removeChildLogger(ErrorLogging * logger)
	{
		auto it = std::find(_loggers.begin(), _loggers.end(), logger);
		if (it != _loggers.end()) {
			_loggers.erase(it);
		}
	}
	
	// ErrorLogging interface
	void RedirectingErrorLogger::error(const std::string & message) {
		error(ErrorInfo(), message);
	}
	void RedirectingErrorLogger::error(const ErrorInfo & info, const std::string & message) {
		_info.errorsCount++;
		for (auto log: _loggers) {
			log->error(info, message);
		}
	}
	void RedirectingErrorLogger::warning(const std::string & message) {
		warning(ErrorInfo(), message);
	}
	void RedirectingErrorLogger::warning(const ErrorInfo & info, const std::string & message) {
		_info.warningsCount++;
		for (auto log: _loggers) {
			log->warning(info, message);
		}
	}
	void RedirectingErrorLogger::info(const std::string & message) {
		info(ErrorInfo(), message);
	}
	void RedirectingErrorLogger::info(const ErrorInfo & info, const std::string & message) {
		for (auto log: _loggers) {
			log->info(info, message);
		}
	}
	void RedirectingErrorLogger::debug(const std::string & message) {
		info(ErrorInfo(), message);
	}
	void RedirectingErrorLogger::debug(const ErrorInfo & info, const std::string & message) {
		for (auto log: _loggers) {
			log->debug(info, message);
		}
	}
	ErrorLogging::Info RedirectingErrorLogger::getInfo() const {
		return _info;
	}
	void RedirectingErrorLogger::resetInfo() {
		_info = {0, 0};
	}

} // bastapir
