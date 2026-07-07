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

#include <errno.h>
#include <filesystem>
#include <map>

#include "ClientApi.h"      // Public User Interface
#include "ClientApiProxy.h" // Hidden Dynamic Library Implementation
#include "DirApi.h"         // Get the install path of the exe
#include "DllApi.h"
#include "DllProxy.h" // Dynamic Library Interface Template
#include "Exceptions.h"
#include "IClientApi.h"
#include "JsonFields.h"
#include "Revision.h"
#include "nlohmann/json.hpp"

#define CLIENTAPI_DLLNAME DLLFILENAME("ClientApiProxy")

namespace Ipf
{

//////////////////////////////////////////////
// DLL Loader for ClientApi Proxy class
//////////////////////////////////////////////

class ClientApiLoader : public DllProxy<Proxy::ClientApi>
{
public:
	ClientApiLoader(const FrameworkLoadConfig& load_config)
		: DllProxy<Proxy::ClientApi>("ClientApiR6"), load_config_(load_config)
	{}
	int Open(const std::filesystem::path& path) override;

	// IPF-EF PV IClientApi revision was 3.  We maintain backward compatibility
	// with all revisions from the current revision back to and including PV.
	revision_t min_revision_ = CLIENTAPI_REVISION_REL_2_0_101_00;
	revision_t max_revision_ = CLIENTAPI_CURRENT_REVISION;

private:
	FrameworkLoadConfig load_config_;
};

int ClientApiLoader::Open(const std::filesystem::path& libname)
{
	// Load from framework install location
	std::filesystem::path p = GetFrameworkDirectory(load_config_);
	p /= libname;
	int rc = super::Open(p.string().c_str());
	if (rc == ERROR_SUCCESS) {
		revision_t revision = GetRevision();
		if (revision < min_revision_ || revision > max_revision_) {
			rc = EFAULT;
		}
	}
	return rc;
}

///////////////////////////////////////////////////////////////////
// Ipf::ClientApi::Impl = Public User Object Hidden Implementation
// This is the "Thin Layer" Hidden Implementation of the ClientApi
// which invisibly locates and loads the "Actual" ClientApi Proxy
//
// This class Catches and Rethrows copies of all exceptions types
// to avoid crashing when catching exceptions in client code due
// to DLL Unloading of the module that threw the exception.
///////////////////////////////////////////////////////////////////

class ClientApi::Impl
{
public:
	explicit Impl(const std::string& config = "{}");
	~Impl();

	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;

	std::string GetProperties(const std::string& properties_config = "")
	{
		std::string properties = "{}";
		try {
			properties = obj_->GetProperties(properties_config);
		}
		catch (...) {
			RethrowException();
		}
		return properties;
	}

	std::string GetValue(const std::string& path)
	{
		std::string value;
		try {
			value = obj_->GetValue(path);
		}
		catch (...) {
			RethrowException();
		}
		return value;
	}
	std::string GetNode(const std::string& path)
	{
		std::string node = "{}";
		try {
			node = obj_->GetNode(path);
		}
		catch (...) {
			RethrowException();
		}
		return node;
	}
	std::string GetSchema(const std::string& path, SchemaType schema_type)
	{
		std::string schema = "{}";
		try {
			schema = obj_->GetSchema(path, schema_type);
		}
		catch (...) {
			RethrowException();
		}
		return schema;
	}
	std::string ExecuteCommand(const std::string& path, const std::string& command)
	{
		std::string result = "{}";
		try {
			result = obj_->ExecuteCommand(path, command);
		}
		catch (...) {
			RethrowException();
		}
		return result;
	}
	std::string QueryNode(const std::string& path)
	{
		std::string node = "{}";
		try {
			node = obj_->QueryNode(path);
		}
		catch (...) {
			RethrowException();
		}
		return node;
	}

	void RegisterEvent(
		const std::string& path, EventCallback cb, void* context, const std::string& event_settings)
	{
		try {
			obj_->RegisterEvent(path, cb, context, event_settings);
		}
		catch (...) {
			RethrowException();
		}
	}
	void UnregisterEvent(const std::string& path, EventCallback cb)
	{
		try {
			obj_->UnregisterEvent(path, cb);
		}
		catch (...) {
			RethrowException();
		}
	}

