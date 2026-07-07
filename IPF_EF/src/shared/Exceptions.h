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

#include "nlohmann/json.hpp"
#include <stdexcept>
#include <string>
#ifdef IPF_TRACE_AVAILABLE
#include "Tracing.h"
#else
#define IPF_TRACE_DEBUG(fmt, ...) (void)0
#endif

namespace Ipf
{
// Exception codes per exception type
typedef int ipf_exception_code_t;
constexpr ipf_exception_code_t IpfExceptionCode = 1;
constexpr ipf_exception_code_t FrameworkExceptionCode = 100;
constexpr ipf_exception_code_t LimitExceededExceptionCode = 200;
constexpr ipf_exception_code_t TimeoutExceptionCode = 300;
constexpr ipf_exception_code_t InvalidArgumentExceptionCode = 400;
constexpr ipf_exception_code_t InvalidDeleteExceptionCode = 401;
constexpr ipf_exception_code_t AccessDeniedExceptionCode = 500;
constexpr ipf_exception_code_t JsonExceptionCode = 600;
constexpr ipf_exception_code_t ProviderNotFoundCode = 700;
constexpr ipf_exception_code_t NotImplementedCode = 800;
constexpr ipf_exception_code_t ProviderExceptionCode = 50000;

class IpfException : public std::runtime_error
{
private:
	int err = -1;
	static constexpr char DEFAULT_IPF_EXCEPTION[] = "Unknown IPF exception";

public:
	IpfException() : std::runtime_error{std::string(DEFAULT_IPF_EXCEPTION)}
	{}
	explicit IpfException(const std::string& what_msg, const int ec = IpfExceptionCode)
		: std::runtime_error{what_msg}, err{ec}
	{
		IPF_TRACE_DEBUG("%s", what_msg.c_str());
	}
	int errcode() const
	{
		return err;
	}
};

class FrameworkException : public IpfException
{
public:
	static constexpr char DEFAULT_FRAMEWORK_EXCEPTION[] = "Unknown framework exception";
	static constexpr char INVALID_PROVIDER_LOCATION[] = "Invalid provider location: ";
	static constexpr char INVALID_CATALOG_LOCATION[] = "Invalid catalog file location";
	static constexpr char DUPLICATE_PROVIDER_NODE[] = "Duplicate namespace node: ";
	static constexpr char DUPLICATE_CHILD_PROVIDER[] = "Duplicate provider instance: ";
	static constexpr char DUPLICATE_REGISTRATION[] = "Duplicate registration for provider: ";
	static constexpr char FRAMEWORK_BUG[] = "Framework internal error: ";
	static constexpr char UNKNOWN_PROVIDER_EXCEPTION[] = "Unknown exception from provider: ";
	static constexpr char LIB_LOAD[] = "Failed to load library: ";
	static constexpr char OBJECT_ERROR[] = "Failed to create instance of ";
	static constexpr char BUFFER_ALLOC_FAILURE[] = "Failed to allocate memory for ";
	static constexpr char INVALID_ESIF_DATATYPE[] = "Invalid ESIF data type: ";
	static constexpr char INVALID_DATA_LENGTH[] = "Invalid data length in ";
	static constexpr char GET_MANIFEST_EXCEPTION[] = "Failed to get manifest from provider: ";
	static constexpr char CATALOG_MISSING[] = "Provider catalog is missing";
	static constexpr char CATALOG_READ_FAILURE[] = "Failed to open catalog for read operation";
	static constexpr char CATALOG_WRITE_FAILURE[] = "Failed to open catalog for write operation";
	static constexpr char CATALOG_PARSE_FAILURE[] = "Failed to parse catalog";
	static constexpr char PROVIDER_METADATA_ERROR[] = "Failed to parse provider metadata";
	static constexpr char MISSING_PROGRAM_FILES[] = "Failed to locate program files path";
	static constexpr char PROVIDER_UNREGISTER_MISSING_RECORD[] =
		"Failed to unregister provider package. Missing catalog entry: ";
	static constexpr char PROVIDER_UNREGISTER_MISSING_CATALOG[] =
		"Failed to unregister provider package. Missing catalog";
	static constexpr char INF_DISCOVERY_BUFFER_FAILURE[] =
		"Failed to discover providers installed using INF method. Failed to allocate buffer for ";
	static constexpr char INF_DISCOVERY_DEVLIST_FAILURE[] =
		"Failed to discover providers installed using INF method. Failed to get list of active "
		"devices";
	static constexpr char UNEXPECTED_PROVIDER_THREAD_TERMINATION[] =
		"Unexpected termination of provider thread: ";
	static constexpr char REMOVE_TEMP_FILE_FAILURE[] = "Failed to remove temp file";
	static constexpr char CIRCULAR_PATH_REFERENCE_FAILURE[] =
		"Detected a circular path reference! Path trace: ";
	static constexpr char PROVIDER_HOST_UNAVAILABLE[] =
		"Provider Host not available or connection lost"; // When the Provider Host dies during a
														  // call (or it isn't detected until a call
														  // is made)
	static constexpr char PROVIDER_HOST_DID_NOT_START[] =
		"Provider Host did not start in time"; // Provider Host recovery timeout
	static constexpr char UNKNOWN_EVENT[] = "Event is unknown in Framework event schema";

