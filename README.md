# AutobahnC++

WAMP for C++ on Boost/ASIO.

[![Linux CI](https://github.com/crossbario/autobahn-cpp/workflows/Linux%20CI/badge.svg)](https://github.com/crossbario/autobahn-cpp/actions?query=workflow%3A%22Linux+CI%22)
[![Windows CI](https://github.com/crossbario/autobahn-cpp/workflows/Windows%20CI/badge.svg)](https://github.com/crossbario/autobahn-cpp/actions?query=workflow%3A%22Windows+CI%22)
[![Documentation](https://img.shields.io/badge/docs-latest-brightgreen.svg?style=flat)](https://crossbario.github.io/autobahn-cpp-docs/)
[![Docker toolchain image](https://img.shields.io/badge/docker-ready-blue.svg)](https://hub.docker.com/r/crossbario/autobahn-cpp/)

---

**[See here](https://crossbario.github.io/autobahn-cpp-docs/) for the Autobahn C++ reference documentation.**

Get in touch on our [user forum](https://crossbar.discourse.group/).

---

## Introduction

AutobahnC++ is a subproject of [Autobahn](http://autobahn.ws/) which provides a C++ [WAMP](http://wamp.ws/) implementation that is able to talk WAMP over `stdio` pipes.

 * **Caller**
 * **Callee**
 * **Publisher**
 * **Subscriber**

AutobahnC++ is open-source, licensed under the [Boost Software License](LICENSE).

The API and implementation make use of modern C++ 11 and new asynchronous idioms using (upcoming) features of the standard C++ library, in particular **Futures**, [**Continuations**](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf) and **Lambdas**.

> [Continuations](http://en.wikipedia.org/wiki/Continuation) are *one* way of managing control flow in an asynchronous program. Other styles include: asynchronous [Callbacks](http://en.wikipedia.org/wiki/Callback_%28computer_programming%29), [Coroutines](http://en.wikipedia.org/wiki/Coroutine) (`yield` or `await`), Actors ([Erlang/OTP](http://www.erlang.org/), [Scala](http://www.scala-lang.org/)/[Akka](http://akka.io/) or [Rust](https://www.rust-lang.org/)) and [Transactional memory](http://en.wikipedia.org/wiki/Transactional_Synchronization_Extensions).
>

AutobahnC++ supports running WAMP (`rawsocket-msgpack`) over **TCP(-TLS)**, **Unix domain sockets** or **pipes** (`stdio`). The library is "header-only", light-weight (< 2k code lines) and **depends on** the following:

 * C++11 compiler
 * [`boost::future`](http://www.boost.org/doc/libs/1_66_0/doc/html/thread/synchronization.html#thread.synchronization.futures)
 * [`boost::asio`](http://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio.html)
 * [`msgpack-c`](https://github.com/msgpack/msgpack-c)
 * [`WebSocket++`](https://github.com/zaphoyd/websocketpp)

For getting help, questions or feedback, get in touch on the **[mailing list](https://groups.google.com/forum/#!forum/autobahnws)**, **[Twitter](https://twitter.com/autobahnws)** or **IRC `#autobahn`** (Freenode).

---


## Show me some code!

Here is how programming with C++ and AutobahnC++ looks like.

**Calling a remote Procedure**

```c++
auto c1 = session.call("com.mathservice.add2", std::make_tuple(23, 777))
    .then([&](boost::future<wamp_call_result> result) {
        std::cout << "Got call result " << result.get().argument<uint64_t>(0) << std::endl;
    });
```

**Registering a remoted Procedure**
```c++
auto r1 = session.provide("com.myapp.cpp.square",
    [](autobahn::wamp_invocation invocation) {
        std::cout << "Procedure is invoked .." << endl;
        uint64_t x = invocation->argument<uint64_t>(0);
        return x * x;
    })
    .then([](boost::future<autobahn::wamp_registration> reg) {
        std::cout << "Registered with ID " << reg.get().id() << std::endl;
    });
```

**Publishing an Event**

```c++
session.publish("com.myapp.topic2", std::make_tuple(23, true, std::string("hello")));
```

**Publishing an Event (acknowledged)**

```c++
auto opts = PublishOptions();
opts.acknowledge = True;

session.publish("com.myapp.topic2", std::make_tuple(23, true, std::string("hello")), opts)
    .then([](boost::future<autobahn::wamp_publication> pub) {
        std::cout << "Published with ID " << pub.get().id() << std::endl;
    });
```

**Subscribing to a Topic**

```c++
auto s1 = session.subscribe("com.myapp.topic1",
    [](const autobahn::wamp_event& event) {
        std::cout << "Got event: " << event.argument<uint64_t>(0) << std::endl;
    })
    .then([](boost::future<autobahn::wamp_subscription> sub) {
        std::cout << "Subscribed with ID " << sub.get().id() << std::endl;
    });
```


Here is JavaScript running in Chrome call into C++ running on command line. Both are connected via a WAMP router, in this case [**Autobahn**|Python](http://autobahn.ws/python) based.

![](doc/_static/cpp_from_js.png)

* [Example C++ code](https://github.com/crossbario/autobahn-cpp/blob/master/examples/register2.cpp)
* [Example JavaScript code](https://github.com/crossbario/autobahn-cpp/blob/master/examples/web/index.html)


## Installation

Autobahn C++ is a "header-only" library, which means there isn't anything to build (for the library itself), and the only thing to install are the header files.

For using Autobahn C++ in your project, clone the project, checkout a Git release tag (optional) and copy the library header files to your system (or project include directory):

```console
cd ~
git clone https://github.com/crossbario/autobahn-cpp.git
cd autobahn-cpp
cp -r autobahn/ /usr/local/include/
```

---


## Examples

The Autobahn C++ repository contains a number of [examples](examples) that demonstrate all 4 basic patterns of using WAMP.

There are also examples for WAMP-CRA and Unix domain sockets.


### Building dependencies

The instructions below were tested on Debian/Ubuntu and build and install the following:

* Boost in `/opt/boost`
* MsgPack-C in `/opt/msgpackc`
* WebSocketC++ in `/opt/websocketpp`
* AutobahnC++ in `/opt/autobahncpp`

> *Notes*
>
> * The library code is written in standard C++ 11. Target toolchains currently include **clang** and **gcc**. Support for MSVC is tracked on this [issue](https://github.com/crossbario/autobahn-cpp/issues/2).
> * While C++ 11 includes `std::future` in the standard library, this lacks continuations. `boost::future.then` allows attaching continuations to futures as outlined in the proposal [here](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf). This feature will come to standard C++, but probably not before 2017 (see [C++ Standardisation Roadmap](http://isocpp.org/std/status))
> * Support for `when_all` and `when_any` as described in above proposal depends on Boost 1.56 or higher.
> * The library and example programs were tested and developed with **clang 3.4**, **libc++** and **Boost trunk/1.56** on an Ubuntu 13.10 x86-64 bit system. It also works with **gcc 4.8**, **libstdc++** and **Boost trunk/1.56**. Your mileage with other versions of the former may vary, but we accept PRs;)

---


#### System libraries and cmake

Install some libs and build tools (these are for Debian/Ubuntu):

```console
sudo apt-get install -y libbz2-dev libssl-dev
```

Install cmake

```console
cd ~
wget https://cmake.org/files/v3.11/cmake-3.11.0-Linux-x86_64.sh
chmod +x cmake-3.11.0-Linux-x86_64.sh
sudo ./cmake-3.11.0-Linux-x86_64.sh --prefix=/opt
sudo mv /opt/cmake-3.11.0-Linux-x86_64 /opt/cmake
```

---


#### Clang (alternative)

If you want to work with Clang (rather than GCC), install [clang](http://clang.llvm.org/) and [libc++](http://libcxx.llvm.org/) (these are for Ubuntu):

```console
sudo apt-get install -y clang libc++1 libc++-dev
```

Then make Clang available:

```console
$ sudo update-alternatives --config c++
There are 2 choices for the alternative c++ (providing /usr/bin/c++).

  Selection    Path                      Priority   Status
------------------------------------------------------------
* 0            /usr/bin/g++              20        auto mode
  1            /usr/bin/clang++          10        manual mode
  2            /usr/bin/clang++-libc++   5         manual mode
  3            /usr/bin/g++              20        manual mode

Press <enter> to keep the current choice[*], or type selection number: 1
update-alternatives: using /usr/bin/clang++ to provide /usr/bin/c++ (c++) in manual mode

```

---


#### Boost

Most of the time, your distro's Boost libraries will be outdated (unless you're using Arch or Homebrew). Don't waste time with those. To build the latest Boost 1.66 (current release as of 2018/4) from sources.

Get Boost:

```console
cd ~
wget https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.bz2
tar xvjf boost_1_66_0.tar.bz2
cd boost_1_66_0
```

Then, to build using the *GCC* toolchain:

```console
./bootstrap.sh --with-toolset=gcc
./b2 toolset=gcc -j4
sudo ./b2 install --prefix=/opt/boost
```

> Note: The `-j 4` option will allow use of 4 CPU cores for building.

Instead, to build using the *Clang* toolchain:

```console
./bootstrap.sh --with-toolset=clang
./b2 toolset=clang -j4 \
    cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++"
sudo ./b2 install --prefix=/opt/boost
```

Then add the following to your `~/.profile` or `~/.bashrc`:

```shell
export BOOST_ROOT=/opt/boost
export LD_LIBRARY_PATH=${BOOST_ROOT}/lib:${LD_LIBRARY_PATH}
```

---


#### MsgPack-C

Get [MsgPack-C](https://github.com/msgpack/msgpack-c) and install:

```console
cd ~
git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c
git checkout cpp-1.4.2
cmake -DMSGPACK_CXX11=ON -DCMAKE_INSTALL_PREFIX=/opt/msgpack .
make
sudo make install
```
> On FreeBSD, you need to `pkg install autotools` and invoke `gmake` instead of `make`.

Then add the following to your `~/.profile` or `~/.bashrc`:

```shell
export MSGPACK_ROOT=/opt/msgpack
export LD_LIBRARY_PATH=${MSGPACK_ROOT}/lib:${LD_LIBRARY_PATH}
```

---


#### WebSocket++

Get [WebSocket++](https://github.com/zaphoyd/websocketpp) and install:

```console
cd ~
git clone https://github.com/zaphoyd/websocketpp.git
cd websocketpp
cmake -DCMAKE_INSTALL_PREFIX=/opt/websocketpp .
sudo make install
```

Then add the following to your `~/.profile` or `~/.bashrc`:

```shell
export WEBSOCKETPP_ROOT=/opt/websocketpp
export LD_LIBRARY_PATH=${WEBSOCKETPP_ROOT}/lib:${LD_LIBRARY_PATH}
```

---


### Building the examples

Now that we have all the dependencies, to build the examples:

```console
mkdir build
cd build
/opt/cmake/bin/cmake ..
make -j4
sudo make install
```

You should see a clean build like in the following output:

```console
oberstet@thinkpad-t430s:~/scm/crossbario/autobahn-cpp/build$ /opt/cmake/bin/cmake ..
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
-- Boost version: 1.66.0
-- Found the following Boost libraries:
--   program_options
--   system
--   thread
--   random
--   chrono
--   date_time
--   atomic
-- Found OpenSSL: /usr/lib/x86_64-linux-gnu/libcrypto.so (found version "1.0.2g")
-- AUTOBAHN_BUILD_EXAMPLES:  ON
-- CMAKE_ROOT:               /opt/cmake/share/cmake-3.11
-- Boost_INCLUDE_DIRS:       /opt/boost/include
-- Boost_LIBRARIES:          /opt/boost/lib/libboost_program_options.so/opt/boost/lib/libboost_system.so/opt/boost/lib/libboost_thread.so/opt/boost/lib/libboost_random.so/opt/boost/lib/libboost_chrono.so/opt/boost/lib/libboost_date_time.so/opt/boost/lib/libboost_atomic.so
-- Msgpack_INCLUDE_DIRS:     /opt/msgpack/include
-- Msgpack_LIBRARIES:        /opt/msgpack/libs
-- Websocketpp_INCLUDE_DIRS: /opt/websocketpp/include
-- Websocketpp_LIBRARIES:    /opt/websocketpp/libs
-- Configuring done
-- Generating done
-- Build files have been written to: /home/oberstet/scm/crossbario/autobahn-cpp/build
oberstet@thinkpad-t430s:~/scm/crossbario/autobahn-cpp/build$ make -j4
Scanning dependencies of target examples_parameters
[  5%] Building CXX object examples/CMakeFiles/examples_parameters.dir/parameters.cpp.o
[ 11%] Linking CXX static library libexamples_parameters.a
[ 11%] Built target examples_parameters
Scanning dependencies of target callee
Scanning dependencies of target caller
Scanning dependencies of target provide_prefix
Scanning dependencies of target wampcra
[ 16%] Building CXX object examples/CMakeFiles/caller.dir/caller.cpp.o
[ 22%] Building CXX object examples/CMakeFiles/callee.dir/callee.cpp.o
[ 27%] Building CXX object examples/CMakeFiles/provide_prefix.dir/callee.cpp.o
[ 33%] Building CXX object examples/CMakeFiles/wampcra.dir/wampcra.cpp.o
[ 38%] Linking CXX executable wampcra
[ 38%] Built target wampcra
Scanning dependencies of target subscriber
[ 44%] Linking CXX executable caller
[ 50%] Building CXX object examples/CMakeFiles/subscriber.dir/subscriber.cpp.o
[ 50%] Built target caller
Scanning dependencies of target uds
[ 55%] Linking CXX executable callee
[ 61%] Linking CXX executable provide_prefix
[ 66%] Building CXX object examples/CMakeFiles/uds.dir/uds.cpp.o
[ 66%] Built target callee
Scanning dependencies of target publisher
[ 66%] Built target provide_prefix
Scanning dependencies of target websocket_callee
[ 72%] Building CXX object examples/CMakeFiles/publisher.dir/publisher.cpp.o
[ 77%] Building CXX object examples/CMakeFiles/websocket_callee.dir/websocket_callee.cpp.o
[ 83%] Linking CXX executable subscriber
[ 83%] Built target subscriber
[ 88%] Linking CXX executable publisher
[ 94%] Linking CXX executable uds
[ 94%] Built target publisher
[ 94%] Built target uds
[100%] Linking CXX executable websocket_callee
[100%] Built target websocket_callee
oberstet@thinkpad-t430s:~/scm/crossbario/autobahn-cpp/build$
```

And here are some details for one of the built example binaries:


```console
oberstet@thinkpad-t430s:~/scm/crossbario/autobahn-cpp/build$ file examples/websocket_callee
examples/websocket_callee: ELF 64-bit LSB executable, x86-64, version 1 (GNU/Linux), dynamically linked, interpreter /lib64/ld-linux-x86-64.so.2, for GNU/Linux 2.6.32, BuildID[sha1]=bfcab40b2350acd5869d913226723999cf0b822e, not stripped
oberstet@thinkpad-t430s:~/scm/crossbario/autobahn-cpp/build$ ldd examples/websocket_callee
    linux-vdso.so.1 =>  (0x00007fff44760000)
    libboost_program_options.so.1.66.0 => /opt/boost/lib/libboost_program_options.so.1.66.0 (0x00007f8518873000)
    libboost_system.so.1.66.0 => /opt/boost/lib/libboost_system.so.1.66.0 (0x00007f851866f000)
    libboost_thread.so.1.66.0 => /opt/boost/lib/libboost_thread.so.1.66.0 (0x00007f8518446000)
    libssl.so.1.0.0 => /lib/x86_64-linux-gnu/libssl.so.1.0.0 (0x00007f85181dd000)
    libcrypto.so.1.0.0 => /lib/x86_64-linux-gnu/libcrypto.so.1.0.0 (0x00007f8517d99000)
    libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f8517b7c000)
    libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f85177fa000)
    libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f85175e4000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f851721a000)
    librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007f8517012000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f8516e0e000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f8518af3000)
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f8516b05000)
oberstet@thinkpad-t430s:~/scm/crossbario/autobahn-cpp/build$
```

---


## Resources

* [ASIO C++11 Examples](http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html)
* [Using Asio with C++11](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3388.pdf)
* [C++17: I See a Monad in Your Future! ](http://bartoszmilewski.com/2014/02/26/c17-i-see-a-monad-in-your-future/)
* [Boost Thread](http://www.boost.org/doc/libs/1_55_0/doc/html/thread.html)
* [Boost Issue: when_all](https://svn.boost.org/trac/boost/ticket/7447)
* [Boost Issue. when_any](https://svn.boost.org/trac/boost/ticket/7446)
* [Boost Issue: future fires twice](https://svn.boost.org/trac/boost/ticket/9711)
* [Boost C++ 1y](http://www.boost.org/doc/libs/1_55_0/doc/html/thread/compliance.html#thread.compliance.cxx1y.async)
* [Asynchronous API in C++ and the Continuation Monad](https://www.fpcomplete.com/blog/2012/06/asynchronous-api-in-c-and-the-continuation-monad)

---


## Closures Cheatsheet

* `[]` Capture nothing (or, a scorched earth strategy?)
* `[&]` Capture any referenced variable by reference
* `[=]` Capture any referenced variable by making a copy
* `[=, &foo]` Capture any referenced variable by making a copy, but capture variable `foo` by reference
* `[bar]` Capture `bar` by making a copy; don't copy anything else
* `[this]` Capture the this pointer of the enclosing class

---


## Release process

To push a new release of the toolchain Docker image:

```console
git tag -a v20.8.1 -m "tagged release"
source docker/versions.sh
make build_gcc
make publish_gcc
```

> Note: clang is currently broken.
