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

#include <string>

#include <map>

#include "Base.h"
#include "DllApi.h"
#include "DllProperties.h"
#include "errno.h"
#include <filesystem>

// Manages the lifecycle of a dynamically loaded library: open, close,
// signature verification, and build configuration checking.
// Used by DllProxy<T> and ClientApiLoader to avoid duplicating DLL
// management logic.
class DllHandle
{
public:
	DllHandle() : handle_(nullptr)
	{}

	~DllHandle()
	{
		Close();
	}

	// Disallow copy-construction by explicitly deleting the
	// copy constructor and assignment operator.
	DllHandle(const DllHandle&) = delete;
	DllHandle& operator=(const DllHandle&) = delete;

	// Open a DLL at the given path. Performs signature verification and
	// build configuration checking. Returns ERROR_SUCCESS on success,
	// or an errno-style error code on failure:
	//   EINVAL  - path is empty
	//   EEXIST  - library already loaded
	//   EILSEQ  - digital signature verification failed
	//   ENOENT  - DLLOpen failed (library not found)
	//   ENXIO   - GetDllBuildType symbol not found
	//   ENOEXEC - build configuration mismatch
	int Open(const std::filesystem::path& fpath)
	{
		std::string path = fpath.string();
		int rc = EINVAL;
		if (path.length()) {
			if (handle_) {
				rc = EEXIST;
			}
			else if (!VerifySignature(fpath)) {
				rc = EILSEQ;
			}
			else {
				handle_ = DLLOpen(path.c_str());
				if (handle_) {
					rc = VerifyBuildConfig();

					if (rc != ERROR_SUCCESS) {
						Close();
					}
				}
				else {
					rc = ENOENT;
				}
			}
		}
		return rc;
	}

	void Close()
	{
		if (handle_) {
#ifdef _WIN32
			DLLClose(handle_);
#else
			// dlclose() causes a segfault in ~ProviderAccess() after unloading the last Provider,
			// so we temporarily disable calling dlclose() on Linux platforms pending investigation
#endif
			handle_ = nullptr;
		}
	}

	DLLHANDLE Get() const
	{
		return handle_;
	}

	bool IsOpen() const
	{
		return handle_ != nullptr;
	}

	template <typename T> T LoadSymbol(const char* name)
	{
		if (!IsOpen() || name == nullptr) {
			return T();
		}
		return reinterpret_cast<T>(DLLSymbol(handle_, name));
	}

	// Returns a human-readable description for an error code returned by Open().
	static const char* ErrorMessage(int rc)
	{
		static const std::map<int, const char*> errormap = {
			{EINVAL, "Path Is Empty"},
			{EEXIST, "Library Already Loaded"},
			{ENOENT, "Cannot Open Library"},
			{ENXIO, "Build Type Symbol Not Found"},
			{ENOEXEC, "Incompatible Build Configuration"},
			{EILSEQ, "Invalid Digital Signature"},
		};
		auto it = errormap.find(rc);
		return (it != errormap.end()) ? it->second : "Unknown Error";
	}

	inline bool VerifySignature(const std::filesystem::path& libpath);

private:
	// Returns ERROR_SUCCESS if build configs match, ENXIO if the
	// GetDllBuildType symbol is missing, or ENOEXEC on mismatch.
	int VerifyBuildConfig()
	{
		using BuildTyper = buildtype_t (*)();
		BuildTyper GetDllBuildTypeFunc = LoadSymbol<BuildTyper>("GetDllBuildType");
		if (GetDllBuildTypeFunc) {
			buildtype_t self = GetSdkBuildType();
			buildtype_t dll = GetDllBuildTypeFunc();
			return (self.DebugRuntime == dll.DebugRuntime) ? ERROR_SUCCESS : ENOEXEC;
		}
		return ENXIO;
	}

	DLLHANDLE handle_;
};

#if defined(_WIN32) && !defined(IPF_BUILD_DEBUG)
//
// Verify Digital Signatures for Windows Release Builds only
//
#include <Softpub.h>
#include <wintrust.h>

#pragma comment(lib, "wintrust")

inline bool DllHandle::VerifySignature(const std::filesystem::path& libpath)
{
	LONG rc = ERROR_INVALID_PARAMETER;

	if (!libpath.empty()) {
		GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		WINTRUST_DATA wdata = {0};
		WINTRUST_FILE_INFO finfo = {0};
		std::wstring wpath = libpath.wstring();

		// Initialize Parameters
		finfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
		finfo.pcwszFilePath = wpath.c_str();

		wdata.cbStruct = sizeof(wdata);
		wdata.dwUIChoice = WTD_UI_NONE;
		wdata.fdwRevocationChecks = WTD_REVOKE_NONE;
		wdata.dwUnionChoice = WTD_CHOICE_FILE;
		wdata.dwStateAction = WTD_STATEACTION_VERIFY;
		wdata.pFile = &finfo;

		// Configure WinVerifyTrust to prevent the OS from trying to update the revocation list over
		// the internet for each call
		wdata.dwProvFlags = WTD_CACHE_ONLY_URL_RETRIEVAL;

		// Verify Digital Signature
		rc = WinVerifyTrust(NULL, &action, &wdata);

		// Close WinTrust Data and Release Handles
		wdata.dwStateAction = WTD_STATEACTION_CLOSE;
		WinVerifyTrust(NULL, &action, &wdata);
	}

	// Success only for Verified Digital Signature Signed by a Trusted Root CA
	return (rc == ERROR_SUCCESS);
}
#else
// Allow Unverified or Missing Digital Signature
inline bool DllHandle::VerifySignature(const std::filesystem::path& libpath)
{
	UNREFERENCED_PARAMETER(libpath);
	return true;
}
#endif
