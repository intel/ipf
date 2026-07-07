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
#include "Exceptions.h"
#include "SchemaType.h"

// Export Symbols only when building ClientApi Dynamic Library
#if defined(ClientApi_EXPORTS)
#define APIEXPORT DLLEXPORT
#else
#define APIEXPORT
#endif

namespace Ipf
{

class ClientApi
{
public:
	APIEXPORT ClientApi();
	APIEXPORT ClientApi(const std::string& config);
	APIEXPORT virtual ~ClientApi();

	APIEXPORT std::string GetProperties(const std::string& properties_config = "");

	APIEXPORT std::string GetValue(const std::string& path = "");
	APIEXPORT std::string GetNode(const std::string& path = "");
	APIEXPORT std::string GetSchema(const std::string& path, SchemaType schema_type);
	APIEXPORT std::string ExecuteCommand(const std::string& path, const std::string& command);
	APIEXPORT std::string QueryNode(const std::string& path);

	APIEXPORT void RegisterEvent(
		const std::string& path,
		EventCallback cb,
		void* context = nullptr,
		const std::string& event_settings = "null");
	APIEXPORT void UnregisterEvent(const std::string& path, EventCallback cb);

	APIEXPORT void SetValue(const std::string& path, bool value);
	APIEXPORT void SetValue(const std::string& path, int value);
	APIEXPORT void SetValue(const std::string& path, int64_t value);
	APIEXPORT void SetValue(const std::string& path, double value);
	APIEXPORT void SetValue(const std::string& path, const std::string& value);
	APIEXPORT void SetValue(const std::string& path, const char* value);

	// Added in ClientApi R5
	APIEXPORT std::string GetCapabilities(const std::string& path = "");

private:
	class Impl;

	std::unique_ptr<Impl> pImpl;
};

} // namespace Ipf
