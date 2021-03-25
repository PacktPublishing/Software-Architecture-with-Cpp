//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_RESP_DETAIL_OVERLOADED_H_
#define REDISCPP_RESP_DETAIL_OVERLOADED_H_

// REDIS-CPP
#include <redis-cpp/detail/config.h>

namespace rediscpp
{
inline namespace resp
{
namespace detail
{

template<typename ... T>
struct overloaded
    : public T ...
{
    using T::operator() ... ;
};

template<typename ... T>
overloaded(T ... ) -> overloaded<T ... >;

}   // namespace detail
}   // namespace resp
}   // namespace rediscpp

#endif  // !REDISCPP_RESP_DETAIL_MARKER_H_
