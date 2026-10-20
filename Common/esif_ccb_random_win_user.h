/******************************************************************************
** Copyright (c) 2013-2026 Intel Corporation All Rights Reserved
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

#if defined(ESIF_ATTR_OS_WINDOWS) && defined(ESIF_ATTR_USER)

#include <bcrypt.h>

static ESIF_INLINE esif_error_t esif_ccb_random(void* buffer, size_t buf_len)
{
	esif_error_t rc = ESIF_E_PARAMETER_IS_NULL;
	if (buffer && buf_len > 0) {
		NTSTATUS stat = BCryptGenRandom(
			NULL,
			(PUCHAR)buffer,
			(DWORD)buf_len,
			BCRYPT_USE_SYSTEM_PREFERRED_RNG
		);
		if (stat == 0) {
			rc = ESIF_OK;
		}
		else {
			rc = ESIF_E_UNSPECIFIED;
		}
	}
	return rc;
}

#endif /* WINDOWS USER */