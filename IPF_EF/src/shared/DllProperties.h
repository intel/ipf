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

#include <stdint.h>

#include "Base.h"

// Encoded SDK Version Number
// Supports Versions up to 65535.65535.65535.65535 for Windows FileVersion compatibility
typedef struct {
	uint64_t Build : 16;    // SDK Version: major.minor.revision.BUILD = 0-65535
	uint64_t Revision : 16; // SDK Version: major.minor.REVISION.build = 0-65535
	uint64_t Minor : 16;    // SDK Version: major.MINOR.revision.build = 0-65535
	uint64_t Major : 16;    // SDK Version: MAJOR.minor.revision.build = 0-65535
} version_t;

// Encoded Component Build Type Flags
typedef struct {
	uint64_t Revision : 8;      // buildtype_t Revision (0-255)
	uint64_t DebugBuild : 1;    // Is Debug Build?
	uint64_t DebugRuntime : 1;  // Is Debug Runtime? (/MDd or /MTd)? [Windows]
	uint64_t StaticRuntime : 1; // Is Static Runtime? (/MD or /MT)? [Windows]
	uint64_t Reserved : 53;     // Reserved for Future Expansion
} buildtype_t;

#define IPFSDK_BUILDTYPE_REVISION 0

// Expose SDK Build Type Properties of code calling this function
static inline buildtype_t GetSdkBuildType()
{
	buildtype_t build = {};
	build.Revision = IPFSDK_BUILDTYPE_REVISION;
#ifdef IPF_BUILD_DEBUG
	build.DebugBuild = 1;
#endif
#if defined(_WIN32) && defined(_DEBUG)
	build.DebugRuntime = 1;
#endif
#if defined(_WIN32) && !defined(_DLL)
	build.StaticRuntime = 1;
#endif
	return build;
}

// Expose SDK Version Number components as a uint64_t
static inline version_t GetSdkVersion()
{
	version_t version = {};
	version.Major = IPFSDK_VER_MAJOR;
	version.Minor = IPFSDK_VER_MINOR;
	version.Revision = IPFSDK_VER_REVISION;
	version.Build = IPFSDK_VER_BUILD;
	return version;
}

// Export these symbols only from the ClientApi DLL. Static-library consumers must not
// propagate them into the export table of the binary that links ClientApiLib.
#if defined(ClientApi_EXPORTS)
#define DLLPROPERTIES_EXPORT DLLEXPORT
#else
#define DLLPROPERTIES_EXPORT
#endif

// Referencing these symbols causes DllProperties to be linked from static libraries
extern "C"
{
	extern DLLPROPERTIES_EXPORT buildtype_t GetDllBuildType();
	extern DLLPROPERTIES_EXPORT version_t GetDllVersion();
	extern DLLPROPERTIES_EXPORT const char* GetDllProperties();
}