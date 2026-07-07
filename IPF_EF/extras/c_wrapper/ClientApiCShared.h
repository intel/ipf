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

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(*(a)))

#define MAX_VALUE_BUFFER_CHARS 256
#define MAX_ERROR_MESSAGE_LENGTH 2048
#define MAX_EXCEPTION_TYPE_LENGTH 256

#define UNKNOWN_TYPE "unknown"

#ifdef _WIN32
#define snprintf(str, siz, fmt, ...) _snprintf_s(str, siz, _TRUNCATE, fmt, ##__VA_ARGS__)
#endif

/* API Implmentation Helper Macros */

#define BEGIN_EXCEPTIONS_HANDLING try {

#define END_EXCEPTIONS_HANDLING(err)                                                               \
	}                                                                                              \
	catch (Ipf::IpfException & ex)                                                                 \
	{                                                                                              \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "%s", ex.what());                         \
		(void)snprintf(LastExceptionType, ARRAY_SIZE(LastExceptionType), "%s", typeid(ex).name()); \
		ret = (err);                                                                               \
	}                                                                                              \
	catch (std::exception & ex)                                                                    \
	{                                                                                              \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "%s", ex.what());                         \
		(void)snprintf(LastExceptionType, ARRAY_SIZE(LastExceptionType), "%s", typeid(ex).name()); \
		ret = IpfError::IPF_ERR_UNKNOWN;                                                           \
	}                                                                                              \
	catch (...)                                                                                    \
	{                                                                                              \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "unexpected error");                      \
		(void)snprintf(LastExceptionType, ARRAY_SIZE(LastExceptionType), "%s", UNKNOWN_TYPE);      \
		ret = IpfError::IPF_ERR_UNEXPECTED;                                                        \
	}

#define CHECK_PARAM_IPF                                                                            \
	if (!pIpf) {                                                                                   \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "pIpf is null");                          \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_BUFFERLEN                                                                      \
	if (!len) {                                                                                    \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "len is null");                           \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_BUFFER                                                                         \
	if (!buf) {                                                                                    \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "buf is null");                           \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_KEY                                                                            \
	if (!std::string(key).size()) {                                                                \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "key is null or empty");                  \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_KEYS                                                                           \
	if (!keys) {                                                                                   \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "keys is null");                          \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_COMMAND                                                                        \
	if (!std::string(command).size()) {                                                            \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "command is null or empty");              \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_CALLBACK                                                                       \
	if (!cb) {                                                                                     \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "callback function is null");             \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_SCHEMA                                                                         \
	if (!std::string(schema).size()) {                                                             \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "schema is null or empty");               \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define DECLARE_RESULT_STRING_BUFFER_LENGTH                                                        \
	std::string result;                                                                            \
	size_t buffer_length = 0;

#define STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER                                                    \
	if (ret == IpfError::IPF_ERR_OK) {                                                             \
		buffer_length = str.size() + 1;                                                            \
		size_t current_buffer_len = *len;                                                          \
		*len = buffer_length;                                                                      \
		if (!buf || (current_buffer_len < buffer_length)) {                                        \
			return IpfError::IPF_ERR_BUFFERTOOSMALL;                                               \
		}                                                                                          \
		(void)snprintf(buf, *len, "%s", str.c_str());                                              \
	}

#define UPDATE_BUFFER_LENGTH_COPY_BUFFER                                                           \
	if (ret == IpfError::IPF_ERR_OK) {                                                             \
		auto str = std::move(result);                                                              \
		STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER                                                    \
	}

