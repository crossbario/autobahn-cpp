# **Autobahn**|Cpp

**Autobahn**|Cpp is a subproject of [Autobahn](http://autobahn.ws/) which implements the [Web Application Messaging Protocol (WAMP)](http://wamp.ws/) in C++ supporting the following application roles

 * **Caller**
 * **Callee**
 * **Publisher**
 * **Subscriber**

The API and implementation make use of modern C++ 11 and new asynchronous idioms using (upcoming) features of the standard C++ library, in particular **Futures**, [**Continuations**](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf) and **Lambdas**.

> [Continuations](http://en.wikipedia.org/wiki/Continuation) are *one* way of managing control flow in an asynchronous program. Other styles include: asynchronous [Callbacks](http://en.wikipedia.org/wiki/Callback_%28computer_programming%29), [Coroutines](http://en.wikipedia.org/wiki/Coroutine) (`yield` or `await`), Actors ([Erlang/OTP](http://www.erlang.org/), [Scala](http://www.scala-lang.org/)/[Akka](http://akka.io/) or [Rust](http://www.scala-lang.org/)) and [Transactional memory](http://en.wikipedia.org/wiki/Transactional_Synchronization_Extensions).
>

**Autobahn**|Cpp supports running WAMP (`rawsocket-msgpack`) over **TCP(-TLS)**, **Unix domain sockets** or **pipes** (`stdio`). The library is "header-only", light-weight (< 2k code lines) and **depends on** the following:

 * C++ 11 compiler
 * [`boost::future`](http://www.boost.org/doc/libs/1_55_0/doc/html/thread/synchronization.html#thread.synchronization.futures)
 * [`boost::any`](http://www.boost.org/doc/libs/1_55_0/doc/html/any.html)
 * [`boost::asio`](http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio.html)

For getting help, questions or feedback, get in touch on the **[mailing list](https://groups.google.com/forum/#!forum/autobahnws)**, **[Twitter](https://twitter.com/autobahnws)** or **IRC `#autobahn`** (Freenode).


## Show me some code!

Here is how programming with C++ and **Autobahn**|Cpp looks like.

**Calling a remote Procedure**

```c++
auto c1 = session.call("com.mathservice.add2", {23, 777})
.then(
   [&](future<any> f) {
		cout << "Got call result " << any_cast<uint64_t> (f.get()) << endl;
	});
```

**Registering a remoted Procedure**
```c++
auto r1 = session.provide("com.myapp.cpp.square",
   [](const anyvec& args, const anymap& kwargs) {
      cout << "Procedure is invoked .." << endl;
      uint64_t x = any_cast<uint64_t> (args[0]);
      return x * x;
   })
.then(
   [](future<registration> reg) {
      cout << "Registered with ID " << reg.get().id << endl;
   });
```

**Publishing an Event**

```c++
session.publish("com.myapp.topic2", {23, true, string("hello")});
```

**Publishing an Event (acknowledged)**

```c++
auto opts = PublishOptions();
opts.acknowledge = True;

auto p1 = session.publish("com.myapp.topic2", {23, true, string("hello")}, opts)
.then(
   [](future<publication> pub) {
      cout << "Published with ID " << pub.get().id << endl;
   });
```

**Subscribing to a Topic**

```c++
auto s1 = session.subscribe("com.myapp.topic1",
   [](const anyvec& args, const anymap& kwargs) {
      cout << "Got event: " << any_cast<uint64_t>(args[0]) << endl;
   })
.then(
   [](future<subscription> sub) {
      cout << "Subscribed with ID " << sub.get().id << endl;
   });
```


Here is JavaScript running in Chrome call into C++ running on command line. Both are connected via a WAMP router, in this case [**Autobahn**|Python](http://autobahn.ws/python) based.

![](doc/_static/cpp_from_js.png)

* [Example C++ code](https://github.com/tavendo/AutobahnCpp/blob/master/examples/register2.cpp)
* [Example JavaScript code](https://github.com/tavendo/AutobahnCpp/blob/master/examples/index.html)

## Building

> *Notes*
>
> * The library code is written in standard C++ 11. Target toolchains currently include **clang** and **gcc**. Support for MSVC is tracked on this [issue](https://github.com/tavendo/AutobahnCpp/issues/2).
> * While C++ 11 includes `std::future` in the standard library, this lacks continuations. `boost::future.then` allows attaching continuations to futures as outlined in the proposal [here](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf). This feature will come to standard C++, but probably not before 2015 (see [C++ Standardisation Roadmap](http://isocpp.org/std/status))
> * Support for `when_all` and `when_any` as described in above proposal depends on Boost 1.56 (upcoming release as of 31/03/2014) or higher.
> * The library and example programs were tested and developed with **clang 3.4**, **libc++** and **Boost trunk/1.56** on an Ubuntu 13.10 x86-64 bit system. It also works with **gcc 4.8**, **libstdc++** and **Boost trunk/1.56**. Your mileage with other versions of the former may vary, but we accept PRs;)


### Build tools

Install some libs and build tools:

```shell
sudo apt-get install libbz2-dev libssl-dev ruby libtool autoconf scons
```

### clang

Install [clang](http://clang.llvm.org/) and [libc++](http://libcxx.llvm.org/):

```shell
sudo apt-get install clang-3.4 libc++1 libc++-dev
```

### Boost

To build Boost 1.55 (current release) from sources, get source code package for the latest Boost release from [here](http://www.boost.org/) and

```shell
cd $HOME
tar xvjf Downloads/boost_1_55_0.tar.bz2
cd boost_1_55_0/
./bootstrap.sh
./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" -j 4
```

> Note: The `-j 4` option will allow use of 4 cores for building.
>

To build with GCC instead of clang:

```shell
./b2 toolset=gcc -j 4
```

To build Boost trunk (needed for `when_all`, `when_any`)

```shell
cd $HOME
git clone --recursive git@github.com:boostorg/boost.git
cd boost/
./bootstrap.sh
./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" -j 4
```

### MsgPack-C

Get [MsgPack-C](https://github.com/msgpack/msgpack-c) and build with clang:

```shell
cd $HOME
git clone https://github.com/msgpack/msgpack-c.git
cd msgpack-c
./bootstrap
CXX=`which clang++` CC=`which clang` CXXFLAGS="-std=c++11 -stdlib=libc++" \
   LDFLAGS="-stdlib=libc++" ./configure --prefix=$HOME/msgpack_clang
make
make install
```

To build with GCC instead of clang:


```shell
./configure --prefix=$HOME/msgpack_gcc
```

### **Autobahn**|Cpp

To get **Autobahn**|Cpp library and examples, clone the repo

```shell
cd $HOME
git clone git@github.com:tavendo/AutobahnCpp.git
cd AutobahnCpp
```

The library is "header-only", means there isn't anything to compile or build. Just include the relevant headers.


## Examples

The Autobahn|Cpp repository contains a number of [examples](https://github.com/tavendo/AutobahnCpp/tree/master/examples) that demonstrate all 4 basic patterns of using WAMP:

* [Call 1](https://github.com/tavendo/AutobahnCpp/blob/master/examples/call1.cpp)
* [Call 2](https://github.com/tavendo/AutobahnCpp/blob/master/examples/call2.cpp)
* [Call 3](https://github.com/tavendo/AutobahnCpp/blob/master/examples/call3.cpp)
* [Register 1](https://github.com/tavendo/AutobahnCpp/blob/master/examples/register1.cpp)
* [Register 2](https://github.com/tavendo/AutobahnCpp/blob/master/examples/register2.cpp)
* [Publish 1](https://github.com/tavendo/AutobahnCpp/blob/master/examples/publish1.cpp)
* [Publish 2](https://github.com/tavendo/AutobahnCpp/blob/master/examples/publish2.cpp)
* [Publish 3](https://github.com/tavendo/AutobahnCpp/blob/master/examples/publish3.cpp)
* [Subscribe 1](https://github.com/tavendo/AutobahnCpp/blob/master/examples/subscribe1.cpp)


### Building the Examples

For building the examples, add the following to your `~/.profile`:


```shell
## Use clang
##
export CC='clang'
export CXX='clang++'

## Libaries (clang based)
##
export BOOST_ROOT=${HOME}/boost_trunk_clang
export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib:${LD_LIBRARY_PATH}

export MSGPACK_ROOT=${HOME}/msgpack_clang
export LD_LIBRARY_PATH=${MSGPACK_ROOT}/lib:${LD_LIBRARY_PATH}
```

For building with GCC, use the following

```shell
## Use GNU
##
export CC='gcc'
export CXX='g++'

## Libraries (GCC based)
##
export BOOST_ROOT=${HOME}/boost_trunk_gcc
export LD_LIBRARY_PATH=${BOOST_ROOT}/stage/lib:${LD_LIBRARY_PATH}

export MSGPACK_ROOT=${HOME}/msgpack_gcc
export LD_LIBRARY_PATH=${MSGPACK_ROOT}/lib:${LD_LIBRARY_PATH}
```

Now build all examples:

```shell
cd autobahn/examples
scons -j 4
```

The examples will get built in `autobahn/build/examples`.


### Running the Examples

The examples include a [Autobahn|Python](http://autobahn.ws/python) based WAMP router and example backend.

To run this, you need [Python](http://python.org) and [pip](http://www.pip-installer.org/en/latest/installing.html) already installed.

Then, to install **Autobahn|Python**

```shell
pip install autobahn[twisted]
```

Start the example router in a first terminal

```shell
cd autobahn/examples
python server.py
```

Then start one of the built C++ examples in a second terminal

```shell
cd autobahn
./build/examples/call1
```


## Documentation

[Click here](http://autobahn.ws/cpp/reference/) for the Autobahn|Cpp reference documentation.

## Get in touch

Get in touch on IRC `#autobahn` on `chat.freenode.net`, follow us on [Twitter](https://twitter.com/autobahnws) or join the [mailing list](http://groups.google.com/group/autobahnws).


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


## Closures Cheatsheet

* `[]` Capture nothing (or, a scorched earth strategy?)
* `[&]` Capture any referenced variable by reference
* `[=]` Capture any referenced variable by making a copy
* `[=, &foo]` Capture any referenced variable by making a copy, but capture variable `foo` by reference
* `[bar]` Capture `bar` by making a copy; don't copy anything else
* `[this]` Capture the this pointer of the enclosing class
