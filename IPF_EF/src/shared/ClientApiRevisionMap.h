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
#include "Revision.h"

#include <string>

namespace Ipf
{

// Maps a "Major.Minor" framework version string to the corresponding ClientApi revision.
//
// The mapping is:
//   2.0, 2.1 -> CLIENTAPI_REVISION_REL_2_0_101_00 (R3)
//   2.2      -> CLIENTAPI_REVISION_LNL_DEV         (R4)
//   2.3      -> CLIENTAPI_REVISION_GETCAPABILITIES  (R5)
//   2.4      -> CLIENTAPI_REVISION_PROVIDER_ISOLATION (R6)
//
// Throws IpfException if the version string is malformed or does not map to a known revision.
inline revision_t ClientApiRevisionFromVersion(const std::string& version)
{
	// Parse "Major.Minor"
	auto dot = version.find('.');
	if (dot == std::string::npos) {
		throw IpfException(
			"Version " + version + " is invalid. Expected exactly two components: MAJOR.MINOR.");
	}

	std::string majorStr = version.substr(0, dot);
	std::string minorStr = version.substr(dot + 1);

	if (majorStr.empty() || minorStr.empty() ||
		majorStr.find_first_not_of("0123456789") != std::string::npos ||
		minorStr.find_first_not_of("0123456789") != std::string::npos) {
		throw IpfException(
			"Version " + version +
			" is invalid. Expected format: MAJOR.MINOR with numeric components.");
	}

	// Reject leading zeros (e.g. "02.04") to match schema validation rules
	if ((majorStr.size() > 1 && majorStr[0] == '0') ||
		(minorStr.size() > 1 && minorStr[0] == '0')) {
		throw IpfException(
			"Version " + version +
			" is invalid. Components must not have leading zeros.");
	}

	int majorVal, minorVal;
	try {
		majorVal = std::stoi(majorStr);
		minorVal = std::stoi(minorStr);
	}
	catch (const std::out_of_range&) {
		throw IpfException("Version " + version + " is invalid. Components must be in [0..65535].");
	}

	constexpr int MAX_COMPONENT_VALUE = 65535;
	if (majorVal > MAX_COMPONENT_VALUE || minorVal > MAX_COMPONENT_VALUE) {
		throw IpfException("Version " + version + " is invalid. Components must be in [0..65535].");
	}

	// Search version-to-revision mapping table. If not found, it will throw.
	struct VersionRevisionEntry {
		int major;
		int minor;
		revision_t revision;
	};

	static constexpr VersionRevisionEntry VERSION_MAP[] = {
		{IPFSDK_VER_MAJOR, IPFSDK_VER_MINOR, CLIENTAPI_CURRENT_REVISION},
		{2, 4, CLIENTAPI_REVISION_PROVIDER_ISOLATION}, // R6
		{2, 3, CLIENTAPI_REVISION_GETCAPABILITIES}, // R5
		{2, 2, CLIENTAPI_REVISION_LNL_DEV},         // R4
		{2, 1, CLIENTAPI_REVISION_REL_2_1_101_00},  // R3
		{2, 0, CLIENTAPI_REVISION_REL_2_0_101_00},  // R3
	};

	for (const auto& entry : VERSION_MAP) {
		if (majorVal == entry.major && minorVal == entry.minor) {
			return entry.revision;
		}
	}

	throw IpfException("Version " + version + " does not map to a known ClientApi revision.");
}

} // namespace Ipf
