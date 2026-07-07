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

// IPF Extensible Framework SDK Version
#define IPFSDK_VER_MAJOR 2
#define IPFSDK_VER_MINOR 4
#define IPFSDK_VER_REVISION 8888
#define IPFSDK_VER_BUILD 9999

#define IPFSDK_TOARG(arg) #arg
#define IPFSDK_TOSTR(arg) IPFSDK_TOARG(arg)
#define IPFSDK_TOVER IPFSDK_VER_MAJOR.IPFSDK_VER_MINOR.IPFSDK_VER_REVISION.IPFSDK_VER_BUILD
#define IPFEF_SDK_VERSION IPFSDK_TOSTR(IPFSDK_TOVER)

//
// OS Base Type Definitions
//
#if defined(_WIN32)
#define IPF_OS_TYPE "Windows"
#define IPF_PLATFORM_TYPE "x64" // x86 not supported
#define IPF_INLINE __inline
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)
#define DLLCALLCONV __cdecl
#define __FUNC__ __FUNCTION__
#define UNREFERENCED_PARAMETER(x) (x)
#else
#define IPF_OS_TYPE "Linux"
#define IPF_PLATFORM_TYPE "x64" // x86 not supported
#define IPF_INLINE inline
#define DLLEXPORT __attribute__((visibility("default")))
#define DLLIMPORT
#define DLLCALLCONV
#define __FUNC__ __PRETTY_FUNCTION__
#define UNREFERENCED_PARAMETER(x) (void)(x)
#endif

// NDEBUG is a C-standard cross-OS compiler setting used to disable C-assertions.
// It has nothing to do with _DEBUG, which is Windows-specific and automatically
// set when using the C/C++ Debug Runtime Library options /MDd or /MTd
#if !defined(NDEBUG)
#define IPF_BUILD_DEBUG
#endif

#ifdef IPF_BUILD_DEBUG
#define IPF_BUILD_TYPE "Debug"
#else
#define IPF_BUILD_TYPE "Release"
#endif

// Macro for statements that are executed only in Debug builds like std::cout streams.
// Enable globally with -DENABLE_DEBUGSTMT=true cmake flag or CMakeLists.txt option.
// Individual modules can #define ENABLE_DEBUGSTMT or DISABLE_DEBUGSTMT before any other
// #includes to override global behavior and allow or suppress debug statement output.
// Individual statements can be forced or suppressed with DO_DEBUGSTMT and NO_DEBUGSTMT.
// Example: DEBUGMSG(std::cout << "Some Message" << std::endl);
#if defined(IPF_BUILD_DEBUG) && defined(ENABLE_DEBUGSTMT) && !defined(DISABLE_DEBUGSTMT)
#define DEBUGSTMT(stmt) stmt
#else
#define DEBUGSTMT(stmt) (void)(0)
#endif
#define DO_DEBUGSTMT(stmt) stmt
#define NO_DEBUGSTMT(stmt) (void)(0)

// Opaque Base Types
#include <stdint.h>
typedef uint32_t revision_t; // Interface Revision Number Type
