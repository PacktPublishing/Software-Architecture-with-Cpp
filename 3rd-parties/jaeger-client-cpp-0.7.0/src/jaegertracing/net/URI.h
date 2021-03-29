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

#ifndef JAEGERTRACING_NET_URI_H
#define JAEGERTRACING_NET_URI_H

#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include "jaegertracing/Compilers.h"

namespace jaegertracing {
namespace net {

struct URI {
    using QueryValueMap = std::unordered_multimap<std::string, std::string>;

    static URI parse(const std::string& uriStr);

    static std::string queryEscape(const std::string& input);

    static std::string queryUnescape(const std::string& input);

    URI()
        : _scheme()
        , _host()
        , _port(0)
        , _path()
        , _query()
    {
    }

    std::string authority() const
    {
        if (_port != 0) {
            return _host + ':' + std::to_string(_port);
        }
        return _host;
    }

    std::string target() const
    {
        auto result = _path;
        if (result.empty()) {
            result = "/";
        }
        if (!_query.empty()) {
            result += '?' + _query;
        }
        return result;
    }

    template <typename Stream>
    void print(Stream& out) const
    {
        out << "{ scheme=\"" << _scheme << '"' << ", host=\"" << _host << '"'
            << ", port=" << _port << ", path=\"" << _path << '"' << ", query=\""
            << _query << '"' << " }";
    }

    QueryValueMap parseQueryValues() const;

    std::string _scheme;
    std::string _host;
    int _port;
    std::string _path;
    std::string _query;
};

}  // namespace net
}  // namespace jaegertracing

inline std::ostream& operator<<(std::ostream& out,
                                const jaegertracing::net::URI& uri)
{
    uri.print(out);
    return out;
}

#endif  // JAEGERTRACING_NET_URI_H
