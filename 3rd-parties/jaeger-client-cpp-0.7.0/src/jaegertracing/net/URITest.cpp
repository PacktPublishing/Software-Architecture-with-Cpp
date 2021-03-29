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

#include "jaegertracing/net/URI.h"
#include <gtest/gtest.h>
#include <iosfwd>
#include <iterator>
#include <string>
#include <unordered_map>
#include <utility>

namespace jaegertracing {
namespace net {

TEST(URI, testMatch)
{
    ASSERT_EQ(0, URI::parse("http://localhost")._port);
    ASSERT_EQ(80, URI::parse("http://localhost:80")._port);
}

TEST(URI, testAuthority)
{
    ASSERT_EQ("localhost", URI::parse("http://localhost").authority());
    ASSERT_EQ("localhost:80", URI::parse("http://localhost:80").authority());
}

TEST(URI, testPrint)
{
    std::ostringstream oss;
    oss << URI::parse("localhost");
    ASSERT_EQ(
        "{ scheme=\"\""
        ", host=\"\""
        ", port=0"
        ", path=\"localhost\""
        ", query=\"\" }",
        oss.str());
}

TEST(URI, queryEscape)
{
    ASSERT_EQ("hello%20world", URI::queryEscape("hello world"));
    ASSERT_EQ("hello-world", URI::queryEscape("hello-world"));
    ASSERT_EQ("hello.world", URI::queryEscape("hello.world"));
    ASSERT_EQ("hello_world", URI::queryEscape("hello_world"));
    ASSERT_EQ("hello~world", URI::queryEscape("hello~world"));
    ASSERT_EQ("hello%3Aworld", URI::queryEscape("hello:world"));
}

TEST(URI, queryUnescape)
{
    ASSERT_EQ("hello world", URI::queryUnescape("hello%20world"));
    ASSERT_EQ("hello%2world", URI::queryUnescape("hello%2world"));
    ASSERT_EQ("hello%world", URI::queryUnescape("hello%world"));
    ASSERT_EQ("hello world", URI::queryUnescape("hello w%6Frld"));
    ASSERT_EQ("hello world", URI::queryUnescape("hello w%6frld"));
}

TEST(URI, testParseQueryValues)
{
    {
        const auto uri = URI::parse(
            "http://example.com?test1=1&test2=%6F&test2=r&test3&test4");
        const auto values = uri.parseQueryValues();
        ASSERT_EQ(5, values.size());

        const auto test1 = values.equal_range("test1");
        ASSERT_EQ(1, std::distance(test1.first, test1.second));
        ASSERT_EQ("test1", test1.first->first);
        ASSERT_EQ("1", test1.first->second);

        const auto test2 = values.equal_range("test2");
        ASSERT_EQ(2, std::distance(test2.first, test2.second));
        auto test2Itr = test2.first;
        ASSERT_EQ("test2", test2Itr->first);
        ASSERT_TRUE(test2Itr->second == "o" || test2Itr->second == "r");
        auto nextTest2Value = (test2Itr->second == "o") ? "r" : "o";
        ++test2Itr;
        ASSERT_EQ("test2", test2Itr->first);
        ASSERT_EQ(nextTest2Value, test2Itr->second);

        const auto test3 = values.equal_range("test3");
        ASSERT_EQ(1, std::distance(test3.first, test3.second));
        ASSERT_EQ("test3", test3.first->first);
        ASSERT_TRUE(test3.first->second.empty());

        const auto test4 = values.equal_range("test4");
        ASSERT_EQ(1, std::distance(test4.first, test4.second));
        ASSERT_EQ("test4", test4.first->first);
        ASSERT_TRUE(test4.first->second.empty());
    }

    ASSERT_TRUE(URI::parse("http://example.com").parseQueryValues().empty());

    {
        const auto uri = URI::parse("http://example.com?key=value");
        const auto values = uri.parseQueryValues();
        ASSERT_EQ(1, values.size());
        ASSERT_EQ("key", std::begin(values)->first);
        ASSERT_EQ("value", std::begin(values)->second);
    }
}

}  // namespace net
}  // namespace jaegertracing
