//-------------------------------------------------------------------
//  redis-cpp
//  https://github.com/tdv/redis-cpp
//  Created:     03.2020
//  Copyright 2020 Dmitry Tkachenko (tkachenkodmitryv@gmail.com)
//  Distributed under the MIT License
//  (See accompanying file LICENSE)
//-------------------------------------------------------------------

// STD
#include <cstdlib>
#include <iostream>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

int main()
{
    try
    {
        auto stream = rediscpp::make_stream("localhost", "6379");

        int const N = 10;
        auto const key_pref = "my_key_";

        // Executing command 'SET' N times without getting any response
        for (int i = 0 ; i < N ; ++i)
        {
            auto const item = std::to_string(i);
            rediscpp::execute_no_flush(*stream,
                "set", key_pref + item, item , "ex", "60");
        }

        // Flush all
        std::flush(*stream);

        // Getting response for each sended 'SET' request
        for (int i = 0 ; i < N ; ++i)
        {
            rediscpp::value value{*stream};
            std::cout << "Set " << key_pref << i << ": "
                      << value.as<std::string_view>() << std::endl;
        }

        // Executing command 'GET' N times without getting any response
        for (int i = 0 ; i < N ; ++i)
        {
            rediscpp::execute_no_flush(*stream, "get",
                key_pref + std::to_string(i));
        }

        // Flush all
        std::flush(*stream);

        // Getting response for each sended 'GET' request
        for (int i = 0 ; i < N ; ++i)
        {
            rediscpp::value value{*stream};
            std::cout << "Get " << key_pref << i << ": "
                      << value.as<std::string_view>() << std::endl;
        }
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
