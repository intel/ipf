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

#include <filesystem>
#include <optional>
#include <variant>

#include "ClientApi.h" // Public User Interface
#include "ClientApiLoader.h"
#include "ClientApiProxy.h" // Hidden Dynamic Library Implementation
#include "ClientApiRevisionTraits.h"
#include "DirApi.h" // Get the install path of the exe
#include "Exceptions.h"
#include "FrameworkLoadConfig.h"
#include "IClientApi.h"
#include "JsonFields.h"
#include "Revision.h"
#include "nlohmann/json.hpp"

namespace Ipf
{

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
private:
	// Central client API dispatch helper.
	// Onboarding checklist for a new ClientApi interface revision:
	// 1) Add pointer type to ClientApiVariant.
	// 2) Add ClientApiRevisionTraits specialization.
	// 3) Existing common methods automatically dispatch without per-method edits.
	// This must be declared at the top of the class because GCC can't deduce the return type of the
	// lambda if it is declared after usage of VisitClientApi. MSVC is fine with it for some
	// reason...
	template <typename Fn> decltype(auto) VisitClientApi(Fn&& fn)
	{
		return std::visit(
			[&fn](auto* api) -> decltype(auto) {
				using ApiType = std::remove_pointer_t<decltype(api)>;
				static_assert(
					ClientApiRevisionTraits<ApiType>::is_defined,
					"Missing ClientApiRevisionTraits specialization for ClientApi revision type");
				return fn(api);
			},
			obj_);
	}

public:
	explicit Impl(const std::string& config = "{}");
	~Impl();

	Impl(const Impl&) = delete;
	Impl& operator=(const Impl&) = delete;

	std::string GetProperties(const std::string& properties_config = "")
	{
		std::string properties = "{}";
		try {
			properties = VisitClientApi(
				[&](auto* api) -> std::string { return api->GetProperties(properties_config); });
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
			value = VisitClientApi([&](auto* api) -> std::string { return api->GetValue(path); });
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
			node = VisitClientApi([&](auto* api) -> std::string { return api->GetNode(path); });
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
			schema = VisitClientApi(
				[&](auto* api) -> std::string { return api->GetSchema(path, schema_type); });
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
			result = VisitClientApi(
				[&](auto* api) -> std::string { return api->ExecuteCommand(path, command); });
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
			node = VisitClientApi([&](auto* api) -> std::string { return api->QueryNode(path); });
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
			VisitClientApi([&](auto* api) {
				using ApiType = std::remove_pointer_t<decltype(api)>;
				if constexpr (ClientApiRevisionTraits<ApiType>::supports_event_settings) {
					api->RegisterEvent(path, cb, context, event_settings);
				}
				else {
					if (!event_settings.empty() && event_settings != "null") {
						throw Ipf::ApiFeatureUnavailableException("Event settings");
					}
					api->RegisterEvent(path, cb, context);
				}
			});
		}
		catch (...) {
			RethrowException();
		}
	}

	void UnregisterEvent(const std::string& path, EventCallback cb)
	{
		try {
			VisitClientApi([&](auto* api) { api->UnregisterEvent(path, cb); });
		}
		catch (...) {
			RethrowException();
		}
	}

	void SetValue(const std::string& path, bool value)
	{
		try {
			VisitClientApi([&](auto* api) { api->SetValue(path, value); });
		}
		catch (...) {
			RethrowException();
		}
	}

	void SetValue(const std::string& path, int64_t value)
	{
		try {
			VisitClientApi([&](auto* api) { api->SetValue(path, value); });
		}
		catch (...) {
			RethrowException();
		}
	}

	void SetValue(const std::string& path, double value)
	{
		try {
			VisitClientApi([&](auto* api) { api->SetValue(path, value); });
		}
		catch (...) {
			RethrowException();
		}
	}

	void SetValue(const std::string& path, const std::string& value)
	{
		try {
			VisitClientApi([&](auto* api) { api->SetValue(path, value); });
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
			VisitClientApi([&](auto* api) { api->SetValue(path, value); });
		}
		catch (...) {
			RethrowException();
		}
	}

	std::string GetCapabilities(const std::string& path)
	{
		std::string capabilities = "{}";
		try {
			capabilities = VisitClientApi([&](auto* api) -> std::string {
				using ApiType = std::remove_pointer_t<decltype(api)>;
				if constexpr (ClientApiRevisionTraits<ApiType>::supports_get_capabilities) {
					return api->GetCapabilities(path);
				}
				else {
					throw Ipf::ApiFeatureUnavailableException("GetCapabilities");
				}
			});
		}
		catch (...) {
			RethrowException();
		}
		return capabilities;
	}

private:
	std::unique_ptr<ClientApiLoader> lib_; // DLL Loader object
	ClientApiVariant obj_; // DLL-created object (raw pointer wrapped in variant type to handle
						   // multiple revisions)
};

// Public User Object Hidden Constructor
ClientApi::Impl::Impl(const std::string& config) : lib_(), obj_()
{
	// Parse configuration to extract framework load options
	FrameworkLoadConfig load_config = ParseFrameworkLoadConfig(config);

	// Create a new instance of the DLL Loader for each instance of this class
	// so the DLL stays loaded until all instances of this class are destroyed
	const std::filesystem::path framework_path = GetFrameworkDirectory(load_config);
	lib_ = std::make_unique<ClientApiLoader>();
	try {
		lib_->Open(framework_path, load_config.minimum_clientapi_revision);
	}
	catch (...) {
		lib_.reset();
		RethrowException();
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
}

ClientApi::Impl::~Impl()
{
	// Destroy DLL-Exported class instance
	if (lib_) {
		try {
			lib_->DestroyInstance(obj_);
		}
		catch (...) {
			// Can't throw here. Won't get here in practice - won't hit the dtor without an obj_
		}
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
	// ClientApiProxy only uses int64_t
	pImpl->SetValue(path, static_cast<int64_t>(value));
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
