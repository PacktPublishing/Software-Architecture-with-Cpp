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

#ifndef JAEGERTRACING_SENDER_H
#define JAEGERTRACING_SENDER_H

#include <stdexcept>
#include <string>

#include "jaegertracing/Compilers.h"

namespace jaegertracing {

class Span;

class Sender {
  public:
    class Exception : public std::runtime_error {
      public:
        Exception(const std::string& what, int numFailed)
            : std::runtime_error(what)
            , _numFailed(numFailed)
        {
        }

        int numFailed() const { return _numFailed; }

      private:
        int _numFailed;
    };

    virtual ~Sender() = default;

    virtual int append(const Span& span) = 0;

    virtual int flush() = 0;

    virtual void close() = 0;
};

}  // namespace jaegertracing

#endif  //JAEGERTRACING_SENDER_H
