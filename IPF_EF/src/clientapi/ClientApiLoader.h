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

#include <cassert>
#include <filesystem>
#include <optional>
#include <type_traits>
#include <variant>

#include "ClientApiProxy.h"
#include "ClientApiRevisionTraits.h"
#include "DllHandle.h"
#include "Exceptions.h"
#include "JsonObjectTranslators.h"
#include "Revision.h"

namespace Ipf
{

class ClientApiLoader final
{
public:
	ClientApiLoader() = default;

	~ClientApiLoader()
	{
		Close();
	}

	// Disallow copy-construction by explicitly deleting the
	// copy constructor and assignment operator.
	ClientApiLoader(const ClientApiLoader&) = delete;
	ClientApiLoader& operator=(const ClientApiLoader&) = delete;

	// Safe to call even if Open() was never called or failed.
	// DllHandle::Close() and optional::reset() are both no-ops in that case.
	void Close();

	template <typename T>
	static std::optional<ClientApiProxyVariant>
	TryLoadClientApiBindings(DLLHANDLE handle, const revision_t min_revision)
	{
		using Traits = ClientApiLoaderRevisionTraits<T>;

		if (Traits::revision < min_revision) {
			return std::nullopt;
		}

		T api{handle};
		const bool api_valid = IsValidClientApiProxy(api);

		if (!api_valid) {
			return std::nullopt;
		}

		return ClientApiProxyVariant{std::in_place_type<T>, std::move(api)};
	}

	std::optional<ClientApiProxyVariant>
	TryLoadClientApiBindingsInOrder(const revision_t min_revision);

	ClientApiProxyVariant LoadClientApiBindings(const revision_t min_revision);

	void Open(const std::filesystem::path& framework_path, const revision_t min_revision);

	template <typename ProxyType, typename CreatorType>
	static ClientApiVariant
	LoadSpecificClientApi(const revision_t revision, const std::string& config, CreatorType& arg)
	{
		const std::string converted_config =
			JsonObjectTranslators::RetargetProviderConfigFromClientApiRevision(config, revision);

		ProxyType* api_obj = arg.creator ? arg.creator(converted_config) : nullptr;
		if (!api_obj) {
			throw FrameworkException(
				std::string(FrameworkException::OBJECT_ERROR) +
				ClientApiRevisionTraits<ProxyType>::revision_name);
		}
		return api_obj;
	}

	// Safe Creator that only calls DL Creator if DL is loaded
	ClientApiVariant CreateInstance(const std::string& config);

	// Safe Destroyer that only calls DL Destroyer if DL is loaded
	void DestroyInstance(ClientApiVariant obj);

	revision_t GetRevision();

private:
	// Central loader dispatch helper
	template <typename EmptyFn, typename LoadedFn>
	std::invoke_result_t<EmptyFn> VisitLoadedClientApi(EmptyFn&& on_empty, LoadedFn&& on_loaded)
	{
		if (!api_.has_value()) {
			return on_empty();
		}

		return std::visit(
			[&on_loaded](auto& arg) -> std::invoke_result_t<EmptyFn> {
				using HolderType = std::decay_t<decltype(arg)>;
				static_assert(
					ClientApiLoaderRevisionTraits<HolderType>::is_defined,
					"Missing ClientApiLoaderRevisionTraits specialization");
				return on_loaded(arg);
			},
			*api_);
	}

	DllHandle handle_;                         // DLL Handle with lifecycle management
	std::optional<ClientApiProxyVariant> api_; // Store function pointers for the loaded revision
};

} // namespace Ipf
