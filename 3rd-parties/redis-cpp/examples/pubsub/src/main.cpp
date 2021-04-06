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
#include <thread>

// BOOST
#include <boost/thread.hpp>

#include <redis-cpp/stream.h>
#include <redis-cpp/execute.h>

int main()
{
    try
    {
        auto const N = 100;
        auto const queue_name = "test_queue";

        bool volatile stopped = false;

        // A message printer. The message from a queue.
        auto print_message = [] (auto const &value)
        {
            using namespace rediscpp::resp::deserialization;
            std::visit(rediscpp::resp::detail::overloaded{
                   [] (bulk_string const &val)
                   { std::cout << val.get() << std::endl; },
                   [] (auto const &)
                   { std::cout << "Unexpected value type." << std::endl; }
               }, value);
        };

        // The subscriber is runned in its own thread.
        // It's some artificial example, when publisher
        // and subscriber are working in one process.
        // It's only for demonstration library abilities.
        boost::thread subscriber{
            [&stopped, &queue_name, &print_message]
            {
                // Its own stream for a subscriber
                auto stream = rediscpp::make_stream("localhost", "6379");
                auto response = rediscpp::execute(*stream, "subscribe", queue_name);
                // An almost endless loop for getting messages from the queues.
                while (!stopped)
                {
                    // Reading / waiting for a message.
                    rediscpp::value value{*stream};
                    // Message extraction.
                    std::visit(rediscpp::resp::detail::overloaded{
                            // We're wondered only an array in response.
                            // Otherwise, there is an error.
                            [&print_message] (rediscpp::resp::deserialization::array const &arr)
                            {
                                std::cout << "-------- Message --------" << std::endl;
                                for (auto const &i : arr.get())
                                    print_message(i);
                                std::cout << "-------------------------" << std::endl;
                            },
                            // Oops. An error in a response.
                            [] (rediscpp::resp::deserialization::error_message const &err)
                            { std::cerr << "Error: " << err.get() << std::endl; },
                            // An unexpected response.
                            [] (auto const &)
                            { std::cout << "Unexpected value type." << std::endl; }
                        }, value.get());
                }
            }
        };

        // An artificial delay. It's not necessary in real code.
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

        // Its own stream for a publisher.
        auto stream = rediscpp::make_stream("localhost", "6379");

        // The publishing N messages.
        for (int i = 0 ; i < N ; ++i)
        {
            auto response = rediscpp::execute(*stream,
                    "publish", queue_name, std::to_string(i));
            std::cout << "Delivered to " << response.as<std::int64_t>()
                      << " subscribers." << std::endl;
        }

        // An artificial delay. It's not necessary in real code.
        // It's due to the artificiality of the example,
        // where everything is in one process.
        std::this_thread::sleep_for(std::chrono::milliseconds{200});

        stopped = true;
        std::this_thread::sleep_for(std::chrono::milliseconds{200});
        // Why not?... Please, avoid it in real code.
        // It's justified only in examples.
        subscriber.interrupt();
    }
    catch (std::exception const &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