	void SetValue(const std::string& path, bool value)
	{
		try {
			obj_->SetValue(path, value);
		}
		catch (...) {
			RethrowException();
		}
	}
	void SetValue(const std::string& path, int value)
	{
		try {
			obj_->SetValue(path, static_cast<int64_t>(value));
		}
		catch (...) {
			RethrowException();
		}
	}
	void SetValue(const std::string& path, int64_t value)
	{
		try {
			obj_->SetValue(path, value);
		}
		catch (...) {
			RethrowException();
		}
	}
	void SetValue(const std::string& path, double value)
	{
		try {
			obj_->SetValue(path, value);
		}
		catch (...) {
			RethrowException();
		}
	}
	void SetValue(const std::string& path, const std::string& value)
	{
		try {
			obj_->SetValue(path, value);
		}
		catch (...) {
			RethrowException();
		}
	}
	void SetValue(const std::string& path, const char* value)
	{
		if (value == nullptr) {
			throw InvalidArgumentException("SetValue: value parameter cannot be null");
		}
		try {
			obj_->SetValue(path, value);
		}
		catch (...) {
			RethrowException();
		}
	}
	std::string GetCapabilities(const std::string& path)
	{
		std::string capabilities = "{}";
		try {
			capabilities = obj_->GetCapabilities(path);
		}
		catch (...) {
			RethrowException();
		}
		return capabilities;
	}

private:
	std::unique_ptr<ClientApiLoader> lib_; // DLL Loader object
	Proxy::ClientApi* obj_ = nullptr;      // DLL-created object (Raw Pointer)
};

// Maximum allowed values for framework load configuration
static constexpr unsigned int MAX_INITIAL_FRAMEWORK_RETRIES = 1000;
static constexpr unsigned int MAX_RETRY_DELAY_MS = 60000;

// Helper function to parse framework load configuration from JSON config string
static FrameworkLoadConfig ParseFrameworkLoadConfig(const std::string& config)
{
	FrameworkLoadConfig load_config;

	if (config.empty() || config == "{}") {
		return load_config;
	}

	nlohmann::json config_json = nlohmann::json::parse(config);

	// Check platform config location first (body.options.framework_load_options)
	if (config_json.contains(JsonFields::BODY) &&
		config_json[JsonFields::BODY].contains(JsonFields::OPTIONS) &&
		config_json[JsonFields::BODY][JsonFields::OPTIONS].contains(
			JsonFields::FRAMEWORK_LOAD_OPTIONS) &&
		config_json[JsonFields::BODY][JsonFields::OPTIONS][JsonFields::FRAMEWORK_LOAD_OPTIONS]
			.contains(JsonFields::INITIAL_FRAMEWORK_RETRIES)) {
		load_config.initial_framework_retries =
			config_json[JsonFields::BODY][JsonFields::OPTIONS][JsonFields::FRAMEWORK_LOAD_OPTIONS]
					   [JsonFields::INITIAL_FRAMEWORK_RETRIES];
		if (config_json[JsonFields::BODY][JsonFields::OPTIONS][JsonFields::FRAMEWORK_LOAD_OPTIONS]
				.contains(JsonFields::RETRY_DELAY_MS)) {
			load_config.retry_delay_ms =
				config_json[JsonFields::BODY][JsonFields::OPTIONS]
						   [JsonFields::FRAMEWORK_LOAD_OPTIONS][JsonFields::RETRY_DELAY_MS];
		}
	}
	// Check provider config location (body.framework_load_options)
	else if (
		config_json.contains(JsonFields::BODY) &&
		config_json[JsonFields::BODY].contains(JsonFields::FRAMEWORK_LOAD_OPTIONS) &&
		config_json[JsonFields::BODY][JsonFields::FRAMEWORK_LOAD_OPTIONS].contains(
			JsonFields::INITIAL_FRAMEWORK_RETRIES)) {
		load_config.initial_framework_retries =
			config_json[JsonFields::BODY][JsonFields::FRAMEWORK_LOAD_OPTIONS]
					   [JsonFields::INITIAL_FRAMEWORK_RETRIES];
		if (config_json[JsonFields::BODY][JsonFields::FRAMEWORK_LOAD_OPTIONS].contains(
				JsonFields::RETRY_DELAY_MS)) {
			load_config.retry_delay_ms =
				config_json[JsonFields::BODY][JsonFields::FRAMEWORK_LOAD_OPTIONS]
						   [JsonFields::RETRY_DELAY_MS];
		}
	}

	// Check if outside bounds. It's Unsigned, so only need to check upper bound
	if (load_config.initial_framework_retries > MAX_INITIAL_FRAMEWORK_RETRIES) {
		// Throw early. Full schema validation in the framework will also catch this
		throw Ipf::IpfException(
			"initial_framework_retries value is outside of bounds 0 to " +
			std::to_string(MAX_INITIAL_FRAMEWORK_RETRIES));
	}
	if (load_config.retry_delay_ms > MAX_RETRY_DELAY_MS) {
		// Throw early. Full schema validation in the framework will also catch this
		throw Ipf::IpfException(
			"retry_delay_ms value is outside of bounds 0 to " + std::to_string(MAX_RETRY_DELAY_MS));
	}

	return load_config;
}

// Public User Object Hidden Constructor
ClientApi::Impl::Impl(const std::string& config) : lib_(), obj_()
{
	// Parse configuration to extract framework load options
	FrameworkLoadConfig load_config = ParseFrameworkLoadConfig(config);

	// Create a new instance of the DLL Loader for each instance of this class
	// so the DLL stays loaded until all instances of this class are destroyed
	int error = ENOMEM;
	lib_ = std::make_unique<ClientApiLoader>(load_config);
	if (lib_) {
		error = lib_->Open(CLIENTAPI_DLLNAME);
		if (error) {
			lib_.reset();
		}
	}
	if (error) {
		std::map<int, const char*> errormap = {
			{EINVAL, "Parameter is Null"},
			{EEXIST, "Library Already Loaded"},
			{ENXIO, "Symbol Not Found"},
			{ENOENT, "Cannot Open Library"},
			{EFAULT, "Incompatible Version"},
			{ENOEXEC, "Incompatible Build Configuration"},
			{EILSEQ, "Invalid Digital Signature"},
		};
		throw FrameworkException(
			std::string(FrameworkException::LIB_LOAD) + CLIENTAPI_DLLNAME + " : " +
			std::to_string(error) + " : " + errormap[error]);
	}

	// Create DLL-Exported class instance
	// Use a Raw Pointer since std::unique_ptr would not be able to access
	// lib_ in its custom deleter to destroy the instance
	try {
		obj_ = lib_->CreateInstance(config);
	}
	catch (...) {
		RethrowException();
	}

	if (!obj_) {
		throw FrameworkException(std::string(FrameworkException::OBJECT_ERROR) + "ClientApi");
	}
}

ClientApi::Impl::~Impl()
{
	// Destroy DLL-Exported class instance
	if (lib_ && obj_) {
		lib_->DestroyInstance(obj_);
	}
}

///////////////////////////////////////////////
// Ipf::ClientApi = Public User Object Methods
///////////////////////////////////////////////

ClientApi::ClientApi() : pImpl{std::make_unique<Impl>()}
{}

ClientApi::ClientApi(const std::string& config) : pImpl{std::make_unique<Impl>(config)}
{}

ClientApi::~ClientApi() = default;

std::string ClientApi::GetNode(const std::string& path)
{
	return pImpl->GetNode(path);
}

std::string ClientApi::GetProperties(const std::string& properties_config)
{
	return pImpl->GetProperties(properties_config);
}

std::string ClientApi::GetValue(const std::string& path)
{
	return pImpl->GetValue(path);
}

std::string ClientApi::GetSchema(const std::string& path, SchemaType schema_type)
{
	return pImpl->GetSchema(path, schema_type);
}

std::string ClientApi::ExecuteCommand(const std::string& path, const std::string& command)
{
	return pImpl->ExecuteCommand(path, command);
}

std::string ClientApi::QueryNode(const std::string& path)
{
	return pImpl->QueryNode(path);
}

void ClientApi::RegisterEvent(
	const std::string& path, EventCallback cb, void* context, const std::string& event_settings)
{
	pImpl->RegisterEvent(path, cb, context, event_settings);
}

void ClientApi::UnregisterEvent(const std::string& path, EventCallback cb)
{
	pImpl->UnregisterEvent(path, cb);
}

void ClientApi::SetValue(const std::string& path, bool value)
{
	pImpl->SetValue(path, value);
}

void ClientApi::SetValue(const std::string& path, int value)
{
	pImpl->SetValue(path, value);
}

void ClientApi::SetValue(const std::string& path, int64_t value)
{
	pImpl->SetValue(path, value);
}

void ClientApi::SetValue(const std::string& path, double value)
{
	pImpl->SetValue(path, value);
}

void ClientApi::SetValue(const std::string& path, const std::string& value)
{
	pImpl->SetValue(path, value);
}

void ClientApi::SetValue(const std::string& path, const char* value)
{
	pImpl->SetValue(path, value);
}

// Added in R5
std::string ClientApi::GetCapabilities(const std::string& path)
{
	return pImpl->GetCapabilities(path);
}

} // namespace Ipf
