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

#ifndef JAEGERTRACING_TRACEID_H
#define JAEGERTRACING_TRACEID_H

#include <cstdint>
#include <iomanip>
#include <iostream>

namespace jaegertracing {

class TraceID {
  public:
    static TraceID fromStream(std::istream& in);

    TraceID()
        : TraceID(0, 0)
    {
    }

    TraceID(uint64_t high, uint64_t low)
        : _high(high)
        , _low(low)
    {
    }

    bool isValid() const { return _high != 0 || _low != 0; }

    template <typename Stream>
    void print(Stream& out) const
    {
        if (_high == 0) {
            out << std::setw(16) << std::setfill('0') << std::hex << _low;
        }
        else {
            out << std::setw(16) << std::setfill('0') << std::hex << _high
                << std::setw(16) << std::setfill('0') << std::hex << _low;
        }
    }

    uint64_t high() const { return _high; }

    uint64_t low() const { return _low; }

    bool operator==(const TraceID& rhs) const
    {
        return _high == rhs._high && _low == rhs._low;
    }

  private:
    uint64_t _high;
    uint64_t _low;
};

}  // namespace jaegertracing

inline std::ostream& operator<<(std::ostream& out,
                                const jaegertracing::TraceID& traceID)
{
    traceID.print(out);
    return out;
}

inline std::istream& operator<<(std::istream& in,
                                jaegertracing::TraceID& traceID)
{
    traceID = jaegertracing::TraceID::fromStream(in);
    return in;
}

#endif  // JAEGERTRACING_TRACEID_H
