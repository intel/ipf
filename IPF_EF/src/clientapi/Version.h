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

#define VERSION_MAJOR 2
#define VERSION_MINOR 4
#define VERSION_PATCH 0

#ifndef RC_INVOKED
#define VERSION_STRINGIZE_IMPL(value) #value
#define VERSION_STRINGIZE(value) VERSION_STRINGIZE_IMPL(value)
#define VERSION_MAJOR_STRING VERSION_STRINGIZE(VERSION_MAJOR)
#define VERSION_MINOR_STRING VERSION_STRINGIZE(VERSION_MINOR)
#define VERSION_PATCH_STRING VERSION_STRINGIZE(VERSION_PATCH)
#else
// Keep these resource-compiler string literals in sync with VERSION_MAJOR,
// VERSION_MINOR, and VERSION_PATCH above.
#define VERSION_MAJOR_STRING "2"
#define VERSION_MINOR_STRING "4"
#define VERSION_PATCH_STRING "0"
#endif

#define VERSION_STRING VERSION_MAJOR_STRING "." VERSION_MINOR_STRING "." VERSION_PATCH_STRING
