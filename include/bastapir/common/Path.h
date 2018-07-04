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

#include <bastapir/common/Types.h>

namespace bastapir
{
	// MAR: - PathComponents
	
	struct PathComponents
	{
		std::string directory;
		std::string fileName;
		std::string fileNameNoExt;
		std::string extension;
	};
	
	// MARK: - Path object
	
	class Path
	{
	public:
		
		/// Constructs Path object with any path. The provided path is automatically transformed
		/// to platform specific path.
		Path(const std::string & any_path);
		
		/// path string in platform specific format.
		const std::string path;
		
		/// Returns PathComponents parsed from path stored in this object.
		/// Note that function
		PathComponents pathComponents() const;
		
		/// Converts path in any format to platform specific path.
		static std::string convertToPlatformPath(const std::string & any_path);
		
		/// Returns PathComponents parsed from given |any_path|
		static PathComponents components(const std::string & any_path);
		
		/// Contains directory separator used on current platform.
		static const std::string directorySeparator;
		
	private:
		
	};
	
} // bastapir
