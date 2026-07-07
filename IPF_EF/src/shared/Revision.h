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

//
// Significant revisions that are checked to enable / disable features
//

// ProviderApi Rev3 v2.0.10100 release - initial revision
// ClientApi Rev3 v2.0.10100 release - initial revision
#define PROVIDERAPI_REVISION_REL_2_0_101_00 3
#define CLIENTAPI_REVISION_REL_2_0_101_00 3

// ProviderApi Rev4 v2.1.10000 release - added Start() method to IProvider
#define PROVIDERAPI_REVISION_REL_2_1_100_00 4
#define CLIENTAPI_REVISION_REL_2_1_100_00 3

// ProviderApi Rev4 v2.1.10100 release
#define PROVIDERAPI_REVISION_REL_2_1_101_00 4
#define CLIENTAPI_REVISION_REL_2_1_101_00 3

// ProviderApi R5 and ClientApi R4- LNL development branch - add event_settings argument to
// RegisterEvent()
#define PROVIDERAPI_REVISION_LNL_DEV 5
#define CLIENTAPI_REVISION_LNL_DEV 4

// Add GetCapabilities to ClientApi and ProviderApi
#define PROVIDERAPI_REVISION_GETCAPABILITIES 6
#define CLIENTAPI_REVISION_GETCAPABILITIES 5

// Add Provider Isolation to ClientApi
#define PROVIDERAPI_REVISION_PROVIDER_ISOLATION 6
#define CLIENTAPI_REVISION_PROVIDER_ISOLATION 6

#define PROVIDERAPI_CURRENT_REVISION PROVIDERAPI_REVISION_PROVIDER_ISOLATION
#define CLIENTAPI_CURRENT_REVISION CLIENTAPI_REVISION_PROVIDER_ISOLATION
