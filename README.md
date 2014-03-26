# **Autobahn**|Cpp

**Autobahn**|Cpp is a subproject of [Autobahn](http://autobahn.ws/) which provides a C++ [WAMP](http://wamp.ws/) implementation that is able to talk WAMP over `stdio` pipes.

It implements version 2 of WAMP with the following roles:

 * Caller
 * Callee
 * Publisher
 * Subscriber

and depends on

 * C++ 11 compiler
 * `boost::any`
 * `boost::future`


> Note: While C++ 11 provides a `std::future`, that lacks continuations. **Autobahn**|Cpp makes use of `boost::future.then` for attaching continuations to futures as outlined in the proposal [here](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf). This feature will come to standard C++, but probably not before 2015:
* [C++ Standardisation Roadmap](http://isocpp.org/std/status)
* [Boost C++ 1y](http://www.boost.org/doc/libs/1_55_0/doc/html/thread/compliance.html#thread.compliance.cxx1y.async)


## Building

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

Get the latest Boost from [here](http://www.boost.org/). Then

```shell
cd $HOME
tar xvjf Downloads/boost_1_55_0.tar.bz2
cd boost_1_55_0/
./bootstrap.sh
./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++"
```

Get Boost trunk by doing:

```shell
git clone --recursive git@github.com:boostorg/boost.git


svn co  http://svn.boost.org/svn/boost/trunk boost-trunk
```


Add the following to `$HOME/.profile`

```shell
export LD_LIBRARY_PATH=${HOME}/boost_1_55_0/stage/lib:${LD_LIBRARY_PATH}
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

Add the following to `$HOME/.profile`

```shell
export LD_LIBRARY_PATH=${HOME}/msgpack_clang/lib:${LD_LIBRARY_PATH}
```

### **Autobahn**|Cpp

Finally, to build **Autobahn**|Cpp

```shell
source $HOME/.profile
cd $HOME
git clone git@github.com:tavendo/AutobahnCpp.git
cd AutobahnCpp
scons
```

## Futures

* [C++17: I See a Monad in Your Future! ](http://bartoszmilewski.com/2014/02/26/c17-i-see-a-monad-in-your-future/)
* [Boost Thread](http://www.boost.org/doc/libs/1_55_0/doc/html/thread.html)
* [Boost Issue: when_all](https://svn.boost.org/trac/boost/ticket/7447)
* [Boost Issue. when_any](https://svn.boost.org/trac/boost/ticket/7446)
* [Boost Issue: future fires twice](https://svn.boost.org/trac/boost/ticket/9711)

## Closures Cheetsheet

* `[]` Capture nothing (or, a scorched earth strategy?)
* `[&]` Capture any referenced variable by reference
* `[=]` Capture any referenced variable by making a copy
* `[=, &foo]` Capture any referenced variable by making a copy, but capture variable `foo` by reference
* `[bar]` Capture `bar` by making a copy; don't copy anything else
* `[this]` Capture the this pointer of the enclosing class
