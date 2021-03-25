//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_STREAM_H_
#define REDISCPP_STREAM_H_

#ifndef REDISCPP_PURE_CORE

// STD
#include <iosfwd>
#include <memory>
#include <string_view>

// REDIS-CPP
#include <redis-cpp/detail/config.h>

namespace rediscpp
{

[[nodiscard]]
std::shared_ptr<std::iostream> make_stream(
        std::string_view host, std::string_view port);

}   // namespace rediscpp

#ifdef REDISCPP_HEADER_ONLY
#include <redis-cpp/detail/stream.hpp>
#endif  // !REDISCPP_HEADER_ONLY

#endif  // !REDISCPP_PURE_CORE

#endif  // !REDISCPP_STREAM_H_
