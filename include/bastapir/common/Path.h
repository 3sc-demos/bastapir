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
	
	/// The `PathComponents` contains various information parsed
	/// from full file or directory path.
	struct PathComponents
	{
		/// Parent directory. Examples:
		/// - If path is `/tmp/text.txt`, then directory is `/tmp`
		/// - If path is `text.txt`, then directory is empty.
		std::string directory;
		/// File or directory name. Examples:
		/// - If path is `/tmp/text.txt`, then file name  is `text.txt`
		/// - If path is `text.txt`, then file name is `text.txt`.
		std::string fileName;
		/// File name without an extension. Examples:
		/// - If path is `/tmp/text.txt`, then property is `text`
		/// - If path is `text.txt`, then property is `text`.
		/// - If path is `text.something.txt`, then property is `text.something`.
		std::string fileNameNoExt;
		/// Contains file extension. Examples:
		/// - If path is `/tmp/text.txt`, then extension is `txt`
		/// - If path is `text.txt`, then extension is `txt`.
		/// - If path is `text.something.txt`, then extension is `txt`.
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
		/// Note that function doesn't cache components internally, so each call for components
		/// will parse stored path.
		PathComponents pathComponents() const;
		
		/// Converts path in any format to platform specific path.
		static std::string convertToPlatformPath(const std::string & any_path);
		
		/// Returns PathComponents parsed from given |any_path|
		static PathComponents components(const std::string & any_path);
		
		/// Contains directory separator used on current platform.
		static const std::string directorySeparator;		
	};
	
} // bastapir
