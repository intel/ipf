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

/*
 * This header is a self-contained JSON-object implementation of Ipf::ClientApi
 * that may be used by client applications so that they can directly work with
 * JSON objects instead of JSON strings returned by the Ipf::ClientApi interface
 *
 * The C++ implementation uses nlohmann::json, but other languages will use
 * other JSON libraries. This header can be customized by developers to use
 * a different JSON library if they choose, and link with the ClientApi
 * static or dynamic library.
 */

#include "ClientApi.h"
#include "nlohmann/json.hpp"

using Json = nlohmann::json;

namespace Ipf
{

class ClientApiJson
{
public:
	ClientApiJson() : ipf()
	{}
	ClientApiJson(const Json& config) : ipf(config.dump())
	{}
	virtual ~ClientApiJson()
	{}

	Json GetProperties(const std::string& properties_config = "")
	{
		return Json::parse(ipf.GetProperties(properties_config));
	}

	Json GetValue(const std::string& path = "")
	{
		return Json::parse(ipf.GetValue(path));
	}
	Json GetNode(const std::string& path = "")
	{
		return Json::parse(ipf.GetNode(path));
	}
	Json GetSchema(const std::string& path, SchemaType schema_type)
	{
		return Json::parse(ipf.GetSchema(path, schema_type));
	}
	Json ExecuteCommand(const std::string& path, const Json& command)
	{
		return Json::parse(ipf.ExecuteCommand(path, command.dump()));
	}
	Json QueryNode(const std::string& path)
	{
		return Json::parse(ipf.QueryNode(path));
	}
	void RegisterEvent(
		const std::string& path,
		EventCallback cb,
		void* context = nullptr,
		const Json& event_settings = nullptr)
	{
		ipf.RegisterEvent(path, cb, context, event_settings.dump());
	}
	void UnregisterEvent(const std::string& path, EventCallback cb)
	{
		ipf.UnregisterEvent(path, cb);
	}

	void SetValue(const std::string& path, bool value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, int value)
	{
		ipf.SetValue(path, static_cast<int64_t>(value));
	}
	void SetValue(const std::string& path, int64_t value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, double value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, const std::string& value)
	{
		ipf.SetValue(path, value);
	}
	void SetValue(const std::string& path, const char* value)
	{
		if (value == nullptr) {
			throw InvalidArgumentException("SetValue: value parameter cannot be null");
		}
		ipf.SetValue(path, value);
	}

	Json GetCapabilities(const std::string& path)
	{
		return Json::parse(ipf.GetCapabilities(path));
	}

private:
	ClientApi ipf;
};

} // namespace Ipf
