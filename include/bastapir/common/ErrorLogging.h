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

#include <bastapir/common/ErrorInfo.h>

namespace bastapir
{
	// MARK: - Abstract logger
	
	class ErrorLogging
	{
	public:
		enum Severity
		{
			SevError, SevWarning, SevInfo, SevDebug
		};
		
		// Report error
		virtual void error(const std::string & message) = 0;
		virtual void error(const ErrorInfo & info, const std::string & message) = 0;
		
		// Report warning
		virtual void warning(const std::string & message) = 0;
		virtual void warning(const ErrorInfo & info, const std::string & message) = 0;
		
		// Print information
		virtual void info(const std::string & message) = 0;
		virtual void info(const ErrorInfo & info, const std::string & message) = 0;
		
		// Print debug information
		virtual void debug(const std::string & message) = 0;
		virtual void debug(const ErrorInfo & info, const std::string & message) = 0;
	};
	
	
	// MARK: - File looger
	
	/// Concrete implementation of ErrorLogging interface.
	class FileErrorLogger: public ErrorLogging
	{
	public:
		FileErrorLogger(FILE * std_out = stdout, FILE * err_out = stderr, bool close_streams = false);
		~FileErrorLogger();
		
		void setLogPrefix(const std::string & prefix);
		const std::string & prefix() const;
		
		void setBasePathForSourceFiles(const std::string & base_path);
		const std::string & basePathForSourceFiles() const;
		
		void setMinimumDisplayedSeverity(Severity severity);
		Severity minimumDisplayedSeverity() const;
		
		// ErrorLogging interface
		virtual void error(const std::string & message);
		virtual void error(const ErrorInfo & info, const std::string & message);
		virtual void warning(const std::string & message);
		virtual void warning(const ErrorInfo & info, const std::string & message);
		virtual void info(const std::string & message);
		virtual void info(const ErrorInfo & info, const std::string & message);
		virtual void debug(const std::string & message);
		virtual void debug(const ErrorInfo & info, const std::string & message);

	private:
		
		void dump(FILE * stream, Severity severity, const ErrorInfo & ei, const std::string & message);
		
		std::string _prefix;
		std::string _base_path;
		
		FILE * _out;
		FILE * _err;
		Severity _min_severity;
		bool _close_streams;
	};
	
	// MARK: - Redirecting logger
	
	class RedirectingErrorLogger: public ErrorLogging
	{
	public:
		RedirectingErrorLogger();
		~RedirectingErrorLogger();

		void addChildLogger(ErrorLogging * logger);
		void removeChildLogger(ErrorLogging * logger);
		
		// ErrorLogging interface
		virtual void error(const std::string & message);
		virtual void error(const ErrorInfo & info, const std::string & message);
		virtual void warning(const std::string & message);
		virtual void warning(const ErrorInfo & info, const std::string & message);
		virtual void info(const std::string & message);
		virtual void info(const ErrorInfo & info, const std::string & message);
		virtual void debug(const std::string & message);
		virtual void debug(const ErrorInfo & info, const std::string & message);
		
	private:
		std::vector<ErrorLogging*> _loggers;
	};
}
