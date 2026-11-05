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

#include "ClientApiLoader.h"

#include "DllApi.h"

namespace Ipf
{

namespace
{
constexpr char CLIENTAPI_DLLNAME[] = DLLFILENAME("ClientApiProxy");
}

std::optional<ClientApiProxyVariant>
ClientApiLoader::TryLoadClientApiBindingsInOrder(const revision_t min_revision)
{
	using LoaderFn = std::optional<ClientApiProxyVariant> (*)(DLLHANDLE, const revision_t);
	struct LoadStep {
		revision_t revision;
		LoaderFn load;
	};

	static constexpr LoadStep steps[] = {
		{ClientApiLoaderRevisionTraits<ClientApiBindingsR6>::revision,
		 &TryLoadClientApiBindings<ClientApiBindingsR6>},
		{ClientApiLoaderRevisionTraits<ClientApiBindingsR5>::revision,
		 &TryLoadClientApiBindings<ClientApiBindingsR5>},
		{ClientApiLoaderRevisionTraits<ClientApiBindingsR4>::revision,
		 &TryLoadClientApiBindings<ClientApiBindingsR4>},
		{ClientApiLoaderRevisionTraits<ClientApiBindingsR3>::revision,
		 &TryLoadClientApiBindings<ClientApiBindingsR3>},
	};

	for (const auto& step : steps) {
		if (step.revision < min_revision) {
			break;
		}

		auto candidate = step.load(handle_.Get(), min_revision);
		if (candidate.has_value()) {
			return candidate;
		}
	}

	return std::nullopt;
}

ClientApiProxyVariant ClientApiLoader::LoadClientApiBindings(const revision_t min_revision)
{
	std::optional<ClientApiProxyVariant> api = TryLoadClientApiBindingsInOrder(min_revision);

	if (!api.has_value()) {
		throw FrameworkException(
			std::string(FrameworkException::LIB_LOAD) + CLIENTAPI_DLLNAME +
			": no supported ClientApi proxy revision found");
	}

	return std::move(*api);
}

void ClientApiLoader::Open(const std::filesystem::path& framework_path, const revision_t min_revision)
{
	int rc = handle_.Open(framework_path / CLIENTAPI_DLLNAME);
	if (rc != ERROR_SUCCESS) {
		throw FrameworkException(
			std::string(FrameworkException::LIB_LOAD) + CLIENTAPI_DLLNAME + " : " +
			std::to_string(rc) + " : " + DllHandle::ErrorMessage(rc));
	}

	try {
		// LoadClientApiBindings throws FrameworkException if no supported revision found
		api_.emplace(LoadClientApiBindings(min_revision));
	}
	catch (...) {
		Close();
		throw;
	}
}

void ClientApiLoader::Close()
{
	handle_.Close();
	api_.reset();
}

ClientApiVariant ClientApiLoader::CreateInstance(const std::string& config)
{
	return VisitLoadedClientApi(
		[]() -> ClientApiVariant {
			throw FrameworkException(
				std::string(FrameworkException::OBJECT_ERROR) + "ClientApiLoader");
		},
		[&config](auto& arg) -> ClientApiVariant {
			using HolderType = std::decay_t<decltype(arg)>;
			using Traits = ClientApiLoaderRevisionTraits<HolderType>;
			return LoadSpecificClientApi<typename Traits::ApiType>(
				Traits::revision, config, arg);
		});
}

void ClientApiLoader::DestroyInstance(ClientApiVariant obj)
{
	VisitLoadedClientApi(
		[]() {
			// No-op, nothing to destroy
		},
		[&obj](auto& arg) {
			using HolderType = std::decay_t<decltype(arg)>;
			using Traits = ClientApiLoaderRevisionTraits<HolderType>;
			assert((std::holds_alternative<typename Traits::ApiType*>(obj)));
			typename Traits::ApiType* p = std::get<typename Traits::ApiType*>(obj);
			arg.destroyer(p);
		});
}

revision_t ClientApiLoader::GetRevision()
{
	return VisitLoadedClientApi(
		[]() -> revision_t {
			throw FrameworkException(
				"ClientApiLoader tried to get revision without DLL loaded");
		},
		[](auto& arg) -> revision_t {
			using HolderType = std::decay_t<decltype(arg)>;
			return ClientApiLoaderRevisionTraits<HolderType>::revision;
		});
}

} // namespace Ipf
