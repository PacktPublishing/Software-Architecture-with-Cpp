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

#include "jaegertracing/LogRecord.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"

namespace jaegertracing {
void LogRecord::thrift(thrift::Log& log) const
{
    log.__set_timestamp(std::chrono::duration_cast<std::chrono::microseconds>(
                            _timestamp.time_since_epoch())
                            .count());

    std::vector<thrift::Tag> fields;
    fields.reserve(_fields.size());
    std::transform(std::begin(_fields),
                   std::end(_fields),
                   std::back_inserter(fields),
                   [](const Tag& tag) {
                       thrift::Tag thriftTag;
                       tag.thrift(thriftTag);
                       return thriftTag;
                   });
    log.__set_fields(fields);
}
}  // namespace jaegertracing