	FrameworkException() : IpfException{std::string(DEFAULT_FRAMEWORK_EXCEPTION)}
	{}
	explicit FrameworkException(
		const std::string& what_msg, const int err_code = FrameworkExceptionCode)
		: IpfException{what_msg, err_code}
	{}
};

class ProviderException : public IpfException
{
public:
	static constexpr char DEFAULT_PROVIDER_EXCEPTION[] = "Unknown provider Exception";
	static constexpr char UNSUPPORTED_PROVIDER_METHOD[] = "Unsupported method by provider: ";
	static constexpr char EVENT_NOT_REGISTERED[] = "Event was not registered: ";
	static constexpr char EVENT_CALLBACK_NOT_REGISTERED[] = "Callback was not registered for event: ";
	static constexpr char UNKNOWN_EVENT[] = "Event is unknown in the provider event schema: ";

	ProviderException() : ProviderException{std::string(DEFAULT_PROVIDER_EXCEPTION)}
	{}
	explicit ProviderException(
		const std::string& what_msg, const int err_code = ProviderExceptionCode)
		: IpfException{what_msg, err_code}
	{}
};

class LimitExceededException : public FrameworkException
{
public:
	static constexpr char DEFAULT_LIMIT_EXCEPTION[] = "Unknown limit exception";
	static constexpr char Q_OVERFLOW[] = "Exceeded maximum queue size for ";
	static constexpr char DEPTH_OVERFLOW[] = "Exceeded maximum provider tree depth of ";
	static constexpr char VISITED_PATHS_OVERFLOW[] = "Exceeded maximum depth of visited paths ";
	static constexpr char PROVIDER_TIMEOUT_THREADS_OVERFLOW[] =
		"Exceeded maximum timedout threads limit for ";
	static constexpr char PROVIDER_ACTIVE_THREADS_OVERFLOW[] =
		"Exceeded maximum active threads limit for ";
	static constexpr char BREADTH_OVERFLOW[] = "Exceeded maximum number of provider siblings of ";

	LimitExceededException() : LimitExceededException{std::string(DEFAULT_LIMIT_EXCEPTION)}
	{}
	explicit LimitExceededException(
		const std::string& what_msg, const int err_code = LimitExceededExceptionCode)
		: FrameworkException{what_msg, err_code}
	{}
};

class TimeoutException : public FrameworkException
{
public:
	static constexpr char DEFAULT_TIMEOUT_EXCEPTION[] = "Unknown timeout exception";
	static constexpr char TIMEOUT[] = "Timed out waiting for ";

	TimeoutException() : TimeoutException{std::string(DEFAULT_TIMEOUT_EXCEPTION)}
	{}
	explicit TimeoutException(
		const std::string& what_msg, const int err_code = TimeoutExceptionCode)
		: FrameworkException{what_msg, err_code}
	{}
};

class InvalidArgumentException : public FrameworkException
{
public:
	static constexpr char DEFAULT_ARGUMENT_EXCEPTION[] = "Unknown argument exception";
	static constexpr char INVALID_PATH[] = "Invalid path: ";
	static constexpr char INVALID_CLIENT_CONFIG[] = "Invalid configuration in client application: ";
	static constexpr char INVALID_ATTACH[] = "Invalid attachment instructions: ";
	static constexpr char INVALID_SCHEMA[] = "Invalid schema ";
	static constexpr char INVALID_DELETE[] =
		"Attempting deleting object multiple times or deleting invalid object: ";
	static constexpr char INVALID_EVENT_SETTINGS[] = "Invalid event settings";
	static constexpr char INVALID_CAPABILITIES[] = "Invalid capabilities";
	static constexpr char INVALID_EVENT_CALLBACK[] = "Event callback is null";

