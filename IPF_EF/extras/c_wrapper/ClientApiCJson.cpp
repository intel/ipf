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

extern "C"
{
#include "ClientApiCJson.h"
}

#include "ClientApiC.cpp" // done intentionally to preserve 'thread_local' of LastError

// File for C-API helper functions
using Json = nlohmann::json;

/* JSON API Implementation Helper Macros */

#define CHECK_PARAM_JSON                                                                           \
	if (!std::string(json).size()) {                                                               \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "json is null or empty");                 \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_JSON2                                                                          \
	if (!std::string(json2).size()) {                                                              \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "json2 is null or empty");                \
		return IpfError::IPF_ERR_INVALIDPARAM;                                                     \
	}

#define CHECK_PARAM_JSONARRAY(a)                                                                   \
	if (!(a).is_array()) {                                                                         \
		(void)snprintf(LastError, ARRAY_SIZE(LastError), "json array expected");                   \
		return IpfError::IPF_ERR_JARRAYEXPECTED;                                                   \
	}

#define DECLARE_RESULT_JSON_BUFFER_LENGTH                                                          \
	Json result;                                                                                   \
	size_t buffer_length = 0;

/* Public JSON C API Functions */

BOOL JsonVerify(const char* json)
{
	return json != nullptr && !std::string(json).empty() &&
		   !Json::parse(json, nullptr, false).is_discarded();
}

size_t JsonGetElementsCount(const char* json)
{
	size_t count = 0;
	if (JsonVerify(json)) {
		auto result = Json::parse(json);
		count = result.size();
	}
	return count;
}

BOOL JsonContainsKey(const char* json, const char* key)
{

	if (std::string(json).empty() || std::string(key).empty()) {
		return FALSE;
	}

	auto result = Json::parse(json, nullptr, false);
	if (result.is_discarded()) {
		return FALSE;
	}

	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (result.find(k) == result.end()) {
			return FALSE;
		}
		result = result[k];
	}

	return TRUE;
}

ipf_err_t JsonGetKeyValue(const char* json, const char* key, char* buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_KEY
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;
	std::string str;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (result.find(k) == result.end()) {
			return IpfError::IPF_ERR_NOTFOUND;
		}
		result = result[k];
	}
	str = result.dump();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETKEYVAL)

	STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t JsonGetKeyValue_bool(const char* json, const char* key, BOOL* buf)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_KEY
	CHECK_PARAM_BUFFER

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	Json result = Json::parse(json);
	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (result.find(k) == result.end()) {
			return IpfError::IPF_ERR_NOTFOUND;
		}
		result = result[k];
	}
	*buf = result.get<bool>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	return ret;
}

ipf_err_t JsonGetValue_bool(const char* json, BOOL* buf)
{

	CHECK_PARAM_BUFFER
	CHECK_PARAM_JSON

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	Json result = Json::parse(json);
	*buf = result.get<bool>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	return ret;
}

ipf_err_t JsonGetKeyValue_int(const char* json, const char* key, int* buf)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_KEY
	CHECK_PARAM_BUFFER

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	Json result = Json::parse(json);
	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (result.find(k) == result.end()) {
			return IpfError::IPF_ERR_NOTFOUND;
		}
		result = result[k];
	}
	*buf = result.get<int>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	return ret;
}

ipf_err_t JsonGetValue_int(const char* json, int* buf)
{

	CHECK_PARAM_BUFFER
	CHECK_PARAM_JSON

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	Json result = Json::parse(json);
	*buf = result.get<int>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	return ret;
}

ipf_err_t JsonGetKeyValue_double(const char* json, const char* key, double* buf)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_KEY
	CHECK_PARAM_BUFFER

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	Json result = Json::parse(json);
	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (result.find(k) == result.end()) {
			return IpfError::IPF_ERR_NOTFOUND;
		}
		result = result[k];
	}
	*buf = result.get<double>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	return ret;
}

ipf_err_t JsonGetValue_double(const char* json, double* buf)
{

	CHECK_PARAM_BUFFER
	CHECK_PARAM_JSON

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	Json result = Json::parse(json);
	*buf = result.get<double>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	return ret;
}

