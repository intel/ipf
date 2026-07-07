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
#include <set>

namespace Ipf
{

namespace JsonFields
{

constexpr char SCHEMA_TYPE[] = "schema_type";
constexpr char SCHEMA_REVISION[] = "schema_revision";
constexpr char PLATFORM_CONFIG[] = "platform_config";
constexpr char PROVIDER_MANIFEST[] = "provider_manifest";
constexpr char PROVIDER_CONFIG[] = "provider_config";
constexpr char PROVIDER_CATALOG[] = "provider_catalog";
constexpr char MODULE_PROPERTIES[] = "module_properties";
constexpr char GET_PROPERTIES[] = "get_properties";
constexpr char PROVIDER_CAPABILITIES[] = "provider_capabilities";
constexpr char EVENT_SETTINGS[] = "event_settings";
constexpr char FRAMEWORK_EVENT_SETTINGS[] = "framework_event_settings";
constexpr char BLOCKED_PROVIDER_CONFIG[] = "blocked_provider_config";
constexpr char EXAMPLES[] = "examples";
constexpr char DESCRIPTION[] = "description";
constexpr char PROVIDER_PACKAGES[] = "provider_packages";
constexpr char MODULES[] = "modules";
constexpr char LOCAL_PROVIDERS[] = "local_providers";
constexpr char INF_PROVIDERS[] = "inf_providers";
constexpr char FILE_PROVIDERS[] = "file_providers";
constexpr char BODY[] = "body";
constexpr char NAMESPACE[] = "namespace";
constexpr char INSTANCE_NAME[] = "name";
constexpr char CLASS[] = "class";
constexpr char UNIQUE_ID[] = "unique_id";
constexpr char ALLOWED_UNIQUE_IDs[] = "allowed_unique_ids";
constexpr char CHILDREN[] = "children";
constexpr char PROPERTIES[] = "properties";
constexpr char INIT[] = "init";
constexpr char ATTACH[] = "attach";
constexpr char LOCATION[] = "location";
constexpr char OPTIONS[] = "options";
constexpr char OBJECT_TYPE[] = "object";
constexpr char COMMANDS[] = "commands";
constexpr char EVENTS[] = "events";
constexpr char SCHEMA[] = "schema";
constexpr char NAME[] = "name";
constexpr char DATA_SCHEMA[] = "data_schema";
constexpr char EVENT_SCHEMA[] = "event_schema";
constexpr char INIT_SCHEMA[] = "init_schema";
constexpr char COMMAND_SCHEMA[] = "command_schema";
constexpr char RESULT_SCHEMA[] = "result_schema";
constexpr char PROPERTIES_TYPE[] = "properties_type";
constexpr char CATALOG[] = "catalog";
constexpr char VERSION[] = "version";
constexpr char AUTHOR[] = "author";
constexpr char REVISION[] = "revision";
constexpr char ATTACH_INSTRUCTIONS[] = "attach_instructions";
constexpr char PROVIDERS[] = "providers";
constexpr char NAMESPACE_CHANGED[] = "NamespaceChanged";
constexpr char PROVIDER_INSTANCE_CREATED[] = "ProviderInstanceCreated";
constexpr char PROVIDER_INSTANCE_DELETED[] = "ProviderInstanceDeleted";
constexpr char PROVIDER_SCHEMA_CHANGED[] = "ProviderSchemaChanged";
constexpr char EXCEPTION_LIST[] = "ExceptionList";
constexpr char PROVIDER_CAPABILITIES_CHANGED[] = "ProviderCapabilitiesChanged";
constexpr char GET_LAST_ERROR[] = "GetLastError";
constexpr char TIME_STAMP[] = "TimeStamp";
constexpr char INSTANCE[] = "instance";
constexpr char PARTKEY[] = "PartKey";
constexpr char INCLUDE[] = "include";
constexpr char EXCLUDE[] = "exclude";
constexpr char PLATFORM[] = "Platform";
constexpr char PROVIDER_CLASS[] = "ProviderClass";
constexpr char PROVIDER_VERSION[] = "ProviderVersion";
constexpr char PRODUCT_VERSION[] = "product_version";
constexpr char BLOCKED_PROVIDERS[] = "blocked_providers";
constexpr char FRAMEWORK_IMPACTED[] = "framework_impacted";
constexpr char BINARY_HASH[] = "binary_hash";
constexpr char EMPTY_JSON_OBJECT[] = "{}";
constexpr char FRAMEWORK_NAME[] = "Framework";
constexpr char CLIENT_INFO[] = "client_info";
constexpr char CLIENT_CONFIG[] = "client_config";
constexpr char CLIENT_REVISION[] = "client_revision";
constexpr char CLIENT_PATH[] = "client_path";

// Fields supporting NodeStatusChanged Status Events
constexpr char NODE_STATUS_CHANGED[] = "NodeStatusChanged";
constexpr char NODE_STATUS_PATH[] = "Path";
constexpr char NODE_STATUS_STATUS[] = "Status";
constexpr char NODE_STATUS_UNKNOWN[] = "Unknown";
constexpr char NODE_STATUS_NOT_READY[] = "NotReady";
constexpr char NODE_STATUS_READY[] = "Ready";
constexpr char NODE_STATUS_EVENT_PATHS[] = "NodeStatusEventPaths";

// Fields supporting NamespaceChanged Status Events
constexpr char PROVIDER[] = "Provider";

// Fields supporting EventSubscriptionRestoreFailed events
constexpr char EVENT_SUBSCRIPTION_RESTORE_FAILED[] = "EventSubscriptionRestoreFailed";
constexpr char PATHS[] = "Paths";

// Fields supporting provider exception events
constexpr char NODE_EXCEPTION_EVENT[] = "NodeException";
constexpr char MODULE_EXCEPTION_EVENT[] = "ModuleException";
constexpr char NODE[] = "Node";
constexpr char MODULE[] = "Module";
constexpr char DURINGINIT[] = "DuringInit";
constexpr char EXCEPTION[] = "Exception";

// Fields supporting generic events
constexpr char GENERIC_EVENTS[] = "generic_events";
constexpr char TRIGGER_TYPE[] = "TriggerType";
constexpr char SAMPLING[] = "Sampling";
constexpr char ABSOLUTE_THRESHOLD[] = "AbsoluteThreshold";
constexpr char RELATIVE_THRESHOLD[] = "RelativeThreshold";
constexpr char CURRENT_VALUE[] = "CurrentValue";
constexpr char CURRENT_TIME[] = "CurrentTime";
constexpr char TRIGGER_SAMPLING[] = "TriggerSampling";
constexpr char TRIGGER_THRESHOLD[] = "TriggerThreshold";
constexpr char SAMPLING_INTERVAL[] = "SamplingInterval";
constexpr char UPPER_THRESHOLD[] = "UpperThreshold";
constexpr char LOWER_THRESHOLD[] = "LowerThreshold";

// Fields supporting capabilities
constexpr char PROPERTY_CAPABILITIES[] = "PropertyCapabilities";
constexpr char SAMPLING_INTERVAL_MIN[] = "SamplingIntervalMin";
constexpr char SAMPLING_INTERVAL_MAX[] = "SamplingIntervalMax";
constexpr char UPPER_THRESHOLD_MIN[] = "UpperThresholdMin";
constexpr char UPPER_THRESHOLD_MAX[] = "UpperThresholdMax";
constexpr char LOWER_THRESHOLD_MIN[] = "LowerThresholdMin";
constexpr char LOWER_THRESHOLD_MAX[] = "LowerThresholdMax";
constexpr char RELATIVE_THRESHOLD_MIN[] = "RelativeThresholdMin";
constexpr char RELATIVE_THRESHOLD_MAX[] = "RelativeThresholdMax";
constexpr char ROLES[] = "Roles";
constexpr char ROLE_PUBLIC[] = "Public";
constexpr char ROLE_ELEVATED[] = "Elevated";
constexpr char IS_STATIC[] = "Static";

// Fields supporting Framework Load Options
constexpr char FRAMEWORK_LOAD_OPTIONS[] = "framework_load_options";
constexpr char INITIAL_FRAMEWORK_RETRIES[] = "initial_framework_retries";
constexpr char RETRY_DELAY_MS[] = "retry_delay_ms";
constexpr char MINIMUM_FRAMEWORK_VERSION[] = "minimum_framework_version";

// Provider Isolation keywords
constexpr char DISABLE_PROVIDER_ISOLATION[] = "disable_provider_isolation";

// Json schema Keywords
constexpr char SCHEMA_SCHEMA[] = "$schema";
constexpr char SCHEMA_ID[] = "$id";
constexpr char SCHEMA_REF[] = "$ref";
constexpr char SCHEMA_DEFS[] = "$defs";
constexpr char SCHEMA_TITLE[] = "title";
constexpr char SCHEMA_DESCRIPTION[] = "description";
constexpr char TYPE[] = "type";
constexpr char SCHEMA_PROPERTIES[] = "properties";
constexpr char SCHEMA_REQUIRED[] = "required";
constexpr char SCHEMA_ITEMS[] = "items";
constexpr char SCHEMA_ADDITIONAL_PROPERTIES[] = "additionalProperties";
constexpr char SCHEMA_ENUM[] = "enum";
constexpr char SCHEMA_CONST[] = "const";
constexpr char SCHEMA_DEFAULT[] = "default";
constexpr char SCHEMA_ONE_OF[] = "oneOf";
constexpr char SCHEMA_ANY_OF[] = "anyOf";
constexpr char SCHEMA_ALL_OF[] = "allOf";
constexpr char SCHEMA_NOT[] = "not";
constexpr char SCHEMA_IF[] = "if";
constexpr char SCHEMA_THEN[] = "then";
constexpr char SCHEMA_ELSE[] = "else";
constexpr char SCHEMA_MINIMUM[] = "minimum";
constexpr char SCHEMA_MAXIMUM[] = "maximum";
constexpr char SCHEMA_EXCLUSIVE_MINIMUM[] = "exclusiveMinimum";
constexpr char SCHEMA_EXCLUSIVE_MAXIMUM[] = "exclusiveMaximum";
constexpr char SCHEMA_MIN_LENGTH[] = "minLength";
constexpr char SCHEMA_MAX_LENGTH[] = "maxLength";
constexpr char SCHEMA_PATTERN[] = "pattern";
constexpr char SCHEMA_FORMAT[] = "format";
constexpr char SCHEMA_MIN_ITEMS[] = "minItems";
constexpr char SCHEMA_MAX_ITEMS[] = "maxItems";
constexpr char SCHEMA_UNIQUE_ITEMS[] = "uniqueItems";
constexpr char SCHEMA_MIN_PROPERTIES[] = "minProperties";
constexpr char SCHEMA_MAX_PROPERTIES[] = "maxProperties";
constexpr char SCHEMA_DEPENDENCIES[] = "dependencies";
constexpr char SCHEMA_PATTERN_PROPERTIES[] = "patternProperties";
constexpr char SCHEMA_MULTIPLE_OF[] = "multipleOf";
constexpr char SCHEMA_CONTAINS[] = "contains";
constexpr char SCHEMA_PROPERTY_NAMES[] = "propertyNames";
constexpr char SCHEMA_READ_ONLY[] = "readOnly";
constexpr char SCHEMA_WRITE_ONLY[] = "writeOnly";
constexpr char SCHEMA_EXAMPLES[] = "examples";
constexpr char SCHEMA_CONTENT_MEDIA_TYPE[] = "contentMediaType";
constexpr char SCHEMA_CONTENT_ENCODING[] = "contentEncoding";
constexpr char ARRAY_TYPE[] = "array";

// List of Schema Keywords
static const std::set<const char*> SchemaKeywords = {
	SCHEMA_SCHEMA,
	SCHEMA_ID,
	SCHEMA_REF,
	SCHEMA_DEFS,
	SCHEMA_TITLE,
	SCHEMA_DESCRIPTION,
	TYPE,
	SCHEMA_PROPERTIES,
	SCHEMA_REQUIRED,
	SCHEMA_ITEMS,
	SCHEMA_ADDITIONAL_PROPERTIES,
	SCHEMA_ENUM,
	SCHEMA_CONST,
	SCHEMA_DEFAULT,
	SCHEMA_ONE_OF,
	SCHEMA_ANY_OF,
	SCHEMA_ALL_OF,
	SCHEMA_NOT,
	SCHEMA_IF,
	SCHEMA_THEN,
	SCHEMA_ELSE,
	SCHEMA_MINIMUM,
	SCHEMA_MAXIMUM,
	SCHEMA_EXCLUSIVE_MINIMUM,
	SCHEMA_EXCLUSIVE_MAXIMUM,
	SCHEMA_MIN_LENGTH,
	SCHEMA_MAX_LENGTH,
	SCHEMA_PATTERN,
	SCHEMA_FORMAT,
	SCHEMA_MIN_ITEMS,
	SCHEMA_MAX_ITEMS,
	SCHEMA_UNIQUE_ITEMS,
	SCHEMA_MIN_PROPERTIES,
	SCHEMA_MAX_PROPERTIES,
	SCHEMA_DEPENDENCIES,
	SCHEMA_PATTERN_PROPERTIES,
	SCHEMA_MULTIPLE_OF,
	SCHEMA_CONTAINS,
	SCHEMA_PROPERTY_NAMES,
	SCHEMA_READ_ONLY,
	SCHEMA_WRITE_ONLY,
	SCHEMA_EXAMPLES,
	SCHEMA_CONTENT_MEDIA_TYPE,
	SCHEMA_CONTENT_ENCODING};

} // namespace JsonFields
} // namespace Ipf
