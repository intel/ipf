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

#include "Base.h"
#include "DllApi.h"
#include "Exceptions.h"
#include "OsAbstractLayer.h"
#include <cstring>
#include <filesystem>
#include <string>

#ifdef _WIN32
#include <cfgmgr32.h>
#pragma comment(lib, "cfgmgr32")

#ifndef ENABLE_LOCALFOLDER
#include <shlobj.h>
#pragma comment(lib, "shell32")
#endif

#include "win/StringUtils.h"

#else
#include <filesystem>     // for std::filesystem
#include <libgen.h>       // for dirname
#include <linux/limits.h> // for PATH_MAX
#include <unistd.h>       // for readlink
#endif

namespace Ipf
{

// IPF Driver Registry Constants
#ifdef _WIN32
namespace IpfDriverRegistry
{
constexpr const char* SERVICE_SUBKEY = "SYSTEM\\CurrentControlSet\\Services\\ipf_cpu";
constexpr const char* IMAGE_PATH_VALUE = "ImagePath";
constexpr const char* SYSTEM_ROOT_PREFIX = "\\SystemRoot";
constexpr const char* SYSTEM_ROOT_ENV = "%SystemRoot%";
} // namespace IpfDriverRegistry
#endif

// Configuration for framework directory loading retry behavior
struct FrameworkLoadConfig {
	unsigned int initial_framework_retries = 121; // Default: 121 attempts (2 min @ 1sec/attempt)
	unsigned int retry_delay_ms = 1000;           // Default: 1 second between retries
};

// IPF Driver installation status
enum class IpfDriverStatus {
	NOT_INSTALLED, // Never installed - no registry key
	UNINSTALLED,   // Was installed but removed - key exists, path invalid
	INSTALLED      // Currently installed - key exists, path valid
};

static inline std::string GetCurrentModulePathname()
{
	std::string pathname;

#ifdef _WIN32
	// Use Handle for the Module (EXE or DLL) that called this function to lookup path
	HMODULE libHandle = NULL;
	DWORD err = ERROR_INVALID_HANDLE;
	char modpath[MAX_PATH] = {0};

	if (GetModuleHandleExA(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
			(LPCSTR)&GetCurrentModulePathname,
			&libHandle)) {
		if ((GetModuleFileNameA(libHandle, modpath, sizeof(modpath)) > 0) &&
			((err = GetLastError()) == ERROR_SUCCESS)) {
			pathname = modpath;
		}
		FreeLibrary(libHandle);
	}
	if (err != ERROR_SUCCESS) {
		throw IpfException("Error Reading Module Path [" + std::to_string(err) + "]");
	}
#else
	// Use path of current module if an .so file, otherwise use process path
	Dl_info info = {};
	if (dladdr((const void*)GetCurrentModulePathname, &info)) {
		if (std::filesystem::path(info.dli_fname).extension() == DLLEXT) {
			pathname = info.dli_fname;
		}
		else {
			char exepath[PATH_MAX] = {0};
			ssize_t len = readlink("/proc/self/exe", exepath, sizeof(exepath) - 1);
			if (len != -1) {
				pathname = exepath;
			}
		}
	}
	if (pathname.empty()) {
		throw IpfException("Error Reading Module Path");
	}
#endif

	return pathname;
}

#ifdef _WIN32
static inline DWORD GetDriverStoreDir(GUID deviceGuid, char* path, DWORD path_len)
{
	RETURN_TYPE status = CR_INVALID_POINTER;
	ULONG devListSize = 0;
	PZZSTR devListPtr = NULL;
	PCSTR filter = "SWD";
	LPCSTR regKeyName = "InstallDir";

	// Verify Parameters
	if (path && path_len) {

		// Get a list of all PCI devices currently present in the system
		status = CM_Get_Device_ID_List_SizeA(
			&devListSize, filter, CM_GETIDLIST_FILTER_ENUMERATOR | CM_GETIDLIST_FILTER_PRESENT);

		// Allocate and load a buffer for the Device ID List
		if (status == CR_SUCCESS) {
			if (devListSize) {
				devListPtr = (PZZSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, devListSize);
			}
			if (devListPtr == NULL) {
				status = CR_OUT_OF_MEMORY;
			}
		}
		if (status == CR_SUCCESS) {
			status = CM_Get_Device_ID_ListA(
				filter,
				devListPtr,
				devListSize,
				CM_GETIDLIST_FILTER_ENUMERATOR | CM_GETIDLIST_FILTER_PRESENT);
		}

		// Find the device exposing our PnP interface
		if (status == CR_SUCCESS) {
			DEVINSTID_A devId = devListPtr;
			ULONG devListRemaining = devListSize;
			status = CR_NO_SUCH_VALUE;

			// Check if device exposes the desired interface
			while (*devId) {
				ULONG ifaceListSize = 0;
				status = CM_Get_Device_Interface_List_SizeA(
					&ifaceListSize,
					(LPGUID)&deviceGuid,
					devId,
					CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES);

				PZZSTR devIdListPtr = NULL;
				HKEY hIpf = NULL;
				DWORD pathLen = path_len;

				if (status == CR_SUCCESS) {
					devIdListPtr =
						(PZZSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ifaceListSize);
					if (devIdListPtr == NULL) {
						status = CR_OUT_OF_MEMORY;
					}
				}
				if (status == CR_SUCCESS) {
					status = CM_Get_Device_Interface_ListA(
						(LPGUID)&deviceGuid,
						devId,
						devIdListPtr,
						ifaceListSize,
						CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES);
				}
				if (status == CR_SUCCESS) {
					if (ifaceListSize > 1) {
						status = CM_Open_Device_Interface_Key_ExA(
							devIdListPtr,
							GENERIC_READ,
							RegDisposition_OpenExisting,
							&hIpf,
							0,
							NULL);

						if (status == CR_SUCCESS) {
							if (ERROR_SUCCESS !=
								RegQueryValueEx(
									hIpf, regKeyName, NULL, NULL, (LPBYTE)path, &pathLen)) {
								status = CR_NO_SUCH_VALUE;
							}
							RegCloseKey(hIpf);
						}
					}
					else {
						status = CR_NO_SUCH_VALUE;
					}
				}

				// Free DevIdList
				if (devIdListPtr) {
					HeapFree(GetProcessHeap(), 0, devIdListPtr);
				}
				if (status == CR_SUCCESS) {
					break;
				}
				size_t devIdLen = ipfef_osal_strnlen_s((LPSTR)devId, devListRemaining) + 1;
				devId += devIdLen;
				devListRemaining -= (ULONG)devIdLen;
				status = CR_NO_SUCH_VALUE;
			}
		}

		// Free DevList
		if (devListPtr) {
			HeapFree(GetProcessHeap(), 0, devListPtr);
		}
	}
	return status;
}

// Returns TRUE if the OS has been up for at least the specified number of seconds
static inline BOOL IsSystemUptime(ULONG seconds)
{
	ULONGLONG ticks = GetTickCount64();
	return (ticks / 1000 >= seconds);
}

// Returns TRUE if the current process is running as a Windows Service
static inline BOOL IsRunningAsService()
{
	static BOOL initialized = FALSE;
	static BOOL isService = FALSE;

	// The definitive way to check if the current process is currently running as a Service
	// is to use EnumServiceStatus, QueryService, and QueryServiceStatusEx to enumerate
	// all Services and see if any of their Process IDs match the current process ID.
	//
	// However, this is a relatively slow operation (~150-250ms) so to optimize performance,
	// We use a different method that checks whether the current Process Token contains any
	// of any of the following well-known SIDs:
	//    1. SECURITY_LOCAL_SYSTEM_RID: Added to the Process Token by Windows when running
	//       a Process as NT AUTHORITY\SYSTEM (LocalSystem)
	//    2. SECURITY_SERVICE_RID: Added to the Process Token by the Service Control Manager
	//       when starting a Service for any Service Account *EXCEPT* LocalSystem including:
	//         * NT AUTHORITY\LOCAL SERVICE (LocalService)
	//         * NT AUTHORITY\NETWORK SERVICE (NetworkService)
	//         * Any Local or Domain user with "Log on as a Service" rights in Local or Group Policy
	//
	//  Oddly, SECURITY_SERVICE_RID is NOT set when running a Service as NT AUTHORITY\SYSTEM,
	//  so we check the Process Token for both SIDs and if either is set, we assume we are running
	//  as a Service for the purposes of the Extensible Framework: So we can retry failed
	//  DriverStore lookups if a Service starts before the EF interface has been exposed by Windows
	//
	if (!initialized) {
		DWORD SidList[] = {
			SECURITY_LOCAL_SYSTEM_RID, // LocalSystem only
			SECURITY_SERVICE_RID,      // LocalService, NetworkService, Local or Domain Users
		};
		for (size_t j = 0; !isService && j < sizeof(SidList) / sizeof(SidList[0]); j++) {
			SID_IDENTIFIER_AUTHORITY NtAuthority = {SECURITY_NT_AUTHORITY};
			PSID sid = NULL;
			BOOL isMember = FALSE;

			if (AllocateAndInitializeSid(&NtAuthority, 1, SidList[j], 0, 0, 0, 0, 0, 0, 0, &sid)) {
				if (CheckTokenMembership(NULL, sid, &isMember)) {
					isService = isMember;
				}
				FreeSid(sid);
			}
		}
		initialized = TRUE;
	}
	return isService;
}

// Returns TRUE if the current process is running as a UMDF Driver
static inline BOOL IsRunningAsUmdfDriver()
{
	static BOOL initialized = FALSE;
	static BOOL isUmdf = FALSE;

	// Get Current EXE name and see if it the UMDF Driver Host full pathname (WUDFHost.exe)
	if (!initialized) {
		char pathname[MAX_PATH] = {0};
		char umdfpath[MAX_PATH] = {0};
		char umdfhost[] = "\\WUDFHost.exe";
		if (GetModuleFileNameA(NULL, pathname, sizeof(pathname)) < sizeof(pathname) &&
			GetSystemDirectoryA(umdfpath, sizeof(umdfpath)) < sizeof(umdfpath) - sizeof(umdfhost)) {
			strcat_s(umdfpath, sizeof(umdfpath), umdfhost);
			if (_stricmp(pathname, umdfpath) == 0) {
				isUmdf = TRUE;
			}
		}
		initialized = TRUE;
	}
	return isUmdf;
}

// Helper: Get driver image path from registry
// Returns empty string if registry key doesn't exist or on error
static inline std::string GetDriverImagePathFromRegistry()
{
	wchar_t buffer[MAX_PATH] = {0};
	DWORD cbData = sizeof(buffer);
	DWORD dwType = REG_EXPAND_SZ;

	if (RegGetValueW(
			HKEY_LOCAL_MACHINE,
			Ipf::Utility::Utf8ToUtf16(IpfDriverRegistry::SERVICE_SUBKEY).c_str(),
			Ipf::Utility::Utf8ToUtf16(IpfDriverRegistry::IMAGE_PATH_VALUE).c_str(),
			RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ | RRF_NOEXPAND,
			&dwType,
			buffer,
			&cbData) == ERROR_SUCCESS) {
		return Ipf::Utility::Utf16ToUtf8(buffer);
	}
	return std::string();
}

// Helper: Normalize driver path by removing quotes, parameters, and converting SystemRoot
static inline std::string NormalizeDriverPath(const std::string& rawPath)
{
	if (rawPath.empty()) {
		return std::string();
	}

	std::string path = rawPath;

	// Remove quotes and truncate parameters
	if (path[0] == '"') {
		size_t endQuote = path.find('"', 1);
		if (endQuote != std::string::npos) {
			path = path.substr(1, endQuote - 1);
		}
	}
	else {
		size_t spacePos = path.find(' ');
		if (spacePos != std::string::npos) {
			path = path.substr(0, spacePos);
		}
	}

	// Convert "\SystemRoot" to "%SystemRoot%"
	if (path.substr(0, strlen(IpfDriverRegistry::SYSTEM_ROOT_PREFIX)) ==
		IpfDriverRegistry::SYSTEM_ROOT_PREFIX) {
		path = std::string(IpfDriverRegistry::SYSTEM_ROOT_ENV) +
			   path.substr(strlen(IpfDriverRegistry::SYSTEM_ROOT_PREFIX));
	}

	// Add "%SystemRoot%\" if necessary to relative paths
	if (!path.empty() && path[0] != '%' && path[0] != '\\' && path[0] != '/' &&
		(path.length() < 2 || path[1] != ':')) {
		path = std::string(IpfDriverRegistry::SYSTEM_ROOT_ENV) + "\\" + path;
	}

	// Expand "%SystemRoot%" and other environment variables
	std::wstring pathW = Ipf::Utility::Utf8ToUtf16(path);
	wchar_t expanded[MAX_PATH] = {0};
	DWORD requiredSize = ExpandEnvironmentStringsW(pathW.c_str(), expanded, MAX_PATH);
	if (requiredSize > 0 && requiredSize <= MAX_PATH) {
		path = Ipf::Utility::Utf16ToUtf8(expanded);
	}
	else if (requiredSize > MAX_PATH) {
		// Allocate a buffer large enough for the expanded string (size includes null terminator)
		std::wstring expandedDynamic;
		expandedDynamic.resize(requiredSize);
		DWORD requiredSize2 = ExpandEnvironmentStringsW(
			pathW.c_str(), expandedDynamic.data(), static_cast<DWORD>(expandedDynamic.size()));
		if (requiredSize2 > 0 && requiredSize2 <= expandedDynamic.size()) {
			path = Ipf::Utility::Utf16ToUtf8(expandedDynamic);
		}
	}

	return path;
}

// Helper: Validate that the driver path exists on the file system
static inline bool ValidateDriverPath(const std::string& path)
{
	if (path.empty()) {
		return false;
	}

	try {
		return std::filesystem::exists(path);
	}
	catch (...) {
		// If we can't check the path (e.g., permission denied, invalid path),
		// treat as not existing
		return false;
	}
}

// Check if IPF driver (ipf_cpu) is installed by querying the registry
static inline IpfDriverStatus CheckIpfDriverInstallation()
{
	// Try to get ImagePath from registry
	std::string rawPath = GetDriverImagePathFromRegistry();

	// If registry key doesn't exist, driver was never installed
	if (rawPath.empty()) {
		return IpfDriverStatus::NOT_INSTALLED;
	}

	// Normalize the path (remove quotes, expand environment variables, etc.)
	std::string normalizedPath = NormalizeDriverPath(rawPath);

	// Check if the driver file actually exists
	if (ValidateDriverPath(normalizedPath)) {
		return IpfDriverStatus::INSTALLED;
	}

	// Registry key exists but file is missing - was installed but removed
	return IpfDriverStatus::UNINSTALLED;
}
#endif

static inline std::string
GetFrameworkDirectory(const FrameworkLoadConfig& config = FrameworkLoadConfig())
{
	std::string folder;

#ifdef _WIN32
	//
	// The precedence for computing the Framework Directory is defined below:
	//
	// 1.  Always use DriverStore path of INF-installed Framework, if installed [Release and Debug
	// builds]
	char DriverStorePath[MAX_PATH] = {0};
	static const GUID IpfEfSwGuid = {
		// 9DD57132-5929-476A-8CCB-9D42B1E4ABED
		0x9dd57132,
		0x5929,
		0x476a,
		{0x8c, 0xcb, 0x9d, 0x42, 0xb1, 0xe4, 0xab, 0xed}};

	// Check if IPF driver is installed before attempting retry loop
	IpfDriverStatus driverStatus = CheckIpfDriverInstallation();

	// If we are Running as a Service or UMDF Driver, retry failed DriverStore
	// lookups to avoid a race condition where Services/UMDFs may start before
	// the IPF EF Interface Device has been exposed by Windows.
	//
	// Note: When running Unit Tests in Jenkins or using tools like PSEXEC to run
	// processes as LocalSystem or LocalService, it will always appear as if the
	// current process is running as a Service, which will cause timeouts if IPF
	// is not installed, so we only do the Service and UMDF checks within the first
	// 5 minutes of boot time. This allows Windows to 1-2min to start before any
	// Services or UMDF drivers are loaded, and an additional 3-4 minutes for
	// the IPF UMDF Driver and IPF Server to start.
	//
	// Only retry if driver appears to be installed
	DWORD cr = CR_DEFAULT;
	DWORD retry_delay = 0;
	DWORD max_attempts = 1;
	DWORD attempts = 0;
	constexpr ULONG max_uptime = 300; // Hardcoded: 5 minutes
	if (driverStatus == IpfDriverStatus::INSTALLED && !IsSystemUptime(max_uptime) &&
		(IsRunningAsService() || IsRunningAsUmdfDriver())) {
		retry_delay = config.retry_delay_ms;
		max_attempts = static_cast<DWORD>(config.initial_framework_retries);
	}
	while (cr != CR_SUCCESS && attempts < max_attempts) {
		if (attempts++ > 0) {
			Sleep(retry_delay);
		}
		cr = GetDriverStoreDir(IpfEfSwGuid, DriverStorePath, MAX_PATH);
	}

	if (cr == CR_SUCCESS) {
		folder = DriverStorePath;
	}

#ifdef IPF_BUILD_DEBUG
#ifdef ENABLE_LOCALFOLDER
	// 2a. Use Current Module Directory [Local (non-Jenkins) Debug Builds]
	if (folder.empty()) {
		try {
			folder = std::filesystem::path(GetCurrentModulePathname()).parent_path().string();
		}
		catch (...) {
		}
	}
#else
	// 2b. Use Program Files Directory [Jenkins Debug Builds]
	if (folder.empty()) {
		char ProgramFiles[MAX_PATH] = {0};
		if (SHGetSpecialFolderPathA(0, ProgramFiles, CSIDL_PROGRAM_FILES, FALSE)) {
			folder = std::string(ProgramFiles) + DLLSEP + "IPF" + DLLSEP + IPF_BUILD_TYPE;
		}
	}

#endif
#endif

#else
	// For Linux, the Current Module (EXE or DLL) Directory is the Framework Directory
	int attempts = 1;
	try {
		folder = std::filesystem::path(GetCurrentModulePathname()).parent_path().string();
	}
	catch (...) {
	}
#endif
	if (folder.empty()) {
		std::string message;
#ifdef _WIN32
		if (driverStatus == IpfDriverStatus::NOT_INSTALLED ||
			driverStatus == IpfDriverStatus::UNINSTALLED) {
			message = "Intel Platform Framework (IPF) is not installed. Please install the IPF "
					  "driver to continue.";
		}
		else {
			message = "Error Locating Framework Directory after " + std::to_string(attempts) +
					  " attempt(s)";
		}
#else
		message =
			"Error Locating Framework Directory after " + std::to_string(attempts) + " attempt(s)";
#endif
		throw IpfException(message);
	}
	return folder;
}

} // namespace Ipf