ipf_err_t JsonGetKeyValue_string(const char* json, const char* key, char* buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_KEY
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;
	std::string str;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (result.find(k) == result.end()) {
			return IpfError::IPF_ERR_NOTFOUND;
		}
		result = result[k];
	}
	str = result.get<std::string>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t JsonGetValue_string(const char* json, char* buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;
	std::string str;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	str = result.get<std::string>();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETVALUE)

	STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t JsonEnumerateKeys(const char* json, char** keys, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_KEYS
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	buffer_length = result.size();
	size_t current_buffer_len = *len;
	*len = buffer_length;
	if (!keys || (current_buffer_len < buffer_length)) {
		return IpfError::IPF_ERR_BUFFERTOOSMALL;
	}
	size_t i = 0;
	for (auto& e : result.items()) {
		(void)snprintf(keys[i++], MAX_VALUE_BUFFER_CHARS, "%s", e.key().c_str());
	}

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_ENUMKEYS)

	return ret;
}

ipf_err_t JsonEnumerateKeyValues(const char* json, char** keys, char** buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_KEYS
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	buffer_length = result.size();
	size_t current_buffer_len = *len;
	*len = buffer_length;
	if (!keys || (current_buffer_len < buffer_length)) {
		return IpfError::IPF_ERR_BUFFERTOOSMALL;
	}
	size_t i = 0;
	for (auto& e : result.items()) {
		(void)snprintf(keys[i], MAX_VALUE_BUFFER_CHARS, "%s", e.key().c_str());
		(void)snprintf(buf[i], MAX_VALUE_BUFFER_CHARS, "%s", e.value().dump().c_str());
		i++;
	}

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_ENUMKEYVALS)

	return ret;
}

ipf_err_t
JsonGetArrayKeyValues(const char* json, size_t count, const char* keys[], char*** buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_KEYS
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	CHECK_PARAM_JSONARRAY(result)
	buffer_length = result.size();
	size_t current_buffer_len = *len;
	*len = buffer_length;
	int base = 10;
	char* ptr;
	if (!buf || (current_buffer_len < buffer_length)) {
		return IpfError::IPF_ERR_BUFFERTOOSMALL;
	}
	for (auto& e : result.items()) {
		auto i = strtol(e.key().c_str(), &ptr, base);
		assert(i < *len);
		if (i >= *len) {
			return IpfError::IPF_ERR_UNEXPECTED;
		}
		for (size_t j = 0; j < count; j++) {
			auto value = e.value()[keys[j]].dump();
			(void)snprintf(buf[i][j], MAX_VALUE_BUFFER_CHARS, "%s", value.c_str());
		}
	}

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_GETKEYVALS)

	return ret;
}

ipf_err_t JsonUpdate(const char* json, const char* key, const char* json2, char* buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_JSON2
	CHECK_PARAM_KEY
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;
	std::string str;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	Json* pResult = &result;
	auto keys = Ipf::SplitPath(key);
	for (auto& k : keys) {
		if (pResult->find(k) == pResult->end()) {
			return IpfError::IPF_ERR_NOTFOUND;
		}
		pResult = &pResult->operator[](k);
	}
	*pResult = Json::parse(json2);

	str = result.dump();

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_UPDATEJSON)

	STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}

ipf_err_t JsonBeautify(const char* json, int indent, char* buf, size_t* len)
{

	CHECK_PARAM_JSON
	CHECK_PARAM_BUFFERLEN
	DECLARE_RESULT_JSON_BUFFER_LENGTH
	if (*len > 0) {
		CHECK_PARAM_BUFFER
	}

	ipf_err_t ret = IpfError::IPF_ERR_OK;
	std::string str;

	BEGIN_EXCEPTIONS_HANDLING

	result = Json::parse(json);
	str = result.dump(indent);

	END_EXCEPTIONS_HANDLING(IpfError::IPF_ERR_BEAUTIFYJSON)

	STRING_UPDATE_BUFFER_LENGTH_COPY_BUFFER

	return ret;
}
