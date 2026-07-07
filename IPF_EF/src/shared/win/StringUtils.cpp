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

#include "StringUtils.h"

#include "Exceptions.h"

#include <Windows.h>

namespace Ipf
{
namespace Utility
{

std::wstring Utf8ToUtf16(const std::string& utf8_str)
{
	if (utf8_str.empty()) {
		return {};
	}

	int wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, nullptr, 0);
	if (wide_size == 0) {
		throw Ipf::IpfException("MultiByteToWideChar failed", GetLastError());
	}

	std::wstring utf16_str(wide_size, 0);
	MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str(), -1, &utf16_str[0], wide_size);

	utf16_str.pop_back();

	return utf16_str;
}

std::string Utf16ToUtf8(const std::wstring& utf16_str)
{
	if (utf16_str.empty()) {
		return {};
	}

	int utf8_size =
		WideCharToMultiByte(CP_UTF8, 0, utf16_str.c_str(), -1, nullptr, 0, nullptr, nullptr);
	if (utf8_size == 0) {
		throw Ipf::IpfException("WideCharToMultiByte failed", GetLastError());
	}

	std::string utf8_str(utf8_size, 0);
	WideCharToMultiByte(
		CP_UTF8, 0, utf16_str.c_str(), -1, &utf8_str[0], utf8_size, nullptr, nullptr);

	utf8_str.pop_back();

	return utf8_str;
}

} // namespace Utility
} // namespace Ipf
