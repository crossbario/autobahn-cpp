///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 2014 Tavendo GmbH
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "../parameters.hpp"

#include <autobahn/autobahn.hpp>
#include <boost/asio.hpp>
#include <boost/version.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <tuple>

int main(int argc, char** argv)
{
    try {
        boost::asio::io_service io_service;
        auto parameters = get_parameters(argc, argv);
        auto endpoint = parameters->rawsocket_endpoint();
        auto realm = parameters->realm();
        auto debug = parameters->debug();

        boost::future<void> started;
        boost::future<void> called;
        boost::future<void> stopped;

        std::unique_ptr<autobahn::wamp_tcp_component> component(
                new autobahn::wamp_tcp_component(
                        io_service, endpoint, realm, debug));

        started = component->start().then([&](boost::future<void>) {
            autobahn::wamp_call_options call_options;
            call_options.set_timeout(std::chrono::seconds(10));

            auto session = component->session();
            std::tuple<uint64_t, uint64_t> arguments(23, 777);
            called = session->call("com.examples.calculator.add", arguments, call_options).then(
                [&](boost::future<autobahn::wamp_call_result> result) {
                    try {
                        uint64_t sum = result.get().argument<uint64_t>(0);
                        std::cerr << "call result: " << sum << std::endl;
                    } catch (const std::exception& e) {
                        std::cerr << "call failed: " << e.what() << std::endl;
                    }

                    stopped = component->stop().then([&](boost::future<void>) {
                        io_service.stop();
                    });
            });
        });

        io_service.run();
        stopped.wait();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    return 0;
}
