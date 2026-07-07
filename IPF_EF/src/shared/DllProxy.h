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

#include "Base.h"
#include "DllApi.h"
#include "DllProperties.h"
#include "errno.h"
#include <filesystem>

template <class T> class DllProxy
{
public:
	DllProxy(const std::string& objname = "");

	// Disallow copy-construction by explicitly deleting the
	// copy constructor and assignment operator.
	DllProxy(const DllProxy&) = delete;
	DllProxy& operator=(const DllProxy&) = delete;

	virtual ~DllProxy();

	virtual int Open(const std::filesystem::path&); // Load Library and Get Function Pointers
	virtual void Close();                           // Close and Unload Library

	template <typename P1>        // template <typename P1, typename P2, typename P3, ...>
	T* CreateInstance(P1 config); // Create Instance of (T*)
	void DestroyInstance(T* obj); // Destroy Instance of (T*)
	revision_t GetRevision();     // Return Revision Number of (T*)

	inline bool VerifySignature(const std::filesystem::path&); // Verify Digital Signature

protected:
	using super = DllProxy<T>; // Typedef of this Base class for use in Derived classes

private:
	DLLHANDLE handle_;                  // OS-specific DLL Handle
	std::string objname_;               // Object Name for DLL Function Lookup (ObjectNameCreate,
										// ObjectNameDestroy, etc)
	typename T::Creator creator_;       // DLL Function to Create Instance of (T*)
	typename T::Destroyer destroyer_;   // DLL Function to Destroy Instance of (T*)
	typename T::Revisioner revisioner_; // DLL Function to Return Revision of (T*)
};

template <class T>
DllProxy<T>::DllProxy(const std::string& objname)
	: handle_(), objname_(), creator_(), destroyer_(), revisioner_()
{
	objname_ = objname;
}

template <class T> DllProxy<T>::~DllProxy()
{
	DllProxy::Close();
}

// Load Library and Get Function Pointers
template <class T> int DllProxy<T>::Open(const std::filesystem::path& fpath)
{
	std::string path = fpath.string();
	int rc = EINVAL;
	bool build_configs_match = false;
	if (path.length()) {
		if (handle_) {
			rc = EEXIST;
		}
		else if (!VerifySignature(path)) { // Verify Digital Signature
			rc = EILSEQ;
		}
		else {
			// Open DLL and Lookup Exported Functions
			handle_ = DLLOpen(path.c_str());
			if (handle_) {
				rc = ENXIO;
				creator_ = reinterpret_cast<typename T::Creator>(
					DLLSymbol(handle_, (objname_ + "Create").c_str()));
				destroyer_ = reinterpret_cast<typename T::Destroyer>(
					DLLSymbol(handle_, (objname_ + "Destroy").c_str()));
				revisioner_ = reinterpret_cast<typename T::Revisioner>(
					DLLSymbol(handle_, (objname_ + "Revision").c_str()));

				// Verify build config is same as current module
				using BuildTyper = buildtype_t (*)();
				BuildTyper GetDllBuildTypeFunc =
					reinterpret_cast<BuildTyper>(DLLSymbol(handle_, "GetDllBuildType"));

				if (GetDllBuildTypeFunc) {
					buildtype_t self = GetSdkBuildType();
					buildtype_t dll = GetDllBuildTypeFunc();
					if (self.DebugRuntime == dll.DebugRuntime) {
						build_configs_match = true;
					}
					else {
						rc = ENOEXEC;
					}
				}

				// Success if DLL Loaded with required function exports and compatible build config
				if (handle_ && creator_ && destroyer_ && revisioner_ && build_configs_match) {
					rc = ERROR_SUCCESS;
				}
			}
			else {
				rc = ENOENT;
			}

			// Return Success or Close on Error
			if (rc != ERROR_SUCCESS) {
				Close();
			}
		}
	}
	return rc;
}

// Close and Unload Library
template <class T> void DllProxy<T>::Close()
{
	if (handle_) {
#ifdef _WIN32
		DLLClose(handle_);
#else
		// dlclose() causes a segfault in ~ProviderAccess() after unloading the last Provider,
		// so we temporarily disable calling dlclose() on Linux platforms pending investigation
#endif
		handle_ = nullptr;
		creator_ = nullptr;
		destroyer_ = nullptr;
		revisioner_ = nullptr;
	}
}

// Safe Creator that only calls DL Creator if DL is loaded
template <class T>
template <typename P1> // <typename P1, typename P2, ...> if more constructor parameters
T* DllProxy<T>::CreateInstance(P1 config)
{
	T* obj = nullptr;
	if (creator_) {
		obj = creator_(config);
	}
	return obj;
}

// Safe Destroyer that only calls DL Destroyer if DL is loaded
template <class T> void DllProxy<T>::DestroyInstance(T* obj)
{
	if (destroyer_ && obj) {
		destroyer_(obj);
	}
}

// Safe Revisioner that only calls DL Versioner if DL is loaded
template <class T> revision_t DllProxy<T>::GetRevision()
{
	revision_t version = 0;
	if (revisioner_) {
		version = revisioner_();
	}
	return version;
}

#if defined(_WIN32) && !defined(IPF_BUILD_DEBUG)
//
// Verify Digital Signatures for Windows Release Builds only
//
#include <Softpub.h>
#include <wintrust.h>

#pragma comment(lib, "wintrust")

template <class T> inline bool DllProxy<T>::VerifySignature(const std::filesystem::path& libpath)
{
	LONG rc = ERROR_INVALID_PARAMETER;

	if (!libpath.empty()) {
		GUID action = WINTRUST_ACTION_GENERIC_VERIFY_V2;
		WINTRUST_DATA wdata = {0};
		WINTRUST_FILE_INFO finfo = {0};
		wchar_t pathname[MAX_PATH] = {0};

		// Initialize Parameters
		wcscpy_s(pathname, MAX_PATH, libpath.wstring().c_str());
		finfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
		finfo.pcwszFilePath = pathname;

		wdata.cbStruct = sizeof(wdata);
		wdata.dwUIChoice = WTD_UI_NONE;
		wdata.fdwRevocationChecks = WTD_REVOKE_NONE;
		wdata.dwUnionChoice = WTD_CHOICE_FILE;
		wdata.dwStateAction = WTD_STATEACTION_VERIFY;
		wdata.pFile = &finfo;

		// Configure WinVerifyTrust to prevent the OS from trying to update the revocation list over the internet for each call
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
template <class T> inline bool DllProxy<T>::VerifySignature(const std::filesystem::path& libpath)
{
	UNREFERENCED_PARAMETER(libpath);
	return true;
}
#endif
