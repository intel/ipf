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

#include "ClientApi.h"
#include "HandleGenerator.h"
#include "OsAbstractLayer.h"
#include "Path.h"

#include <map>
#include <memory>
#include <mutex>

extern "C"
{
#include "ClientApiC.h"
#include "ClientApiCShared.h"
}

thread_local char LastError[MAX_ERROR_MESSAGE_LENGTH];
thread_local char LastExceptionType[MAX_EXCEPTION_TYPE_LENGTH];

// Make sure (unsupported) 32-bit builds surface this error.
static_assert(sizeof(ipf_handle_t) >= sizeof(esif_handle_t), "Opaque handle requires 64-bit pointers");

// Registry mapping opaque handles to ClientApi instances to prevent use-after-free.
// Ownership: IpfCreate inserts instances; IpfDestroy must be called for deterministic
// cleanup (deletes the instance and removes it from the map). Process-exit cleanup is
// intentionally skipped — callers are responsible for calling IpfDestroy before exit.
static std::mutex g_registryMutex;
static std::map<ipf_handle_t, Ipf::ClientApi*> g_instanceMap;

static Ipf::ClientApi* IpfGetInstance(ipf_handle_t pIpf)
{
	std::lock_guard<std::mutex> lock(g_registryMutex);
	auto it = g_instanceMap.find(pIpf);
	return (it != g_instanceMap.end()) ? it->second : nullptr;
}

ipf_err_t IpfCreate(const char* config, ipf_handle_t* pIpf)
{
	constexpr esif_handle_t API_HANDLE_MIN = 0x0000000001000000;
	// Set max so that MAX + INC will always be below ESIF_INVALID_HANDLE to avoid any confusion
	constexpr esif_handle_t API_HANDLE_MAX = 0xfffffffffffe0000;
	constexpr esif_handle_t API_HANDLE_SEED = 0x0000000000ffffff;
	constexpr esif_handle_t API_HANDLE_INC = 0x000000000000ffff;

	// Guarantee handle generation parameters are valid at compile time, since we can't recover if
	// they're not
	static_assert(Ipf::HandleGenerator::ValidateParameters(
		API_HANDLE_MIN, API_HANDLE_MAX, API_HANDLE_SEED, API_HANDLE_INC));

	static Ipf::HandleGenerator handleGenerator(
		API_HANDLE_MIN, API_HANDLE_MAX, API_HANDLE_SEED, API_HANDLE_INC);

	if (!pIpf) {
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "pIpf is null");
		return IpfError::IPF_ERR_INVALIDPARAM;
	}

	*pIpf = NULL;
	ipf_err_t ret = IpfError::IPF_ERR_OK;
	Ipf::ClientApi* instance = nullptr;

	BEGIN_EXCEPTIONS_HANDLING

	if (config != nullptr && ipfef_osal_strnlen_s(config, sizeof(*config)) > 0) {
		instance = new Ipf::ClientApi(config);
	}
	else {
		instance = new Ipf::ClientApi();
	}

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_CREATE)

	// Failed to create instance, so we can stop here.
	if (ret != IpfError::IPF_ERR_OK || !instance) {
		return ret;
	}

	// Register instance in the global registry and generate opaque handle for it
	try {
		std::lock_guard<std::mutex> lock(g_registryMutex);

		esif_handle_t handle = ESIF_INVALID_HANDLE;
		do {
			handle = handleGenerator.Generate();

			// Verify handle is not already in use - if it is, regenerate it.
			// This is very unlikely, but we need to guarantee unique handles.
			if (g_instanceMap.find(reinterpret_cast<ipf_handle_t>(handle)) != g_instanceMap.end()) {
				handle = ESIF_INVALID_HANDLE;
			}
		} while (handle == ESIF_INVALID_HANDLE);

		// Cast handle to ipf_handle_t for the opaque handle and insert instance into the registry
		ipf_handle_t opaque_handle = reinterpret_cast<ipf_handle_t>(handle);
		g_instanceMap[opaque_handle] = instance;
		*pIpf = opaque_handle;
	}
	catch (...) {
		// Failed to register instance, so clean up and return error
		// This will basically never happen, but we need to cover it because we can't throw.
		delete instance;
		instance = nullptr;
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "Failed to register instance");
		ret = IpfError::IPF_ERR_CREATE;
	}

	return ret;
}

void IpfDestroy(ipf_handle_t pIpf)
{

	if (pIpf) {
		std::lock_guard<std::mutex> lock(g_registryMutex);
		auto it = g_instanceMap.find(pIpf);
		if (it != g_instanceMap.end()) {
			delete it->second;
			g_instanceMap.erase(it);
		}
	}
}

ipf_err_t IpfGetProperties(ipf_handle_t pIpf, const char* properties_config, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->GetProperties(properties_config ? properties_config : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETNODE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t IpfGetNode(ipf_handle_t pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->GetNode(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETNODE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t IpfGetValue(ipf_handle_t pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->GetValue(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t
IpfGetSchema(ipf_handle_t pIpf, const char* path, ipf_schema_t schema_type, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	// get the node
	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->GetSchema((path ? path : ""), (Ipf::SchemaType)schema_type);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETSCHEMA)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t IpfQueryNode(ipf_handle_t pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	// Query the node
	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->QueryNode(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_QUERYNODE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t
IpfExecuteCommand(ipf_handle_t pIpf, const char* path, const char* command, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_COMMAND
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (len != NULL && *len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->ExecuteCommand(path ? path : "", command);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_EXECCMD)

	if (len != NULL) {
		UPDATE_BUFFER_LENGTH_COPY_BUFFER
	}

	return ret;
}

ipf_err_t IpfRegisterEvent(ipf_handle_t pIpf, const char* path, EventCallback cb, void* context)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_CALLBACK

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->RegisterEvent(path, cb, context);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_REGISTER)

	return ret;
}

// Added in interface revision 5 to support generic events.
// Clients may specify event settings for sampling interval or thresholds
ipf_err_t IpfRegisterEventWithSettings(
	ipf_handle_t pIpf, const char* path, EventCallback cb, void* context, const char* event_settings)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_CALLBACK

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->RegisterEvent(path, cb, context, event_settings);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_REGISTER)

	return ret;
}

ipf_err_t IpfUnregisterEvent(ipf_handle_t pIpf, const char* path, EventCallback cb)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_CALLBACK

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->UnregisterEvent(path, cb);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_UNREGISTER)

	return ret;
}

ipf_err_t IpfSetValue_bool(ipf_handle_t pIpf, const char* path, BOOL value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	bool b = (value != FALSE);
	ipf_instance->SetValue(path ? path : "", b);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_int(ipf_handle_t pIpf, const char* path, int64_t value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->SetValue(path ? path : "", value);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_uint(ipf_handle_t pIpf, const char* path, uint64_t value)
{
	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->SetValue(path ? path : "", static_cast<int64_t>(value));

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_double(ipf_handle_t pIpf, const char* path, double value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->SetValue(path ? path : "", value);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_string(ipf_handle_t pIpf, const char* path, const char* value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	ipf_instance->SetValue(path ? path : "", value);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfGetCapabilities(ipf_handle_t pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = ipf_instance->GetCapabilities(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETCAPABILITIES)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

const char* IpfGetLastErrorMessage()
{
	return LastError;
}

const char* IpfGetLastExceptionType()
{
	return LastExceptionType;
}
