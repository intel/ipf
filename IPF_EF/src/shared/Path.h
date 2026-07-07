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

#include <sstream>
#include <string>
#include <vector>

namespace Ipf
{

constexpr char SEPARATOR_CHAR = '.';

inline std::vector<std::string> SplitPath(const std::string& path)
{
	std::vector<std::string> tokens{};
	if (path.empty()) {
		return tokens;
	}
	std::string token;
	std::istringstream iss(path);

	while (std::getline(iss, token, SEPARATOR_CHAR)) {
		tokens.push_back(token);
	}
	return tokens;
}

inline std::string JoinPath(
	std::vector<std::string>::const_iterator beginIt,
	std::vector<std::string>::const_iterator endIt,
	char c)
{
	std::string joined;
	for (std::vector<std::string>::const_iterator it = beginIt; it != endIt; ++it) {
		joined += *it;
		if (it != endIt - 1) {
			joined += c;
		}
	}
	return joined;
}

inline std::string
RelativePath(const std::string& parent_full_path, const std::string& child_full_path)
{
	std::string::size_type i = child_full_path.find(parent_full_path + SEPARATOR_CHAR);
	std::string relative_path = child_full_path;
	if (i == 0) {
		relative_path.erase(0, parent_full_path.length() + 1);
	}
	return relative_path;
}

/**
 * @brief Appends a path component to a base path with '.' delimiter
 * @param basePath The base path (can be empty)
 * @param component The component to append
 * @return The concatenated path with '.' delimiter, or just component if basePath is empty
 */
inline std::string AppendPath(const std::string& basePath, const std::string& component)
{
	return basePath.empty() ? component : basePath + SEPARATOR_CHAR + component;
}

} // namespace Ipf