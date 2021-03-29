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

#ifndef JAEGERTRACING_REFERENCE_H
#define JAEGERTRACING_REFERENCE_H

#include <sstream>
#include <string>

#include <opentracing/propagation.h>

#include "jaegertracing/Compilers.h"
#include "jaegertracing/SpanContext.h"

namespace jaegertracing {
namespace thrift {
class SpanRef;
}

class Reference {
  public:
    using Type = opentracing::SpanReferenceType;

    Reference(const SpanContext& spanContext, Type type)
        : _spanContext(spanContext)
        , _type(type)
    {
    }

    const SpanContext& spanContext() const { return _spanContext; }

    Type type() const { return _type; }

    void thrift(thrift::SpanRef& spanRef) const;

  private:
    SpanContext _spanContext;
    Type _type;
};

}  // namespace jaegertracing

#endif  // JAEGERTRACING_REFERENCE_H
