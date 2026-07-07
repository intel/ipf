/******************************************************************************
** Copyright (c) 2020-2026 Intel Corporation All Rights Reserved
**
** Licensed under the Apache License, Version 2.0 (the "License"); you may not
** use this file except in compliance with the License.
**
** You may obtain a copy of the License at
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
** WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**
** See the License for the specific language governing permissions and
** limitations under the License.
**
******************************************************************************/

#pragma once

// OS Abstraction Layer for Dynamically Loaded Libraries
#ifdef _WIN32
#include <windows.h>
#define DLLHANDLE HMODULE
#define DLLEXT ".dll"
#define DLLPREFIX ""
#define DLLFILENAME(name) name DLLEXT
#define DLLSEP "\\"
#define DLLOpen(lib) LoadLibraryExA(lib, NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS)
#define DLLSymbol(h, fn) GetProcAddress(h, fn)
#define DLLClose(h) FreeLibrary(h)
#else
#include <dlfcn.h>
#define DLLHANDLE void*
#define DLLEXT ".so"
#define DLLPREFIX "lib"
#define DLLFILENAME(name) DLLPREFIX name DLLEXT
#define DLLSEP "/"
#define DLLOpen(lib) dlopen(lib, RTLD_NOW | RTLD_GLOBAL)
#define DLLSymbol(h, fn) dlsym(h, fn)
#define DLLClose(h) dlclose(h)
#define ERROR_SUCCESS 0
#endif
