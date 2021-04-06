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

#ifndef JAEGERTRACING_SAMPLERS_SAMPLINGSTATUS_H
#define JAEGERTRACING_SAMPLERS_SAMPLINGSTATUS_H

#include <vector>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/Tag.h"

namespace jaegertracing {
namespace samplers {

class SamplingStatus {
  public:
    SamplingStatus(bool isSampled, const std::vector<Tag>& tags)
        : _isSampled(isSampled)
        , _tags(tags)
    {
    }

    bool isSampled() const { return _isSampled; }

    const std::vector<Tag>& tags() const { return _tags; }

  private:
    bool _isSampled;
    std::vector<Tag> _tags;
};

}  // namespace samplers
}  // namespace jaegertracing

#endif  // JAEGERTRACING_SAMPLERS_SAMPLINGSTATUS_H
