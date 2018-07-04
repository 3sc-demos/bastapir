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

#include <bastapir/common/Path.h>

namespace bastapir
{
#if defined(BASTAPIR_UNIX)
	// macOS, Linux, + other right systems
	static const char s_other_separator = '\\';
	static const char s_right_separator = '/';
#elif defined(BASTAPIR_WIN)
	// Windows,  MS-DOS, etc... :D
	static const char s_other_separator = '/';
	static const char s_right_separator = '\\';
#else
	#error "Undefined platform"
#endif
	
	const std::string Path::directorySeparator({s_right_separator});
	
	Path::Path(const std::string & path) :
		path(convertToPlatformPath(path))
	{
	}
	
	PathComponents Path::pathComponents() const
	{
		PathComponents c;
		// Look for last directory separator
		auto last_dir_index = path.rfind(s_right_separator);
		if (last_dir_index == path.npos) {
			c.fileName = path;
		} else {
			c.directory = path.substr(0, last_dir_index + 1);
			c.fileName  = path.substr(last_dir_index + 1);
		}
		// Look for extension in file name
		auto ext_index = c.fileName.rfind('.');
		if (ext_index == c.fileName.npos || ext_index == 0) {
			c.fileNameNoExt = c.fileName;
		} else {
			c.fileNameNoExt = c.fileName.substr(0, ext_index);
			c.extension     = c.fileName.substr(ext_index + 1);
		}
		return c;
	}
	
	std::string Path::convertToPlatformPath(const std::string & any_path)
	{
		std::string out;
		out.resize(any_path.size());
		std::transform(any_path.begin(), any_path.end(), out.begin(), [] (char c)->char {
			return (c != s_other_separator) ? c : s_right_separator;
		});
		return out;
	}
	
	PathComponents Path::components(const std::string & any_path)
	{
		return Path(any_path).pathComponents();
	}
	
} // bastapir
