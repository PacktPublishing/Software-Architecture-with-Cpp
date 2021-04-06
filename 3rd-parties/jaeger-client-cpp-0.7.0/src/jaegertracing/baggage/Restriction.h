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

#ifndef JAEGERTRACING_BAGGAGE_RESTRICTION_H
#define JAEGERTRACING_BAGGAGE_RESTRICTION_H

namespace jaegertracing {
namespace baggage {

class Restriction {
  public:
    Restriction(bool keyAllowed, int maxValueLength)
        : _keyAllowed(keyAllowed)
        , _maxValueLength(maxValueLength)
    {
    }

    friend bool operator==(const Restriction& lhs, const Restriction& rhs)
    {
        return lhs._keyAllowed == rhs._keyAllowed &&
               lhs._maxValueLength == rhs._maxValueLength;
    }

    bool keyAllowed() const { return _keyAllowed; }

    int maxValueLength() const { return _maxValueLength; }

  private:
    bool _keyAllowed;
    int _maxValueLength;
};

}  // namespace baggage
}  // namespace jaegertracing

#endif  // JAEGERTRACING_BAGGAGE_RESTRICTION_H
