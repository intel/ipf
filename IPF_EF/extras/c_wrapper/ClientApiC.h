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

#ifdef __cplusplus
extern "C"
{
#endif

#include "Base.h"
#include "Events.h"
#include "stddef.h"
#include "stdint.h"

// Opaque handle to an IPF ClientApi instance.
typedef struct ipf_instance_s* ipf_handle_t;

// Export Symbols only when building ClientApi Dynamic Library
#if defined(ClientApi_EXPORTS)
#define API_IMPORT_EXPORT DLLEXPORT
#else
#if defined(ClientApi_IMPORTS)
#define API_IMPORT_EXPORT DLLIMPORT
#else
#define API_IMPORT_EXPORT
#endif
#endif

/* The C API for IPF ClientApi is for use only by C modules ending in ".c"
 * If the C++ Client ending in ".cpp" wants to use C API, please refer to "ABI-Agnostic C++ Application" in the User Guide
 */

typedef int BOOL;
#define TRUE 1
#define FALSE 0

/* Used for Value-To-String case statements */
#define IPF_CASE_ENUM(e)                                                                          \
	case e:                                                                                        \
		return (char*)#e

/*-----------------------------------------------------------------------------
  IPF errors enumeration
-----------------------------------------------------------------------------*/
enum IpfError {
	IPF_ERR_OK = 0,
	IPF_ERR_UNKNOWN,
	IPF_ERR_UNEXPECTED,
	IPF_ERR_MEMORY,
	IPF_ERR_INVALIDPARAM,
	IPF_ERR_CREATE,
	IPF_ERR_GETNODE,
	IPF_ERR_GETSCHEMA,
	IPF_ERR_QUERYNODE,
	IPF_ERR_EXECCMD,
	IPF_ERR_REGISTER,
	IPF_ERR_UNREGISTER,
	IPF_ERR_SETVALUE,
	IPF_ERR_GETVALUE,
	IPF_ERR_GETKEYVAL,
	IPF_ERR_GETKEYVALS,
	IPF_ERR_ENUMKEYS,
	IPF_ERR_ENUMKEYVALS,
	IPF_ERR_VALIDATE,
	IPF_ERR_UPDATEJSON,
	IPF_ERR_BEAUTIFYJSON,
	IPF_ERR_BUFFERTOOSMALL,
	IPF_ERR_JARRAYEXPECTED,
	IPF_ERR_GETCAPABILITIES,
	IPF_ERR_NOTFOUND,
};
typedef enum IpfError ipf_err_t;

/* Convert Return Code To A String */
static __inline char* ipf_ef_error_str(ipf_err_t type)
{
	switch (type) {
		IPF_CASE_ENUM(IPF_ERR_OK);
		IPF_CASE_ENUM(IPF_ERR_UNKNOWN);
		IPF_CASE_ENUM(IPF_ERR_UNEXPECTED);
		IPF_CASE_ENUM(IPF_ERR_MEMORY);
		IPF_CASE_ENUM(IPF_ERR_INVALIDPARAM);
		IPF_CASE_ENUM(IPF_ERR_CREATE);
		IPF_CASE_ENUM(IPF_ERR_GETNODE);
		IPF_CASE_ENUM(IPF_ERR_GETSCHEMA);
		IPF_CASE_ENUM(IPF_ERR_QUERYNODE);
		IPF_CASE_ENUM(IPF_ERR_EXECCMD);
		IPF_CASE_ENUM(IPF_ERR_REGISTER);
		IPF_CASE_ENUM(IPF_ERR_UNREGISTER);
		IPF_CASE_ENUM(IPF_ERR_SETVALUE);
		IPF_CASE_ENUM(IPF_ERR_GETVALUE);
		IPF_CASE_ENUM(IPF_ERR_GETKEYVAL);
		IPF_CASE_ENUM(IPF_ERR_GETKEYVALS);
		IPF_CASE_ENUM(IPF_ERR_ENUMKEYS);
		IPF_CASE_ENUM(IPF_ERR_ENUMKEYVALS);
		IPF_CASE_ENUM(IPF_ERR_VALIDATE);
		IPF_CASE_ENUM(IPF_ERR_UPDATEJSON);
		IPF_CASE_ENUM(IPF_ERR_BEAUTIFYJSON);
		IPF_CASE_ENUM(IPF_ERR_BUFFERTOOSMALL);
		IPF_CASE_ENUM(IPF_ERR_JARRAYEXPECTED);
		IPF_CASE_ENUM(IPF_ERR_GETCAPABILITIES);
		IPF_CASE_ENUM(IPF_ERR_NOTFOUND);
	}
	return (char*)"Unknown Error\n";
}

/*-----------------------------------------------------------------------------
  IPF schema types enumeration
	Note: The real SchemaType enum is defined as enum class in the Ipf
	namespace. These could get out of sync if the enum class ever changes.
-----------------------------------------------------------------------------*/
enum IpfSchemaType {
	IPF_SCHEMA_TYPE_NONE = 0,
	IPF_SCHEMA_TYPE_DATA = 1,
	IPF_SCHEMA_TYPE_EVENT = 2,
	IPF_SCHEMA_TYPE_COMMAND = 3,
	IPF_SCHEMA_TYPE_RESULT = 4,
	IPF_SCHEMA_TYPE_INIT = 5,
};
typedef enum IpfSchemaType ipf_schema_t;

/******************************************************************************
  IPF Client APIs
******************************************************************************/

/*-----------------------------------------------------------------------------
  Create an instance of the IPF ClientApi and get a handle to it
	config - IPF client configuration in json
	pIpf - pointer to receive the opaque handle to the IPF ClientApi instance
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfCreate(const char* config, ipf_handle_t* pIpf);

/*-----------------------------------------------------------------------------
  Destroy the IPF ClientApi instance created by IpfCreate(). 
  Do NOT reuse the handle after calling this function, as it may lead to undefined behavior.
	pIpf - handle to IPF ClientApi instance
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT void IpfDestroy(ipf_handle_t pIpf);

/*-----------------------------------------------------------------------------
  Get Properties
	pIpf - handle to IPF ClientApi instance
	properties_config - properties config
	buf - pointer to buffer where the returned json data will be copied into
	len - pointer to the buffer size, and the json data size on return.
		  If buffer is not large enough, len will be set to the required size.
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t
IpfGetProperties(ipf_handle_t pIpf, const char* properties_config, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get a node
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	buf - pointer to buffer where the returned json data will be copied into
	len - pointer to the buffer size, and the json data size on return.
		  If buffer is not large enough, len will be set to the required size.
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfGetNode(ipf_handle_t pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get a value
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired property
	buf - pointer to buffer where the returned json value will be copied into
	len - pointer to the buffer size, and the json value size on return.
		  If buffer is not large enough, len will be set to the required size.
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfGetValue(ipf_handle_t pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get schema
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	schema_type - schema type
	buf - pointer to buffer where the returned json schema will be copied into
	len - pointer to the buffer size, and the json schema size on return.
		  If buffer is not large enough, len will be set to the required size.
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t
IpfGetSchema(ipf_handle_t pIpf, const char* path, ipf_schema_t schema_type, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Query a node including its properties and child nodes
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	buf - pointer to buffer where the returned json data will be copied into
	len - pointer to the buffer size, and the json data size on return.
		  If buffer is not large enough, len will be set to the required size.
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfQueryNode(ipf_handle_t pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Execute a command on a node
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	command - command in json
	buf - pointer to buffer where the returned json result will be copied into
	len - pointer to the buffer size, and the json result size on return.
		  If buffer is not large enough, len will be set to the required size.
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t
IpfExecuteCommand(ipf_handle_t pIpf, const char* path, const char* command, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Register for data events from provider
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	cb - event callback function
	context - context (user data) for the callback function
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfRegisterEvent(ipf_handle_t pIpf, const char* path, EventCallback cb, void* context);

/*-----------------------------------------------------------------------------
  Register for data events from provider with generic event settings
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	cb - event callback function
	context - context (user data) for the callback function
	event_settings - char* containing json encoded event settings

	Added in client interface revision 5
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfRegisterEventWithSettings(
	ipf_handle_t pIpf, const char* path, EventCallback cb, void* context, const char* event_settings);

/*-----------------------------------------------------------------------------
  Unregister for data events from provider
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node
	cb - event callback function
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfUnregisterEvent(ipf_handle_t pIpf, const char* path, EventCallback cb);

/*-----------------------------------------------------------------------------
  Set a boolean value to a node property
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node property
	value - a boolean value
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfSetValue_bool(ipf_handle_t pIpf, const char* path, BOOL value);

/*-----------------------------------------------------------------------------
  Set an integer value to a node property
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node property
	value - an integer value
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfSetValue_int(ipf_handle_t pIpf, const char* path, int64_t value);

/*-----------------------------------------------------------------------------
  Set an unsigned integer value to a node property
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node property
	value - an unsigned integer value
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfSetValue_uint(ipf_handle_t pIpf, const char* path, uint64_t value);

/*-----------------------------------------------------------------------------
  Set a double value to a node property
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node property
	value - a float value
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfSetValue_double(ipf_handle_t pIpf, const char* path, double value);

/*-----------------------------------------------------------------------------
  Set a string value to a node property
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node property
	value - a string value
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfSetValue_string(ipf_handle_t pIpf, const char* path, const char* value);

/*-----------------------------------------------------------------------------
  Get capabilities on a node
	pIpf - handle to IPF ClientApi instance
	path - the path to the desired node property
	value - a string value
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT ipf_err_t IpfGetCapabilities(ipf_handle_t pIpf, const char* path, char* buf, size_t* len);

/*-----------------------------------------------------------------------------
  Get IPF last error message (thread-safe)
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT const char* IpfGetLastErrorMessage();

/*-----------------------------------------------------------------------------
  Get IPF last exception type (thread-safe)
-----------------------------------------------------------------------------*/
API_IMPORT_EXPORT const char* IpfGetLastExceptionType();

#ifdef __cplusplus
}
#endif