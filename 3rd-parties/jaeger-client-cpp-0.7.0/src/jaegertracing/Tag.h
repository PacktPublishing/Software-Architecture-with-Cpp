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

#ifndef JAEGERTRACING_TAG_H
#define JAEGERTRACING_TAG_H

#include "jaegertracing/Compilers.h"

#include <algorithm>
#include <cstdint>
#include <opentracing/string_view.h>
#include <opentracing/value.h>
#include <opentracing/variant/variant.hpp>
#include <string>

namespace jaegertracing {
namespace thrift {
class Tag;
}

class Tag {
  public:
    using ValueType = opentracing::Value;

    template <typename ValueArg>
    Tag(const std::string& key, ValueArg&& value)
        : _key(key)
        , _value(std::forward<ValueArg>(value))
    {
    }

    template <typename ValueArg>
    Tag(const std::pair<std::string, ValueArg>& tag_pair)
        : _key(tag_pair.first)
        , _value(tag_pair.second)
    {
    }

    bool operator==(const Tag& rhs) const
    {
        return _key == rhs._key && _value == rhs._value;
    }

    const std::string& key() const { return _key; }

    const ValueType& value() const { return _value; }

    void thrift(thrift::Tag& tag) const;

  private:
    std::string _key;
    ValueType _value;
};

}  // namespace jaegertracing

#endif  // JAEGERTRACING_TAG_H
