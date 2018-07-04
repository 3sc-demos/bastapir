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

//
// Compatibility with non-clang compilers.
//
#ifndef __has_builtin
	#define __has_builtin(x)	0
#endif
#ifndef __has_feature
	#define __has_feature(x)	0
#endif
#ifndef __has_extension
	#define __has_extension		__has_feature
#endif

//
// Small platform switch
//
#ifdef __APPLE__

	#include <stdlib.h>
	#include <string.h>

	#define BASTAPIR_UNIX	1
	#define BASTAPIR_WIN	0

#elif defined(WINAPI_FAMILY)

	#include <sdkddkver.h>
	#include <Windows.h>

	#define BASTAPIR_UNIX	0
	#define BASTAPIR_WIN	1

#endif
