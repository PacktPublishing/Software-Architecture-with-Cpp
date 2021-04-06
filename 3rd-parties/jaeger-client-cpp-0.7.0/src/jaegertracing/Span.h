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

#ifndef JAEGERTRACING_SPAN_H
#define JAEGERTRACING_SPAN_H

#include <chrono>
#include <memory>
#include <mutex>

#include <opentracing/span.h>

#include "jaegertracing/LogRecord.h"
#include "jaegertracing/Reference.h"
#include "jaegertracing/SpanContext.h"
#include "jaegertracing/Tag.h"

namespace jaegertracing {

class Tracer;

namespace thrift {
class Span;
}

class Span : public opentracing::Span {
  public:
    using SteadyClock = opentracing::SteadyClock;
    using SystemClock = opentracing::SystemClock;

    explicit Span(
        const std::shared_ptr<const Tracer>& tracer = nullptr,
        const SpanContext& context = SpanContext(),
        const std::string& operationName = "",
        const SystemClock::time_point& startTimeSystem = SystemClock::now(),
        const SteadyClock::time_point& startTimeSteady = SteadyClock::now(),
        const std::vector<Tag>& tags = {},
        const std::vector<Reference>& references = {})
        : _tracer(tracer)
        , _context(context)
        , _operationName(operationName)
        , _startTimeSystem(startTimeSystem)
        , _startTimeSteady(startTimeSteady)
        , _duration()
        , _tags(tags)
        , _references(references)
    {
    }

    Span(const Span& span)
    {
        std::lock(_mutex, span._mutex);
        std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
        std::lock_guard<std::mutex> spanLock(span._mutex, std::adopt_lock);

        _tracer = span._tracer;
        _context = span._context;
        _operationName = span._operationName;
        _startTimeSystem = span._startTimeSystem;
        _startTimeSteady = span._startTimeSteady;
        _duration = span._duration;
        _tags = span._tags;
        _logs = span._logs;
        _references = span._references;
    }

    // Pass-by-value intentional to implement copy-and-swap.
    Span& operator=(Span rhs)
    {
        swap(rhs);
        return *this;
    }

    ~Span() { Finish(); }

    void swap(Span& span)
    {
        using std::swap;

        std::lock(_mutex, span._mutex);
        std::lock_guard<std::mutex> lock(_mutex, std::adopt_lock);
        std::lock_guard<std::mutex> spanLock(span._mutex, std::adopt_lock);

        swap(_tracer, span._tracer);
        swap(_context, span._context);
        swap(_operationName, span._operationName);
        swap(_startTimeSystem, span._startTimeSystem);
        swap(_startTimeSteady, span._startTimeSteady);
        swap(_duration, span._duration);
        swap(_tags, span._tags);
        swap(_logs, span._logs);
        swap(_references, span._references);
    }

    friend void swap(Span& lhs, Span& rhs) { lhs.swap(rhs); }

    void thrift(thrift::Span& span) const;

    template <typename Stream>
    void print(Stream& out) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        out << _context;
    }

    std::string operationName() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _operationName;
    }

    SystemClock::time_point startTimeSystem() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _startTimeSystem;
    }

    SteadyClock::time_point startTimeSteady() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _startTimeSteady;
    }

    SteadyClock::duration duration() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _duration;
    }

    std::vector<Tag> tags() const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _tags;
    }

    template <typename... Arg>
    void setOperationName(Arg&&... args)
    {
        SetOperationName(std::forward<Arg>(args)...);
    }

    void FinishWithOptions(const opentracing::FinishSpanOptions&
                               finishSpanOptions) noexcept override;

    void SetOperationName(opentracing::string_view name) noexcept override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (isFinished()) {
            return;
        }
        _operationName = name;
    }

    void SetTag(opentracing::string_view key,
                const opentracing::Value& value) noexcept override
    {
        if (key == "sampling.priority") {
            setSamplingPriority(value);
            return;
        }
        std::lock_guard<std::mutex> lock(_mutex);
        if (isFinished() || !_context.isSampled()) {
            return;
        }
        _tags.push_back(Tag(key, value));
    }

    void SetBaggageItem(opentracing::string_view restrictedKey,
                        opentracing::string_view value) noexcept override;

    std::string BaggageItem(opentracing::string_view restrictedKey) const
        noexcept override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto itr = _context.baggage().find(restrictedKey);
        return (itr == std::end(_context.baggage())) ? std::string()
                                                     : itr->second;
    }

    void Log(opentracing::SystemTime timestamp,
             std::initializer_list<
                 std::pair<opentracing::string_view, opentracing::Value>>
                 fieldPairs) noexcept override
    {
        doLog(timestamp, fieldPairs);
    }

    void Log(opentracing::SystemTime timestamp,
             const std::vector<
                 std::pair<opentracing::string_view, opentracing::Value>>&
                 fieldPairs) noexcept override
    {
        doLog(timestamp, fieldPairs);
    }

    void Log(std::initializer_list<
             std::pair<opentracing::string_view, opentracing::Value>>
                 fieldPairs) noexcept override
    {
        doLog(SystemClock::now(), fieldPairs);
    }

    const SpanContext& context() const noexcept override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _context;
    }

    const SpanContext& contextNoLock() const noexcept { return _context; }

    const opentracing::Tracer& tracer() const noexcept override;

    std::string serviceName() const noexcept;

    std::string serviceNameNoLock() const noexcept;

  private:
    bool isFinished() const { return _duration != SteadyClock::duration(); }

    template <typename FieldIterator>
    void logFieldsNoLocking(const std::chrono::system_clock::time_point& timestamp, FieldIterator first, FieldIterator last) noexcept
    {
        LogRecord log(timestamp, first, last);
        _logs.push_back(log);
    }

    template <typename Container>
    void doLog(opentracing::SystemTime timestamp, Container fieldPairs) noexcept
    {
        std::lock_guard<std::mutex> lock(_mutex);
        if (!_context.isSampled()) {
            return;
        }

        std::vector<Tag> fields;
        fields.reserve(fieldPairs.size());
        std::transform(
            std::begin(fieldPairs),
            std::end(fieldPairs),
            std::back_inserter(fields),
            [](const std::pair<opentracing::string_view, opentracing::Value>&
                   pair) { return Tag(pair.first, pair.second); });
        logFieldsNoLocking(timestamp, std::begin(fields), std::end(fields));
    }

    void setSamplingPriority(const opentracing::Value& value);

    std::shared_ptr<const Tracer> _tracer;
    SpanContext _context;
    std::string _operationName;
    SystemClock::time_point _startTimeSystem;
    SteadyClock::time_point _startTimeSteady;
    SteadyClock::duration _duration;
    std::vector<Tag> _tags;
    std::vector<LogRecord> _logs;
    std::vector<Reference> _references;
    mutable std::mutex _mutex;
};

}  // namespace jaegertracing

inline std::ostream& operator<<(std::ostream& out,
                                const jaegertracing::Span& span)
{
    span.print(out);
    return out;
}

#endif  // JAEGERTRACING_SPAN_H
