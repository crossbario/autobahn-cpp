AutobahnCpp
===========

WAMP for C++


sudo apt-get install ruby libtool autoconf scons
./boostrap
./configure --prefix=$HOME/msgpack
make
make install



Building with clang:

sudo apt-get install clang-3.4 libc++1 libc++-dev

CXX=`which clang++` CC=`which clang` CXXFLAGS="-std=c++11 -stdlib=libc++" LDFLAGS="-stdlib=libc++" ./configure --prefix=$HOME/msgpack_clang



Add the following to `$HOME/.profile`

```shell
export LD_LIBRARY_PATH=${HOME}/msgpack/lib:${LD_LIBRARY_PATH}
```

and

```shell
source $HOME/.profile
```
