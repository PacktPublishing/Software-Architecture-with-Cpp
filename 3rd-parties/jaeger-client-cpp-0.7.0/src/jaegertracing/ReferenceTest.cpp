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

#include "jaegertracing/Reference.h"
#include "jaegertracing/SpanContext.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"
#include <gtest/gtest.h>
#include <stdexcept>

namespace jaegertracing {

TEST(Reference, testThriftConversion)
{
    const SpanContext context;
    const Reference childRef(context, Reference::Type::ChildOfRef);
    thrift::SpanRef thriftChildRef;
    ASSERT_NO_THROW(childRef.thrift(thriftChildRef));
    const Reference followsFromRef(context, Reference::Type::FollowsFromRef);
    thrift::SpanRef thriftFollowsFromRef;
    ASSERT_NO_THROW(followsFromRef.thrift(thriftFollowsFromRef));
    const Reference invalidRef(context, static_cast<Reference::Type>(-1));
    thrift::SpanRef thriftInvalidRef;
    ASSERT_THROW(invalidRef.thrift(thriftInvalidRef), std::invalid_argument);
}

}  // namespace jaegertracing
