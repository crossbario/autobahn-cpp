AutobahnCpp
===========

WAMP for C++


sudo apt-get install ruby libtool autoconf scons
./boostrap
./configure --prefix=$HOME/msgpack
make
make install


./b2 toolset=clang cxxflags="-stdlib=libc++" linkflags="-stdlib=libc++"


Building with clang:

sudo apt-get install clang-3.4 libc++1 libc++-dev

CXX=`which clang++` CC=`which clang` CXXFLAGS="-std=c++11 -stdlib=libc++" LDFLAGS="-stdlib=libc++" ./configure --prefix=$HOME/msgpack_clang



sudo apt-get install libboost1.54-all-dev



Add the following to `$HOME/.profile`

```shell
export LD_LIBRARY_PATH=${HOME}/msgpack/lib:${LD_LIBRARY_PATH}
```

and

```shell
source $HOME/.profile
```
