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
#include "JsonObjectTranslators.h"
#include "Exceptions.h"
#include "JsonFields.h"
#include "Revision.h"
#include "nlohmann/json.hpp"
#include <array>

namespace Ipf::JsonObjectTranslators
{

using Json = nlohmann::json;

namespace
{

struct RevisionFields {
	int revision;
	const char* const* fields;
	std::size_t field_count;
};

constexpr std::array<const char*, 2> kRevision3Fields = {
	JsonFields::DISABLE_PROVIDER_ISOLATION,
	JsonFields::FRAMEWORK_LOAD_OPTIONS,
};

constexpr std::array<const char*, 2> kRevision2Fields = {
	JsonFields::INCLUDE,
	JsonFields::EXCLUDE,
};

// Fields introduced at each schema revision; erase them when downgrading below that revision.
constexpr std::array<RevisionFields, 2> kFieldsByRevision = {{
	{3, kRevision3Fields.data(), kRevision3Fields.size()},
	{2, kRevision2Fields.data(), kRevision2Fields.size()},
}};

/// @brief Erases a named field from the "body" object of a JSON config if it exists.
/// @param config JSON object to modify in-place.
/// @param field_name Name of the field to erase from the body object.
void EraseFieldFromBodyIfExists(Json& config, const std::string& field_name)
{
	if (config.contains(JsonFields::BODY)) {
		Json& body = config[JsonFields::BODY];
		if (body.contains(field_name)) {
			body.erase(field_name);
		}
	}
}

/// @brief Adjusts a provider config JSON object so its schema_revision matches @p target_revision,
///        stripping any fields introduced at revisions above the target.
/// @param config JSON object to modify in-place; must be a valid provider config object.
/// @param target_revision The schema revision to downgrade the config to.
/// @throws FrameworkException if an unexpected error occurs during field removal.
void RetargetProviderConfig(Json& config, int target_revision)
{
	try {
		// Update schema revision to target revision.
		config[JsonFields::SCHEMA_REVISION] = target_revision;

		for (const auto& fields_by_revision : kFieldsByRevision) {
			if (target_revision < fields_by_revision.revision) {
				for (std::size_t i = 0; i < fields_by_revision.field_count; ++i) {
					EraseFieldFromBodyIfExists(config, fields_by_revision.fields[i]);
				}
			}
		}

		// If there are no properties left in body, return an empty config (the default), because
		// otherwise the schema validation will fail since body is required but we just erased all
		// the fields.
		if (config.contains(JsonFields::BODY) && config[JsonFields::BODY].empty()) {
			config = Json();
		}
	}
	catch (const std::exception& e) {
		throw FrameworkException(std::string("SchemaTranslator: ") + e.what());
	}
}

} // namespace

/// @brief Translates a serialized provider config JSON string to the schema revision that
///        corresponds to the given Client API revision.
/// @param config Serialized JSON string of the provider config. Returned unchanged if empty,
///        if it does not contain a provider config schema type, or if no translation is needed
///        because the config is already at or below the loaded framework's target schema revision.
/// @param client_api_revision The Client API revision of the consuming client, used to
///        determine the target schema revision.
/// @return The translated JSON string, or the original string if no translation is required.
///         Returns the original string unchanged if the config's schema revision is newer than
///         this SDK supports — the client is responsible for re-trying with a compatible config.
/// @throws FrameworkException if @p client_api_revision is outside the supported range, or
///         if the JSON string cannot be parsed.
/// @note Platform configs are not translated by this function since they are only operational in
/// debug builds and test clients can be expected to be on the same framework revision as the SDK.
/// Clients are on their own if they want to retry with a compatible platform config.
std::string
RetargetProviderConfigFromClientApiRevision(const std::string& config, int client_api_revision)
{
	if (config.empty()) {
		return config;
	}

	// Check validation and revision bounds before proceeding
	if (client_api_revision < CLIENTAPI_REVISION_REL_2_0_101_00 ||
		client_api_revision > CLIENTAPI_CURRENT_REVISION) {
		throw FrameworkException(
			"RetargetProviderConfigFromClientApiRevision: Unsupported Client API "
			"revision " +
			std::to_string(client_api_revision));
	}

	try {
		Json config_obj = Json::parse(config);

		// Platform configs pass through unchanged (not translated by this function)
		if (config_obj.contains(JsonFields::SCHEMA_TYPE) &&
			config_obj[JsonFields::SCHEMA_TYPE] == JsonFields::PLATFORM_CONFIG) {
			return config;
		}

		// If it's not a provider config, return unchanged
		if (!config_obj.contains(JsonFields::SCHEMA_TYPE) ||
			config_obj[JsonFields::SCHEMA_TYPE] != JsonFields::PROVIDER_CONFIG) {
			return config;
		}

		// It's a valid provider config - determine target revision
		int target_schema_revision = PROVIDER_CONFIG_SCHEMA_CURRENT_REVISION;
		if (client_api_revision < CLIENTAPI_REVISION_GETCAPABILITIES) {
			target_schema_revision = 1;
		}
		else if (client_api_revision < CLIENTAPI_REVISION_PROVIDER_ISOLATION) {
			target_schema_revision = 2;
		}

		// Check fast-path from raw string (only if it parsed as JSON successfully)
		int parsed_schema_revision = 0;
		if (config_obj.contains(JsonFields::SCHEMA_REVISION) &&
			config_obj[JsonFields::SCHEMA_REVISION].is_number_integer()) {
			parsed_schema_revision = config_obj[JsonFields::SCHEMA_REVISION].get<int>();
		}
		else {
			throw FrameworkException(
				"Schema revision field is missing or not an integer in provider config");
		}

		// Fast path: if already at or below target revision, return original string unchanged
		if (parsed_schema_revision <= target_schema_revision) {
			return config;
		}

		// If the config's schema revision is newer than what this SDK knows about, return it
		// unmodified. This occurs when an older client SDK sends us a config from a newer
		// framework that has introduced new schema features. The client is responsible for
		// re-trying with a compatible config if it errors out on the framework side in this case.
		if (parsed_schema_revision > PROVIDER_CONFIG_SCHEMA_CURRENT_REVISION) {
			return config;
		}

		// Translate the config
		RetargetProviderConfig(config_obj, target_schema_revision);
		return config_obj.dump();
	}
	catch (const nlohmann::json::exception& e) {
		throw FrameworkException(
			std::string("SchemaTranslator: Failed to process JSON config: ") + e.what());
	}
}

} // namespace Ipf::JsonObjectTranslators
