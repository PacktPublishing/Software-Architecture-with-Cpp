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

#include "jaegertracing/SpanContext.h"

#include "jaegertracing/utils/HexParsing.h"

namespace jaegertracing {

SpanContext SpanContext::fromStream(std::istream& in)
{
    SpanContext spanContext;
    spanContext._traceID = TraceID::fromStream(in);
    if (!spanContext._traceID.isValid()) {
        return SpanContext();
    }

    char ch = '\0';
    if (!in.get(ch) || ch != ':') {
        return SpanContext();
    }

    constexpr auto kMaxUInt64Chars = static_cast<size_t>(16);
    auto buffer = utils::HexParsing::readSegment(in, kMaxUInt64Chars, ':');
    if (buffer.empty()) {
        return SpanContext();
    }
    spanContext._spanID = utils::HexParsing::decodeHex<uint64_t>(buffer);

    if (!in.get(ch) || ch != ':') {
        return SpanContext();
    }

    buffer = utils::HexParsing::readSegment(in, kMaxUInt64Chars, ':');
    if (buffer.empty()) {
        return SpanContext();
    }
    spanContext._parentID = utils::HexParsing::decodeHex<uint64_t>(buffer);

    if (!in.get(ch) || ch != ':') {
        return SpanContext();
    }

    constexpr auto kMaxByteChars = static_cast<size_t>(2);
    buffer = utils::HexParsing::readSegment(in, kMaxByteChars, ':');
    if (buffer.empty()) {
        return SpanContext();
    }
    spanContext._flags = utils::HexParsing::decodeHex<unsigned char>(buffer);

    in.clear();
    return spanContext;
}

}  // namespace jaegertracing
