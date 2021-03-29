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

#include "jaegertracing/Tag.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"

namespace jaegertracing {
class ThriftVisitor {
  public:
    using result_type = void;

    explicit ThriftVisitor(thrift::Tag& tag)
        : _tag(tag)
    {
    }

    void operator()(const std::string& value) const { setString(value); }

    void operator()(const char* value) const { setString(value); }

    void operator()(double value) const
    {
        _tag.__set_vType(thrift::TagType::DOUBLE);
        _tag.__set_vDouble(value);
    }

    void operator()(bool value) const
    {
        _tag.__set_vType(thrift::TagType::BOOL);
        _tag.__set_vBool(value);
    }

    void operator()(int64_t value) const { setLong(value); }

    void operator()(uint64_t value) const { setLong(value); }

    template <typename Arg>
    void operator()(Arg&& value) const
    {
        // No-op
    }

  private:
    void setString(opentracing::string_view value) const
    {
        _tag.__set_vType(thrift::TagType::STRING);
        _tag.__set_vStr(value);
    }

    void setLong(int64_t value) const
    {
        _tag.__set_vType(thrift::TagType::LONG);
        _tag.__set_vLong(value);
    }

    thrift::Tag& _tag;
};

void Tag::thrift(thrift::Tag& tag) const
{
    tag.__set_key(_key);
    ThriftVisitor visitor(tag);
    opentracing::util::apply_visitor(visitor, _value);
}

}  // namespace jaegertracing