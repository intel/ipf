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
#include <cstring>

#if defined(_WIN32)

#define ipfef_osal_strncpy_s strncpy_s
#define ipfef_osal_strnlen_s strnlen_s
#define ipfef_osal_memcpy_s memcpy_s

#else

static inline int ipfef_osal_strncpy_s(char* dst, size_t dstsiz, const char* src, size_t count)
{
	UNREFERENCED_PARAMETER(dstsiz);
	strncpy(dst, src, count);
	dst[count - 1] = 0;
	return 0;
}

static inline int ipfef_osal_strnlen_s(const char* str, size_t count)
{
	return strnlen(str, count);
}

static inline void ipfef_osal_memcpy_s(void* dst, size_t dstsiz, void const* src, size_t count)
{
	std::memcpy(dst, src, count);
}

#endif
