AutobahnCpp
===========

WAMP for C++


sudo apt-get install ruby libtool autoconf scons
./boostrap
./configure --prefix=$HOME/msgpack



Add the following to `$HOME/.profile`

```shell
export LD_LIBRARY_PATH=${HOME}/msgpack/lib:${LD_LIBRARY_PATH}
```

and

```shell
source $HOME/.profile
```
