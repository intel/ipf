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

#include <memory>
#include <stdint.h>
#include <string>

#include "Base.h"
#include "Events.h"
#include "IClientApi.h"
#include "IClientApiR3.h"
#include "IClientApiR4.h"
#include "IClientApiR5.h"
#include "SchemaType.h"

namespace Ipf
{
namespace Proxy
{ // Ipf::Proxy
class ClientApi : public IClientApi
{
public:
	ClientApi(const std::string& config = "{0}");
	virtual ~ClientApi();

	std::string GetProperties(const std::string& properties_config = "");

	std::string GetValue(const std::string& path = "");
	std::string GetNode(const std::string& path = "");
	std::string GetSchema(const std::string& path, SchemaType schema_type);
	std::string ExecuteCommand(const std::string& path, const std::string& command);
	std::string QueryNode(const std::string& path);

	void RegisterEvent(
		const std::string& path,
		EventCallback cb,
		void* context = nullptr,
		const std::string& event_settings = "null");
	void UnregisterEvent(const std::string& path, EventCallback cb);

	void SetValue(const std::string& path, bool value);
	void SetValue(const std::string& path, int64_t value);
	void SetValue(const std::string& path, double value);
	void SetValue(const std::string& path, const std::string& value);
	void SetValue(const std::string& path, const char* value);

	// Added in R5
	std::string GetCapabilities(const std::string& path);

	using Creator = ClientApi* (*)(const std::string& config);
	using Destroyer = void (*)(ClientApi*);
	using Revisioner = revision_t (*)();

private:
	// ClientApiProxy is the hidden class used to implement the pImpl pattern for the ClientApi.
	// This is to clearly differentiate it from the ClientApi that is linked into the client. When
	// building the static library, the class will be defined to "ClientApi::Impl", so that the same
	// code can be used to implement the fully in-process and statically-linked framework.
	class ClientApiProxy;

	std::unique_ptr<ClientApiProxy> pImpl;
};

class ClientApiR3 : public IClientApiR3
{
public:
	ClientApiR3(const std::string& config = "{0}");
	virtual ~ClientApiR3();

	std::string GetProperties(const std::string& properties_config = "");

	std::string GetValue(const std::string& path = "");
	std::string GetNode(const std::string& path = "");
	std::string GetSchema(const std::string& path, SchemaType schema_type);
	std::string ExecuteCommand(const std::string& path, const std::string& command);
	std::string QueryNode(const std::string& path);

	void RegisterEvent(const std::string& path, EventCallback cb, void* context = nullptr);
	void UnregisterEvent(const std::string& path, EventCallback cb);

	void SetValue(const std::string& path, bool value);
	void SetValue(const std::string& path, int64_t value);
	void SetValue(const std::string& path, double value);
	void SetValue(const std::string& path, const std::string& value);
	void SetValue(const std::string& path, const char* value);

	using Creator = ClientApiR3* (*)(const std::string& config);
	using Destroyer = void (*)(ClientApiR3*);
	using Revisioner = revision_t (*)();

private:
	ClientApi clientapi_;
};

class ClientApiR4 : public IClientApiR4
{
public:
	ClientApiR4(const std::string& config = "{0}");
	virtual ~ClientApiR4();

	std::string GetProperties(const std::string& properties_config = "");

	std::string GetValue(const std::string& path = "");
	std::string GetNode(const std::string& path = "");
	std::string GetSchema(const std::string& path, SchemaType schema_type);
	std::string ExecuteCommand(const std::string& path, const std::string& command);
	std::string QueryNode(const std::string& path);

	void RegisterEvent(
		const std::string& path,
		EventCallback cb,
		void* context = nullptr,
		const std::string& event_settings = "null");
	void UnregisterEvent(const std::string& path, EventCallback cb);

	void SetValue(const std::string& path, bool value);
	void SetValue(const std::string& path, int64_t value);
	void SetValue(const std::string& path, double value);
	void SetValue(const std::string& path, const std::string& value);
	void SetValue(const std::string& path, const char* value);

	using Creator = ClientApiR4* (*)(const std::string& config);
	using Destroyer = void (*)(ClientApiR4*);
	using Revisioner = revision_t (*)();

private:
	ClientApi clientapi_;
};

class ClientApiR5 : public IClientApiR5
{
public:
	ClientApiR5(const std::string& config = "{0}");
	virtual ~ClientApiR5();

	std::string GetProperties(const std::string& properties_config = "");

	std::string GetValue(const std::string& path = "");
	std::string GetNode(const std::string& path = "");
	std::string GetSchema(const std::string& path, SchemaType schema_type);
	std::string ExecuteCommand(const std::string& path, const std::string& command);
	std::string QueryNode(const std::string& path);

	void RegisterEvent(
		const std::string& path,
		EventCallback cb,
		void* context = nullptr,
		const std::string& event_settings = "null");
	void UnregisterEvent(const std::string& path, EventCallback cb);

	void SetValue(const std::string& path, bool value);
	void SetValue(const std::string& path, int64_t value);
	void SetValue(const std::string& path, double value);
	void SetValue(const std::string& path, const std::string& value);
	void SetValue(const std::string& path, const char* value);

	// Added in R5
	std::string GetCapabilities(const std::string& path);

	using Creator = ClientApiR5* (*)(const std::string& config);
	using Destroyer = void (*)(ClientApiR5*);
	using Revisioner = revision_t (*)();

private:
	ClientApi clientapi_;
};

} // namespace Proxy
} // namespace Ipf
