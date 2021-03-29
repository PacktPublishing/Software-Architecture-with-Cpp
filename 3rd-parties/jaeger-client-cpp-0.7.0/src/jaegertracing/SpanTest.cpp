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

#include "jaegertracing/Span.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"
#include <gtest/gtest.h>
#include <string>

namespace jaegertracing {

TEST(Span, testThriftConversion)
{
    const Span span;
    ASSERT_TRUE(span.serviceName().empty());
    ASSERT_TRUE(span.operationName().empty());
    thrift::Span thriftSpan;
    ASSERT_NO_THROW(span.thrift(thriftSpan));
}

}  // namespace jaegertracing
