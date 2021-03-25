//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

#ifndef REDISCPP_VALUE_H_
#define REDISCPP_VALUE_H_

// STD
#include <iosfwd>
#include <memory>
#include <string_view>

// REDIS-CPP
#include <redis-cpp/detail/config.h>
#include <redis-cpp/resp/deserialization.h>
#include <redis-cpp/resp/detail/overloaded.h>

namespace rediscpp
{

class value final
{
public:
    using item_type = resp::deserialization::array::item_type;

    value(std::istream &stream)
        : marker_{resp::deserialization::get_mark(stream)}
    {
        switch (marker_)
        {
        case resp::detail::marker::simple_string :
            item_ = std::make_unique<item_type>(resp::deserialization::simple_string{stream});
            break;
        case resp::detail::marker::error_message :
            item_ = std::make_unique<item_type>(resp::deserialization::error_message{stream});
            break;
        case resp::detail::marker::integer :
            item_ = std::make_unique<item_type>(resp::deserialization::integer{stream});
            break;
        case resp::detail::marker::bulk_string :
            item_ = std::make_unique<item_type>(resp::deserialization::bulk_string{stream});
            break;
        case resp::detail::marker::array :
            item_ = std::make_unique<item_type>(resp::deserialization::array{stream});
            break;
        default :
            break;
        }
    }

    value(item_type const &item)
        : marker_{resp::detail::marker::array}
        , item_{std::make_unique<item_type>(item)}
    {
    }

    [[nodiscard]]
    bool empty() const noexcept
    {
        return !item_.operator bool();
    }

    [[nodiscard]]
    item_type const& get() const
    {
        if (empty())
            throw std::runtime_error{"Empty value."};

        return *item_;
    }

    [[nodiscard]]
    bool is_simple_string() const noexcept
    {
        return marker_ == resp::detail::marker::simple_string;
    }

    [[nodiscard]]
    bool is_error_message() const noexcept
    {
        return marker_ == resp::detail::marker::error_message;
    }

    [[nodiscard]]
    bool is_bulk_string() const noexcept
    {
        return marker_ == resp::detail::marker::bulk_string;
    }

    [[nodiscard]]
    bool is_integer() const noexcept
    {
        return marker_ == resp::detail::marker::integer;
    }

    [[nodiscard]]
    bool is_array() const noexcept
    {
        return marker_ == resp::detail::marker::array;
    }

    [[nodiscard]]
    bool is_string() const noexcept
    {
        return is_simple_string() || is_bulk_string();
    }

    [[nodiscard]]
    auto as_error_message() const
    {
        return get_value<std::string_view, resp::deserialization::error_message>();
    }

    [[nodiscard]]
    auto as_simple_string() const
    {
        return get_value<std::string_view, resp::deserialization::simple_string>();
    }

    [[nodiscard]]
    auto as_integer() const
    {
        return get_value<std::int64_t, resp::deserialization::integer>();
    }

    [[nodiscard]]
    auto as_bulk_string() const
    {
        return get_value<std::string_view, resp::deserialization::bulk_string>();
    }

    [[nodiscard]]
    auto as_string() const
    {
        return is_simple_string() ?
                get_value<std::string_view, resp::deserialization::simple_string>() :
                get_value<std::string_view, resp::deserialization::bulk_string>();
    }

    template <typename T>
    operator T () const
    {
        return as<T>();
    }

    template <typename T>
    [[nodiscard]]
    T as() const
    {
        if (empty())
            throw std::runtime_error{"Empty value."};
        if (is_error_message())
            throw std::runtime_error{std::string{as_error_message()}};
        return T{get_value<std::decay_t<T>>()};
    }

private:
    char marker_;
    std::unique_ptr<item_type> item_;

    template <typename T>
    std::enable_if_t<std::is_integral_v<T>, T>
    get_value() const
    {
        return static_cast<T>(as_integer());
    }

    template <typename T>
    std::enable_if_t<
            std::is_same_v<T, std::string_view> ||
            std::is_same_v<T, std::string>, std::string_view>
    get_value() const
    {
        return as_string();
    }

    template <typename T>
    static auto is_null(T &v) noexcept
            -> decltype(v.is_null())
    {
        return v.is_null();
    }

    static  bool is_null(...) noexcept
    {
        return false;
    }

    template <typename R, typename T>
    R get_value() const
    {
        R result;
        std::visit(resp::detail::overloaded{
                [] (auto const &)
                { throw std::bad_cast{}; },
                [&result] (T const &val)
                {
                    if (is_null(val))
                        throw std::logic_error("You can't cast Null to any type.");
                    result = val.get();
                }
            }, get());

        return result;
    }
};

}   // namespace rediscpp

#endif  // !REDISCPP_VALUE_H_
