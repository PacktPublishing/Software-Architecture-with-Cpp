/*
 * Copyright (c) 2017 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAEGERTRACING_BAGGAGE_RESTRICTIONMANAGER_H
#define JAEGERTRACING_BAGGAGE_RESTRICTIONMANAGER_H

#include <string>

#include "jaegertracing/baggage/Restriction.h"

namespace jaegertracing {
namespace baggage {

class RestrictionManager {
  public:
    virtual ~RestrictionManager() = default;

    virtual Restriction getRestriction(const std::string& service,
                                       const std::string& key) = 0;

    virtual void close() noexcept {}
};

class DefaultRestrictionManager : public RestrictionManager {
  public:
    static constexpr auto kDefaultMaxValueLength = 2048;

    explicit DefaultRestrictionManager(int maxValueLength)
        : _defaultRestriction(true,
                              maxValueLength == 0 ? kDefaultMaxValueLength
                                                  : maxValueLength)
    {
    }

    Restriction getRestriction(const std::string& service,
                               const std::string& key) override
    {
        return _defaultRestriction;
    }

  private:
    Restriction _defaultRestriction;
};

}  // namespace baggage
}  // namespace jaegertracing

#endif  // JAEGERTRACING_BAGGAGE_RESTRICTIONMANAGER_H
