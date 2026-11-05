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

#include "FrameworkLoadConfig.h"

#include "ClientApiRevisionMap.h"

namespace Ipf
{

namespace
{

// Maximum allowed values for framework load configuration
constexpr unsigned int MAX_INITIAL_FRAMEWORK_RETRIES = 1000;
constexpr unsigned int MAX_RETRY_DELAY_MS = 60000;

// Helper function to get the appropriate JSON reference for framework_load_options
// Checks provider config location (body.framework_load_options) first,
// then platform config location (body.options.framework_load_options)
const Json* GetFrameworkLoadOptionsRef(const Json& body_json)
{
	// Check provider config location first (most common case)
	if (body_json.contains(JsonFields::FRAMEWORK_LOAD_OPTIONS)) {
		return &body_json.at(JsonFields::FRAMEWORK_LOAD_OPTIONS);
	}

	// Check platform config location
	if (body_json.contains(JsonFields::OPTIONS)) {
		const Json& options_json = body_json.at(JsonFields::OPTIONS);
		if (options_json.contains(JsonFields::FRAMEWORK_LOAD_OPTIONS)) {
			return &options_json.at(JsonFields::FRAMEWORK_LOAD_OPTIONS);
		}
	}

	return nullptr;
}

void ParseFrameworkLoadConfigInternal(const std::string& config, FrameworkLoadConfig& load_config)
{
	if (config.empty() || config == "{}") {
		return; // Use defaults when empty
	}

	Json config_json = Json::parse(config);

	// Get framework load options from either platform or provider config location
	if (config_json.contains(JsonFields::BODY)) {
		const Json* options_ref = GetFrameworkLoadOptionsRef(config_json.at(JsonFields::BODY));
		if (options_ref != nullptr) {
			if (options_ref->contains(JsonFields::INITIAL_FRAMEWORK_RETRIES)) {
				load_config.initial_framework_retries =
					options_ref->at(JsonFields::INITIAL_FRAMEWORK_RETRIES).get<unsigned int>();
			}
			if (options_ref->contains(JsonFields::RETRY_DELAY_MS)) {
				load_config.retry_delay_ms =
					options_ref->at(JsonFields::RETRY_DELAY_MS).get<unsigned int>();
			}
			if (options_ref->contains(JsonFields::MINIMUM_FRAMEWORK_VERSION)) {
				load_config.minimum_clientapi_revision = ClientApiRevisionFromVersion(
					options_ref->at(JsonFields::MINIMUM_FRAMEWORK_VERSION).get<std::string>());
			}
		}
	}

	// Check if numeric parameters are outside bounds. The version was already checked during
	// parsing. These numeric properties are unsigned, so no need to check for negative values.
	if (load_config.initial_framework_retries > MAX_INITIAL_FRAMEWORK_RETRIES) {
		throw Ipf::IpfException(
			"initial_framework_retries value is outside of bounds 0 to " +
			std::to_string(MAX_INITIAL_FRAMEWORK_RETRIES));
	}

	if (load_config.retry_delay_ms > MAX_RETRY_DELAY_MS) {
		throw Ipf::IpfException(
			"retry_delay_ms value is outside of bounds 0 to " + std::to_string(MAX_RETRY_DELAY_MS));
	}
}

} // namespace

FrameworkLoadConfig ParseFrameworkLoadConfig(const std::string& config)
{
	FrameworkLoadConfig load_config; // Initialized with defaults
	try {
		ParseFrameworkLoadConfigInternal(config, load_config);
	}
	catch (...) {
		RethrowException(); // Ensure proper exception typing
	}

	return load_config;
}

} // namespace Ipf
