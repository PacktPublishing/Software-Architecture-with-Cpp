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

#ifndef JAEGERTRACING_SPANCONTEXT_H
#define JAEGERTRACING_SPANCONTEXT_H

#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include <opentracing/span.h>

#include "jaegertracing/Compilers.h"

#include "jaegertracing/TraceID.h"

namespace jaegertracing {

class SpanContext : public opentracing::SpanContext {
  public:
    using StrMap = std::unordered_map<std::string, std::string>;

    enum class Flag : unsigned char { kSampled = 1, kDebug = 2 };

    static SpanContext fromStream(std::istream& in);

    SpanContext()
        : _traceID(0, 0)
        , _spanID(0)
        , _parentID(0)
        , _flags(0)
        , _mutex()
    {
    }

    SpanContext(const TraceID& traceID,
                uint64_t spanID,
                uint64_t parentID,
                unsigned char flags,
                const StrMap& baggage,
                const std::string& debugID = "",
                const std::string& traceState = "")
        : _traceID(traceID)
        , _spanID(spanID)
        , _parentID(parentID)
        , _flags(flags)
        , _baggage(baggage)
        , _debugID(debugID)
        , _traceState(traceState)
        , _mutex()
    {
    }

    SpanContext(const SpanContext& ctx)
        : _traceID(ctx._traceID)
        , _spanID(ctx._spanID)
        , _parentID(ctx._parentID)
        , _flags(ctx._flags)
        , _baggage(ctx._baggage)
        , _debugID(ctx._debugID)
        , _traceState(ctx._traceState)
    {
    }

    SpanContext& operator=(SpanContext rhs)
    {
        swap(rhs);
        return *this;
    }

    void swap(SpanContext& ctx)
    {
        using std::swap;
        swap(_traceID, ctx._traceID);
        swap(_spanID, ctx._spanID);
        swap(_parentID, ctx._parentID);
        swap(_flags, ctx._flags);
        swap(_baggage, ctx._baggage);
        swap(_debugID, ctx._debugID);
        swap(_traceState, ctx._traceState);
    }

    friend void swap(SpanContext& lhs, SpanContext& rhs) { lhs.swap(rhs); }

    const TraceID& traceID() const { return _traceID; }

    uint64_t spanID() const { return _spanID; }

    uint64_t parentID() const { return _parentID; }

    const StrMap& baggage() const { return _baggage; }

    SpanContext withBaggage(const StrMap& baggage) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return SpanContext(_traceID,
                           _spanID,
                           _parentID,
                           _flags,
                           baggage,
                           _debugID,
                           _traceState);
    }

    template <typename Function>
    void forEachBaggageItem(Function f) const
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto&& pair : _baggage) {
            if (!f(pair.first, pair.second)) {
                break;
            }
        }
    }

    template <typename Function>
    void forEachBaggageItem(Function f)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        for (auto&& pair : _baggage) {
            if (!f(pair.first, pair.second)) {
                break;
            }
        }
    }

    unsigned char flags() const { return _flags; }

    const std::string& debugID() const { return _debugID; }

    bool isSampled() const
    {
        return _flags & static_cast<unsigned char>(Flag::kSampled);
    }

    const std::string& traceState() const { return _traceState; }

    bool isDebug() const
    {
        return _flags & static_cast<unsigned char>(Flag::kDebug);
    }

    bool isDebugIDContainerOnly() const
    {
        return !_traceID.isValid() && !_debugID.empty();
    }

    bool isValid() const { return _traceID.isValid() && _spanID != 0; }

    template <typename Stream>
    void print(Stream& out) const
    {
        _traceID.print(out);
        out << ':' << std::setw(16) << std::setfill('0') << std::hex << _spanID
            << ':' << std::setw(16) << std::setfill('0') << std::hex << _parentID
            << ':' << std::hex << static_cast<size_t>(_flags);
    }

    void ForeachBaggageItem(
        std::function<bool(const std::string& key, const std::string& value)> f)
        const override
    {
        forEachBaggageItem(f);
    }

    std::unique_ptr<opentracing::SpanContext> Clone() const noexcept override
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return std::unique_ptr<opentracing::SpanContext>(
            new SpanContext(*this));
    }

    friend bool operator==(const SpanContext& lhs, const SpanContext& rhs)
    {
        {
            std::lock(lhs._mutex, rhs._mutex);
            std::lock_guard<std::mutex> lhsLock(lhs._mutex, std::adopt_lock);
            std::lock_guard<std::mutex> rhsLock(rhs._mutex, std::adopt_lock);
            if (lhs._baggage != rhs._baggage) {
                return false;
            }
        }
        return lhs._traceID == rhs._traceID && lhs._spanID == rhs._spanID &&
               lhs._parentID == rhs._parentID && lhs._flags == rhs._flags &&
               lhs._debugID == rhs._debugID &&
               lhs._traceState == rhs._traceState;
    }

    friend bool operator!=(const SpanContext& lhs, const SpanContext& rhs)
    {
        return !operator==(lhs, rhs);
    }

  private:
    TraceID _traceID;
    uint64_t _spanID;
    uint64_t _parentID;
    unsigned char _flags;
    StrMap _baggage;
    std::string _debugID;
    std::string _traceState;
    mutable std::mutex _mutex;  // Protects _baggage.
};

}  // namespace jaegertracing

inline std::ostream& operator<<(std::ostream& out,
                                const jaegertracing::SpanContext& spanContext)
{
    spanContext.print(out);
    return out;
}

inline std::istream& operator>>(std::istream& in,
                                jaegertracing::SpanContext& spanContext)
{
    spanContext = jaegertracing::SpanContext::fromStream(in);
    return in;
}

#endif  // JAEGERTRACING_SPANCONTEXT_H
