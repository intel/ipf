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

#include <memory>
#include <string>

#include "Base.h"
#include "Events.h"
#include "Revision.h"
#include "SchemaType.h"

namespace Ipf
{
class IClientApiR5
{
public:
    virtual ~IClientApiR5() = default;

    static const revision_t revision = CLIENTAPI_REVISION_GETCAPABILITIES;

    virtual std::string GetProperties(const std::string& properties_config = "") = 0;

    virtual std::string GetValue(const std::string& path = "") = 0;
    virtual std::string GetNode(const std::string& path = "") = 0;
    virtual std::string GetSchema(const std::string& path, Ipf::SchemaType schema_type) = 0;
    virtual std::string ExecuteCommand(const std::string& path, const std::string& command) = 0;
    virtual std::string QueryNode(const std::string& path) = 0;

    virtual void RegisterEvent(
        const std::string& path,
        EventCallback cb,
        void* context = nullptr,
        const std::string& event_settings = "null") = 0;
    virtual void UnregisterEvent(const std::string& path, EventCallback cb) = 0;

    virtual void SetValue(const std::string& path, bool value) = 0;
    virtual void SetValue(const std::string& path, int64_t value) = 0;
    virtual void SetValue(const std::string& path, double value) = 0;
    virtual void SetValue(const std::string& path, const std::string& value) = 0;
    virtual void SetValue(const std::string& path, const char* value) = 0;

	virtual std::string GetCapabilities(const std::string& path) = 0;
};
} // namespace Ipf