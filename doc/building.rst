Building |ab|
=============

Notes
-----
* The library code is written in standard C++ 11. Target toolchains currently include **clang** and **gcc**. Support for MSVC is tracked on this `issue <https://github.com/tavendo/AutobahnCpp/issues/2>`_.
* While C++ 11 includes `std::future` in the standard library, this lacks continuations. ``boost::future.then`` allows attaching continuations to futures as outlined in the proposal `here <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf>`_. This feature will come to standard C++, but probably not before 2015 (see `C++ Standardisation Roadmap <http://isocpp.org/std/status>`_)
* Support for ``when_all`` and ``when_any`` as described in above proposal depends on Boost 1.56 (upcoming release as of 31/03/2014) or higher.
* The library and example programs were tested and developed with **clang 3.4**, **libc++** and **Boost trunk/1.56** on an Ubuntu 13.10 x86-64 bit system. It also works with **gcc 4.8**, **libstdc++** and **Boost trunk/1.56**. Your mileage with other versions of the former may vary, but we accept PRs ;)


Build tools
-----------

Install some libs and build tools:

.. code-block:: console

   sudo apt-get install libbz2-dev libssl-dev ruby libtool autoconf scons


clang
-----

Install `clang <http://clang.llvm.org/>`_ and  `libc++ <http://libcxx.llvm.org/>`_:

.. code-block:: console

   sudo apt-get install clang-3.4 libc++1 libc++-dev


Boost
-----

To build Boost 1.55 (current release) from sources, get source code package for the latest Boost release from `here <http://www.boost.org/>`_ and

.. code-block:: console

   cd $HOME
   tar xvjf Downloads/boost_1_55_0.tar.bz2
   cd boost_1_55_0/
   ./bootstrap.sh
   ./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" -j 4


..note:: Note: The ``-j 4`` option will allow use of 4 cores for building.

To build with GCC instead of clang:

.. code-block:: console

   ./b2 toolset=gcc -j 4


To build Boost trunk (needed for ``when_all``, ``when_any``)

.. code-block:: console

   cd $HOME
   git clone --recursive git@github.com:boostorg/boost.git
   cd boost/
   ./bootstrap.sh
   ./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++" -j 4


MsgPack-C
---------

Get `MsgPack-C <https://github.com/msgpack/msgpack-c>`_ and build with clang:

.. code-block:: console

   cd $HOME
   git clone https://github.com/msgpack/msgpack-c.git
   cd msgpack-c
   ./bootstrap
   CXX=`which clang++` CC=`which clang` CXXFLAGS="-std=c++11 -stdlib=libc++" \
      LDFLAGS="-stdlib=libc++" ./configure --prefix=$HOME/msgpack_clang
   make
   make install


To build with GCC instead of clang:


.. code-block:: console

   ./configure --prefix=$HOME/msgpack_gcc

