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
#include "OsAbstractLayer.h"
#include "Path.h"

extern "C"
{
#include "ClientApiC.h"
#include "ClientApiCShared.h"
}

thread_local char LastError[MAX_ERROR_MESSAGE_LENGTH];
thread_local char LastExceptionType[MAX_EXCEPTION_TYPE_LENGTH];

ipf_err_t IpfCreate(const char* config, void** pIpf)
{

	CHECK_PARAM_IPF

	*pIpf = NULL;
	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	if (config != nullptr && ipfef_osal_strnlen_s(config, sizeof(*config)) > 0) {
		*pIpf = new Ipf::ClientApi(config);
	}
	else {
		*pIpf = new Ipf::ClientApi();
	}

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_CREATE)

	return ret;
}

void IpfDestroy(void* pIpf)
{

	if (pIpf) {
		delete (Ipf::ClientApi*)pIpf;
	}
}

ipf_err_t IpfGetProperties(void* pIpf, const char* properties_config, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->GetProperties(
		properties_config ? properties_config : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETNODE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t IpfGetNode(void* pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->GetNode(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETNODE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t IpfGetValue(void* pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->GetValue(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t
IpfGetSchema(void* pIpf, const char* path, ipf_schema_t schema_type, char* buf, size_t* len)
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

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->GetSchema(
		(path ? path : ""), (Ipf::SchemaType)schema_type);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETSCHEMA)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t IpfQueryNode(void* pIpf, const char* path, char* buf, size_t* len)
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

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->QueryNode(path ? path : "");

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_QUERYNODE)

	UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t
IpfExecuteCommand(void* pIpf, const char* path, const char* command, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_COMMAND
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (len != NULL && *len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->ExecuteCommand(path ? path : "", command);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_EXECCMD)

	if (len != NULL) {
		UPDATE_BUFFER_LENGTH_COPY_BUFFER
	}

	return ret;
}

ipf_err_t IpfRegisterEvent(void* pIpf, const char* path, EventCallback cb, void* context)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_CALLBACK

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->RegisterEvent(path, cb, context);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_REGISTER)

	return ret;
}

// Added in interface revision 5 to support generic events.
// Clients may specify event settings for sampling interval or thresholds
ipf_err_t IpfRegisterEventWithSettings(
	void* pIpf, const char* path, EventCallback cb, void* context, const char* event_settings)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_CALLBACK

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->RegisterEvent(path, cb, context, event_settings);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_REGISTER)

	return ret;
}

ipf_err_t IpfUnregisterEvent(void* pIpf, const char* path, EventCallback cb)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_CALLBACK

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->UnregisterEvent(path, cb);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_UNREGISTER)

	return ret;
}

ipf_err_t IpfSetValue_bool(void* pIpf, const char* path, BOOL value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	bool b = (value != FALSE);
	reinterpret_cast<Ipf::ClientApi*>(pIpf)->SetValue(path ? path : "", b);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_int(void* pIpf, const char* path, int64_t value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->SetValue(path ? path : "", value);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_uint(void* pIpf, const char* path, uint64_t value)
{
	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->SetValue(
		path ? path : "", static_cast<int64_t>(value));

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_double(void* pIpf, const char* path, double value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->SetValue(path ? path : "", value);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfSetValue_string(void* pIpf, const char* path, const char* value)
{

	CHECK_PARAM_IPF

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	reinterpret_cast<Ipf::ClientApi*>(pIpf)->SetValue(path ? path : "", value);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_SETVALUE)

	return ret;
}

ipf_err_t IpfGetCapabilities(void* pIpf, const char* path, char* buf, size_t* len)
{

	CHECK_PARAM_IPF
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_STRING_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = reinterpret_cast<Ipf::ClientApi*>(pIpf)->GetCapabilities(path ? path : "");

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
