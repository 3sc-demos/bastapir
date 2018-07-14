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

#include <bastapir/common/Types.h>

namespace bastapir
{
	/// The `ErrorInfo` structure holds information about source file
	/// where the error occured.
	struct ErrorInfo
	{
		/// File name of source file.
		/// If string is empty, then it's considered that structure doesn't
		/// contain information about error.
		std::string sourceFile;
		/// Line where the error occured.
		/// If value is equal to 0, then the line is not known.
		size_t line 	= 0;
		/// Column in the line, where the error occured.
		/// If value is equal to 0, then the column is not known.
		size_t column	= 0;
		
		/// Returns true if structure contains a valid information.
		bool hasInfo() const
		{
			return !sourceFile.empty();
		}
	};
	
} // bastapir
