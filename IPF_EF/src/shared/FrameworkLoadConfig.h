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

#include "Base.h"
#include "Exceptions.h"
#include "JsonFields.h"
#include "Revision.h"
#include "nlohmann/json.hpp"

namespace Ipf
{

using Json = nlohmann::json;

// Configuration for the ClientApi to control loading of the framework
struct FrameworkLoadConfig {
	unsigned int initial_framework_retries = 121; // Default: 121 attempts (2 min @ 1sec/attempt)
	unsigned int retry_delay_ms = 1000;           // Default: 1 second between retries
	revision_t minimum_clientapi_revision =
		CLIENTAPI_CURRENT_REVISION; // Default: Only load current or higher version of the framework
};

/// @brief Parse options relating to framework loading from the given JSON config string. If options
/// are not specified, default values will be used. If the JSON is malformed or contains invalid
/// values, an exception will be thrown.
/// @param config Stringified JSON containing framework load configuration options. See
/// Provider/Platform Configuration Schema for details on the expected format of this JSON.
/// @return Parsed configuration options
FrameworkLoadConfig ParseFrameworkLoadConfig(const std::string& config);

} // namespace Ipf
