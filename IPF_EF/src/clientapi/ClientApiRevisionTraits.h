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

#include <variant>

#include "ClientApiProxy.h"
#include "DllApi.h"
#include "Revision.h"

namespace Ipf
{

// DLL Loader for ClientApi Proxy class
template <class T> bool IsValidClientApiProxy(const T& proxy)
{
	return proxy.creator != nullptr && proxy.destroyer != nullptr;
}

// These structs hold the function pointers for each ClientApi revision's factory functions, which
// are loaded from ClientApiProxy.dll. The ClientApiLoader loads the results of creator into a
// variant, and then ClientApi::Impl dispatches calls to the correct revision.
struct ClientApiBindingsR3 {
	const Proxy::ClientApiR3::Creator creator;
	const Proxy::ClientApiR3::Destroyer destroyer;

	ClientApiBindingsR3(DLLHANDLE handle)
		: creator{reinterpret_cast<Proxy::ClientApiR3::Creator>(
			  DLLSymbol(handle, "ClientApiCreate"))},
		  destroyer{reinterpret_cast<Proxy::ClientApiR3::Destroyer>(
			  DLLSymbol(handle, "ClientApiDestroy"))}
	{}
};

struct ClientApiBindingsR4 {
	const Proxy::ClientApiR4::Creator creator;
	const Proxy::ClientApiR4::Destroyer destroyer;

	ClientApiBindingsR4(DLLHANDLE handle)
		: creator{reinterpret_cast<Proxy::ClientApiR4::Creator>(
			  DLLSymbol(handle, "ClientApiR4Create"))},
		  destroyer{reinterpret_cast<Proxy::ClientApiR4::Destroyer>(
			  DLLSymbol(handle, "ClientApiR4Destroy"))}
	{}
};

struct ClientApiBindingsR5 {
	const Proxy::ClientApiR5::Creator creator;
	const Proxy::ClientApiR5::Destroyer destroyer;

	ClientApiBindingsR5(DLLHANDLE handle)
		: creator{reinterpret_cast<Proxy::ClientApiR5::Creator>(
			  DLLSymbol(handle, "ClientApiR5Create"))},
		  destroyer{reinterpret_cast<Proxy::ClientApiR5::Destroyer>(
			  DLLSymbol(handle, "ClientApiR5Destroy"))}
	{}
};

struct ClientApiBindingsR6 {
	const Proxy::ClientApi::Creator creator;
	const Proxy::ClientApi::Destroyer destroyer;

	ClientApiBindingsR6(DLLHANDLE handle)
		: creator{reinterpret_cast<Proxy::ClientApi::Creator>(
			  DLLSymbol(handle, "ClientApiR6Create"))},
		  destroyer{reinterpret_cast<Proxy::ClientApi::Destroyer>(
			  DLLSymbol(handle, "ClientApiR6Destroy"))}
	{}
};

// Type definition for the containers that hold the loaded ClientApiProxy creation/destruction
// function pointers.
using ClientApiProxyVariant = std::
	variant<ClientApiBindingsR3, ClientApiBindingsR4, ClientApiBindingsR5, ClientApiBindingsR6>;

// Type definition for the variant that holds the actual ClientApi object created by the loaded
// ClientApiProxy.
using ClientApiVariant =
	std::variant<Proxy::ClientApiR3*, Proxy::ClientApiR4*, Proxy::ClientApiR5*, Proxy::ClientApi*>;

// Compile-time feature tags for concrete ClientApi interface revisions used by
// ClientApi::Impl dispatch. This keeps feature checks local and avoids repeating
// per-revision std::visit lambdas in each method.
// is_defined ensures that any variant type used in ClientApiVariant has a corresponding traits
// specialization, which is important for the correctness of the visit lambdas.
// The trait specializations below define which features are supported in each ClientApi revision,
// which the ClientApi thin layer uses to dispatch calls and throw exceptions for unsupported
// features when necessary. The empty <> specialization is there because there are no new template
// parameters being added, because the type is already fully specified.
template <typename T> struct ClientApiRevisionTraits {
	static constexpr bool is_defined = false;
};

template <> struct ClientApiRevisionTraits<Proxy::ClientApiR3> {
	static constexpr bool is_defined = true;
	static constexpr bool supports_event_settings = false;
	static constexpr bool supports_get_capabilities = false;
	static constexpr const char* revision_name = "ClientApiR3";
};

template <> struct ClientApiRevisionTraits<Proxy::ClientApiR4> {
	static constexpr bool is_defined = true;
	static constexpr bool supports_event_settings = true;
	static constexpr bool supports_get_capabilities = false;
	static constexpr const char* revision_name = "ClientApiR4";
};

template <> struct ClientApiRevisionTraits<Proxy::ClientApiR5> {
	static constexpr bool is_defined = true;
	static constexpr bool supports_event_settings = true;
	static constexpr bool supports_get_capabilities = true;
	static constexpr const char* revision_name = "ClientApiR5";
};

template <> struct ClientApiRevisionTraits<Proxy::ClientApi> {
	static constexpr bool is_defined = true;
	static constexpr bool supports_event_settings = true;
	static constexpr bool supports_get_capabilities = true;
	static constexpr const char* revision_name = "ClientApiR6";
};

// Compile-time mapping for loader revision bindings to the
// concrete proxy type and revision used for config translation in CreateInstance.
template <typename T> struct ClientApiLoaderRevisionTraits {
	static constexpr bool is_defined = false;
};

template <> struct ClientApiLoaderRevisionTraits<ClientApiBindingsR3> {
	static constexpr bool is_defined = true;
	using ApiType = Proxy::ClientApiR3;
	static constexpr revision_t revision = CLIENTAPI_REVISION_REL_2_1_101_00;
};

template <> struct ClientApiLoaderRevisionTraits<ClientApiBindingsR4> {
	static constexpr bool is_defined = true;
	using ApiType = Proxy::ClientApiR4;
	static constexpr revision_t revision = CLIENTAPI_REVISION_LNL_DEV;
};

template <> struct ClientApiLoaderRevisionTraits<ClientApiBindingsR5> {
	static constexpr bool is_defined = true;
	using ApiType = Proxy::ClientApiR5;
	static constexpr revision_t revision = CLIENTAPI_REVISION_GETCAPABILITIES;
};

template <> struct ClientApiLoaderRevisionTraits<ClientApiBindingsR6> {
	static constexpr bool is_defined = true;
	using ApiType = Proxy::ClientApi;
	static constexpr revision_t revision = CLIENTAPI_REVISION_PROVIDER_ISOLATION;
};

} // namespace Ipf
