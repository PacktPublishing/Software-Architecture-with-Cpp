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

#include "jaegertracing/propagation/Propagator.h"
#include "jaegertracing/SpanContext.h"
#include "jaegertracing/TraceID.h"
#include <gtest/gtest.h>
#include <iosfwd>
#include <random>
#include <stddef.h>
#include <string>
#include <unordered_map>

namespace jaegertracing {
namespace propagation {
namespace {

template <typename RandomGenerator>
std::string randomStr(RandomGenerator& gen)
{
    constexpr auto kMaxSize = 10;
    static constexpr char kLetters[] =
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "0123456789";
    constexpr auto kNumLetters = sizeof(kLetters) - 1;
    const auto size = gen() % kMaxSize;
    std::string result(size, '\0');
    for (auto i = static_cast<size_t>(0); i < size; ++i) {
        const auto pos = gen() % kNumLetters;
        result[i] = kLetters[pos];
    }
    return result;
}

}  // anonymous namespace

TEST(Propagator, testBinaryPropagation)
{
    std::stringstream ss;
    BinaryPropagator binaryPropagator;
    std::random_device randomDevice;
    std::default_random_engine engine(randomDevice());
    constexpr auto kMaxNumBaggageItems = 10;
    const auto numBaggageItems = engine() % (kMaxNumBaggageItems - 1) + 1;
    SpanContext::StrMap baggage;
    for (auto i = static_cast<size_t>(0); i < numBaggageItems; ++i) {
        const auto key = randomStr(engine);
        const auto value = randomStr(engine);
        baggage[key] = value;
    }
    SpanContext ctx(
        TraceID(engine(), engine()), engine(), engine(), 0, baggage);
    binaryPropagator.inject(ctx, ss);
    ASSERT_EQ(ctx, binaryPropagator.extract(ss));
}

}  // namespace propagation
}  // namespace jaegertracing
