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

// Event callback declaration
//
// The context pointer is stored when an event is registered and is
// passed to the EventCallback when an event is raised.  It may be
// used to store an object's "this" pointer or other context.
//
// Each callback will be called on a separate thread.
// The implementer of the EventCallback must make sure that it is
// thread-safe.
typedef void (*EventCallback)(const char* event_name, const char* event_data, void* context);
