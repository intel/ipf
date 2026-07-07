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

#include "DllProperties.h"

#include <filesystem>
#include <string>

#include "DirApi.h"
#include "Exceptions.h"

#ifdef _WIN32
#pragma comment(lib, "version")
#endif

// Retrieves embedded file properties for an executable or loadable library if supported by OS
static std::string GetFileProperty(const std::string& filename, const std::string& token)
{
	std::string result = "";
#ifdef _WIN32
	// Use Windows Version Library to extract File Properties (such as ProductName) from binaries:
	// 1. Get and allocate size needed for string table(s) block
	DWORD unused = 0;
	DWORD size = GetFileVersionInfoSize(filename.c_str(), &unused);
	unsigned char* block = nullptr;
	if (size) {
		block = new unsigned char[size];
	}
	if (block) {
		// 2. Retrieve language-specific string table(s) block
		unused = 0;
		if (GetFileVersionInfo(filename.c_str(), unused, size, block)) {
			struct {
				WORD lang;
				WORD codepage;
			}* translate = nullptr;
			UINT blocksize = 0;

			// 3. Iterate supported language(s) of embedded string table(s)
			if (VerQueryValueA(
					block, "\\VarFileInfo\\Translation", (LPVOID*)&translate, &blocksize)) {
				for (size_t j = 0; j < blocksize / sizeof(*translate); j++) {
					UINT len = 0;
					LPVOID value = nullptr;
					char subblock[MAX_PATH] = {0};
					sprintf_s(
						subblock,
						sizeof(subblock),
						"\\StringFileInfo\\%04x%04x\\%s",
						translate[j].lang,
						translate[j].codepage,
						token.c_str());

					// 4. Read language-specific sub-block for the requested property name
					if (VerQueryValueA(block, subblock, &value, &len)) {
						result = (const char*)value;
						break;
					}
				}
			}
		}
		delete[] block;
	}
#else
	// Currently unsupported on Linux
	UNREFERENCED_PARAMETER(filename);
	UNREFERENCED_PARAMETER(token);
#endif
	return result;
}

using namespace Ipf;

// Exported Symbols
extern "C"
{
	DLLEXPORT buildtype_t GetDllBuildType()
	{
		return GetSdkBuildType();
	}

	DLLEXPORT version_t GetDllVersion()
	{
		return GetSdkVersion();
	}

	DLLEXPORT const char* GetDllProperties()
	{
		static std::string properties;
		if (properties.empty()) {
			buildtype_t build = GetSdkBuildType();
			std::string pathname = GetCurrentModulePathname();

			properties = R"({
	"schema_type": "module_properties",
	"schema_revision": 1,
	"body": {
		"filename": ")" + std::filesystem::path(pathname).filename().string() +
						 R"(",
		"is_debug_build": )" +
						 (build.DebugBuild ? "true" : "false") + R"(,
		"is_debug_runtime": )" +
						 (build.DebugRuntime ? "true" : "false") + R"(,
		"is_static_runtime": )" +
						 (build.StaticRuntime ? "true" : "false") + R"(,
		"product_copyright": ")" +
						 GetFileProperty(pathname, "LegalCopyright") + R"(",
		"product_description": ")" +
						 GetFileProperty(pathname, "FileDescription") + R"(",
		"product_name": ")" +
						 GetFileProperty(pathname, "ProductName") + R"(",
		"product_vendor": ")" +
						 GetFileProperty(pathname, "CompanyName") + R"(",
		"product_version": ")" +
						 GetFileProperty(pathname, "ProductVersion") + R"(",
		"sdk_version": ")" +
						 IPFEF_SDK_VERSION + R"("
	}
}
)";
		}
		return properties.c_str();
	}
}