	InvalidArgumentException() : InvalidArgumentException{std::string(DEFAULT_ARGUMENT_EXCEPTION)}
	{}
	explicit InvalidArgumentException(
		const std::string& what_msg, const int err_code = InvalidArgumentExceptionCode)
		: FrameworkException{what_msg, err_code}
	{}
};

class AccessDeniedException : public FrameworkException
{
public:
	static constexpr char DEFAULT_ACCESS_EXCEPTION[] = "Unknown access exception";
	AccessDeniedException() : AccessDeniedException{std::string(DEFAULT_ACCESS_EXCEPTION)}
	{}
	explicit AccessDeniedException(
		const std::string& what_msg, const int err_code = AccessDeniedExceptionCode)
		: FrameworkException{what_msg, err_code}
	{}
};

class JsonException : public FrameworkException
{
public:
	static constexpr char DEFAULT_JSON_EXCEPTION[] = "Unknown JSON exception";
	static constexpr char INVALID_JSON[] = "Invalid JSON: ";
	static constexpr char INVALID_JSON_SCHEMA[] = "Invalid JSON schema: ";
	static constexpr char INVALID_JSON_SCHEMA_TYPE[] = "Invalid JSON schema type: ";
	static constexpr char INVALID_JSON_SCHEMA_REVISION[] = "Invalid JSON schema revision: ";
	static constexpr char SCHEMA_MISMATCH[] = "Failed to validate against JSON schema: ";

	JsonException() : JsonException{std::string(DEFAULT_JSON_EXCEPTION)}
	{}
	explicit JsonException(const std::string& what_msg, const int err_code = JsonExceptionCode)
		: FrameworkException{what_msg, err_code}
	{}
};

class ProviderNotFoundException : public FrameworkException
{
public:
	static constexpr char DEFAULT_PROVIDER_NOT_FOUND_EXCEPTION[] =
		"Unknown provider not found exception";
	static constexpr char PROVIDER_NOT_FOUND[] = "Provider not found: ";
	ProviderNotFoundException()
		: ProviderNotFoundException{std::string(DEFAULT_PROVIDER_NOT_FOUND_EXCEPTION)}
	{}
	explicit ProviderNotFoundException(
		const std::string& what_msg, const int err_code = ProviderNotFoundCode)
		: FrameworkException{what_msg, err_code}
	{}
};

class NotImplementedException : public FrameworkException
{
public:
	static constexpr char NOT_IMPLEMENTED_EXCEPTION[] = "Method is not implemented";
	NotImplementedException() : NotImplementedException{std::string(NOT_IMPLEMENTED_EXCEPTION)}
	{}
	explicit NotImplementedException(
		const std::string& what_msg, const int err_code = NotImplementedCode)
		: FrameworkException{what_msg, err_code}
	{}
};

// Error in strict case where a static function is compiled out
#if defined(_WIN32)
#pragma warning(push)
#pragma warning(disable : 4505)
#endif

static inline void RethrowException()
{
	// Catch and Rethrow a copy of exceptions
	try {
		throw;
	}
	catch (const JsonException& e) {
		IPF_TRACE_DEBUG("JsonException: %s\n", e.what());
		throw e;
	}
	catch (const ProviderNotFoundException& e) {
		IPF_TRACE_DEBUG("ProviderNotFoundException: %s\n", e.what());
		throw e;
	}
	catch (const AccessDeniedException& e) {
		IPF_TRACE_DEBUG("AccessDeniedException: %s\n", e.what());
		throw e;
	}
	catch (const InvalidArgumentException& e) {
		IPF_TRACE_DEBUG("InvalidArgumentException: %s\n", e.what());
		throw e;
	}
	catch (const TimeoutException& e) {
		IPF_TRACE_DEBUG("TimeoutException: %s\n", e.what());
		throw e;
	}
	catch (const LimitExceededException& e) {
		IPF_TRACE_DEBUG("LimitExceededException: %s\n", e.what());
		throw e;
	}
	catch (const FrameworkException& e) {
		IPF_TRACE_DEBUG("FrameworkException: %s\n", e.what());
		throw e;
	}
	catch (const ProviderException& e) {
		IPF_TRACE_DEBUG("ProviderException: %s\n", e.what());
		throw e;
	}
	catch (const IpfException& e) {
		IPF_TRACE_DEBUG("IpfException: %s\n", e.what());
		throw e;
	}
	catch (const nlohmann::json::exception& e) {
		IPF_TRACE_DEBUG("json::exception: %s\n", e.what());
		throw JsonException(e.what());
	}
	catch (const std::logic_error& e) {
		IPF_TRACE_DEBUG("logic_error: %s\n", e.what());
		throw e;
	}
	catch (const std::runtime_error& e) {
		IPF_TRACE_DEBUG("runtime_error: %s\n", e.what());
		throw e;
	}
	catch (const std::exception& e) {
		IPF_TRACE_DEBUG("std::exception: %s\n", e.what());
		throw e;
	}
	catch (...) {
		throw std::runtime_error("Unknown Exception");
	}
}

#if defined(_WIN32)
#pragma warning(pop)
#endif

} // namespace Ipf
