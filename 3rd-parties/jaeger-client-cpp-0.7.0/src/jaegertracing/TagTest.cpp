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

#include "jaegertracing/Tag.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"
#include <gtest/gtest.h>
#include <string>

namespace jaegertracing {

TEST(Tag, testThriftConversion)
{
    const Tag tags[] = { { "testBool", true },
                         { "testDouble", 0.0 },
                         { "testInt64", 0LL },
                         { "testUint64", 0ULL },
                         { "testStr", std::string{ "test" } },
                         { "testNull", nullptr },
                         { "testCStr", "test" } };

    for (auto&& tag : tags) {
        thrift::Tag thriftTag;
        ASSERT_NO_THROW(tag.thrift(thriftTag));
    }
}

}  // namespace jaegertracing
