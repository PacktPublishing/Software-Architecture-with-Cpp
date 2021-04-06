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
#include "jaegertracing/Tracer.h"
#include "jaegertracing/baggage/BaggageSetter.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"
#include <cassert>
#include <cstdint>
#include <istream>
#include <memory>
#include <opentracing/value.h>

namespace jaegertracing {
namespace {

struct SamplingPriorityVisitor {
    using result_type = bool;

    bool operator()(bool boolValue) const { return boolValue; }

    bool operator()(double doubleValue) const { return doubleValue > 0; }

    bool operator()(int64_t intValue) const { return intValue > 0; }

    bool operator()(uint64_t uintValue) const { return uintValue > 0; }

    bool operator()(const std::string& str) const
    {
        std::istringstream iss(str);
        auto intValue = 0;
        if (!(iss >> intValue)) {
            return false;
        }
        return intValue > 0;
    }

    bool operator()(std::nullptr_t) const { return false; }

    bool operator()(const char* str) const
    {
        return operator()(std::string(str));
    }

    template <typename OtherType>
    bool operator()(OtherType) const
    {
        return false;
    }
};

}  // anonymous namespace

void Span::SetBaggageItem(opentracing::string_view restrictedKey,
                          opentracing::string_view value) noexcept
{
    std::lock_guard<std::mutex> lock(_mutex);
    const auto& baggageSetter = _tracer->baggageSetter();
    auto baggage = _context.baggage();
    baggageSetter.setBaggage(*this,
                             baggage,
                             restrictedKey,
                             value,
                             [this](std::vector<Tag>::const_iterator first,
                                    std::vector<Tag>::const_iterator last) {
                                 logFieldsNoLocking(SystemClock::now(), first, last);
                             });
    _context = _context.withBaggage(baggage);
}

void Span::FinishWithOptions(
    const opentracing::FinishSpanOptions& finishSpanOptions) noexcept
{
    const auto finishTimeSteady =
        (finishSpanOptions.finish_steady_timestamp == SteadyClock::time_point())
            ? SteadyClock::now()
            : finishSpanOptions.finish_steady_timestamp;
    std::shared_ptr<const Tracer> tracer;
    {

        std::lock_guard<std::mutex> lock(_mutex);
        if (isFinished()) {
            // Already finished, so return immediately.
            return;
        }
        _duration = finishTimeSteady - _startTimeSteady;
        if (_duration <= SteadyClock::duration()) {
            // Enforce minimum duration of 1 tick (1ns on Linux),
            // so isFinished() returns true
            _duration = SteadyClock::duration(1);
        }

        tracer = _tracer;

        std::copy(finishSpanOptions.log_records.begin(),
                  finishSpanOptions.log_records.end(),
                  std::back_inserter(_logs));
    }

    // Call `reportSpan` even for non-sampled traces.
    if (tracer) {
        tracer->reportSpan(*this);
    }
}

const opentracing::Tracer& Span::tracer() const noexcept
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_tracer) {
        return *_tracer;
    }
    auto tracer = opentracing::Tracer::Global();
    assert(tracer);
    return *tracer;
}

std::string Span::serviceName() const noexcept
{
    std::lock_guard<std::mutex> lock(_mutex);
    return serviceNameNoLock();
}

std::string Span::serviceNameNoLock() const noexcept
{
    if (!_tracer) {
        return std::string();
    }
    return _tracer->serviceName();
}

void Span::setSamplingPriority(const opentracing::Value& value)
{
    SamplingPriorityVisitor visitor;
    const auto priority = opentracing::Value::visit(value, visitor);

    std::lock_guard<std::mutex> lock(_mutex);
    auto newFlags = _context.flags();
    if (priority) {
        newFlags |= static_cast<unsigned char>(SpanContext::Flag::kSampled) |
                    static_cast<unsigned char>(SpanContext::Flag::kDebug);
    }
    else {
        newFlags &= ~static_cast<unsigned char>(SpanContext::Flag::kSampled);
    }

    _context = SpanContext(_context.traceID(),
                           _context.spanID(),
                           _context.parentID(),
                           newFlags,
                           _context.baggage(),
                           _context.debugID());
}

void Span::thrift(thrift::Span& span) const
{
    std::lock_guard<std::mutex> lock(_mutex);
    span.__set_traceIdHigh(_context.traceID().high());
    span.__set_traceIdLow(_context.traceID().low());
    span.__set_spanId(_context.spanID());
    span.__set_parentSpanId(_context.parentID());
    span.__set_operationName(_operationName);

    std::vector<thrift::SpanRef> refs;
    refs.reserve(_references.size());
    std::transform(std::begin(_references),
                   std::end(_references),
                   std::back_inserter(refs),
                   [](const Reference& ref) {
                       thrift::SpanRef thriftRef;
                       ref.thrift(thriftRef);
                       return thriftRef;
                   });
    span.__set_references(refs);

    span.__set_flags(_context.flags());
    span.__set_startTime(std::chrono::duration_cast<std::chrono::microseconds>(
                             _startTimeSystem.time_since_epoch())
                             .count());
    span.__set_duration(
        std::chrono::duration_cast<std::chrono::microseconds>(_duration)
            .count());

    std::vector<thrift::Tag> tags;
    tags.reserve(_tags.size());
    std::transform(std::begin(_tags),
                   std::end(_tags),
                   std::back_inserter(tags),
                   [](const Tag& tag) {
                       thrift::Tag thriftTag;
                       tag.thrift(thriftTag);
                       return thriftTag;
                   });
    span.__set_tags(tags);

    std::vector<thrift::Log> logs;
    logs.reserve(_logs.size());
    std::transform(std::begin(_logs),
                   std::end(_logs),
                   std::back_inserter(logs),
                   [](const LogRecord& log) {
                       thrift::Log thriftLog;
                       log.thrift(thriftLog);
                       return thriftLog;
                   });
    span.__set_logs(logs);
}

}  // namespace jaegertracing
