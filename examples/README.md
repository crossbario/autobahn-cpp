# Examples

## Build

To build the examples, go to the root folder and type `scons`:

```console
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ scons
scons: Reading SConscript files ...
Using clang toolchain
scons: done reading SConscript files.
scons: Building targets ...
clang++ -o build/examples/callee.o -c -std=c++11 -stdlib=libc++ -O2 -Wall -pedantic -Wno-unused-value -Wno-deprecated -pthread -I/home/oberstet/boost_1_60_0 -I/home/oberstet/msgpack_clang/include -I. examples/callee.cpp
clang++ -o build/examples/parameters.o -c -std=c++11 -stdlib=libc++ -O2 -Wall -pedantic -Wno-unused-value -Wno-deprecated -pthread -I/home/oberstet/boost_1_60_0 -I/home/oberstet/msgpack_clang/include -I. examples/parameters.cpp
ar rc build/examples/libparameters.a build/examples/parameters.o
ranlib build/examples/libparameters.a
clang++ -o build/examples/callee -stdlib=libc++ -pthread build/examples/callee.o -L/home/oberstet/boost_1_60_0/stage/lib -L/home/oberstet/msgpack_clang/lib -lboost_thread -lboost_system -lboost_program_options -lmsgpackc build/examples/libparameters.a
clang++ -o build/examples/caller.o -c -std=c++11 -stdlib=libc++ -O2 -Wall -pedantic -Wno-unused-value -Wno-deprecated -pthread -I/home/oberstet/boost_1_60_0 -I/home/oberstet/msgpack_clang/include -I. examples/caller.cpp
clang++ -o build/examples/caller -stdlib=libc++ -pthread build/examples/caller.o -L/home/oberstet/boost_1_60_0/stage/lib -L/home/oberstet/msgpack_clang/lib -lboost_thread -lboost_system -lboost_program_options -lmsgpackc build/examples/libparameters.a
clang++ -o build/examples/publisher.o -c -std=c++11 -stdlib=libc++ -O2 -Wall -pedantic -Wno-unused-value -Wno-deprecated -pthread -I/home/oberstet/boost_1_60_0 -I/home/oberstet/msgpack_clang/include -I. examples/publisher.cpp
clang++ -o build/examples/publisher -stdlib=libc++ -pthread build/examples/publisher.o -L/home/oberstet/boost_1_60_0/stage/lib -L/home/oberstet/msgpack_clang/lib -lboost_thread -lboost_system -lboost_program_options -lmsgpackc build/examples/libparameters.a
clang++ -o build/examples/subscriber.o -c -std=c++11 -stdlib=libc++ -O2 -Wall -pedantic -Wno-unused-value -Wno-deprecated -pthread -I/home/oberstet/boost_1_60_0 -I/home/oberstet/msgpack_clang/include -I. examples/subscriber.cpp
clang++ -o build/examples/subscriber -stdlib=libc++ -pthread build/examples/subscriber.o -L/home/oberstet/boost_1_60_0/stage/lib -L/home/oberstet/msgpack_clang/lib -lboost_thread -lboost_system -lboost_program_options -lmsgpackc build/examples/libparameters.a
scons: done building targets.
```

This will produce a couple of executables:

```console
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ ll ./build/examples/
insgesamt 10328
drwxrwxr-x 2 oberstet oberstet    4096 Jan 28 18:48 ./
drwxrwxr-x 3 oberstet oberstet    4096 Jan 28 17:09 ../
-rwxrwxr-x 1 oberstet oberstet  837508 Jan 28 18:48 callee*
-rw-rw-r-- 1 oberstet oberstet 1541680 Jan 28 18:48 callee.o
-rwxrwxr-x 1 oberstet oberstet  946842 Jan 28 18:48 caller*
-rw-rw-r-- 1 oberstet oberstet 1682264 Jan 28 18:48 caller.o
-rw-rw-r-- 1 oberstet oberstet  345618 Jan 28 18:48 libparameters.a
-rw-rw-r-- 1 oberstet oberstet  311112 Jan 28 18:48 parameters.o
-rwxrwxr-x 1 oberstet oberstet  897523 Jan 28 18:48 publisher*
-rw-rw-r-- 1 oberstet oberstet 1608264 Jan 28 18:48 publisher.o
-rwxrwxr-x 1 oberstet oberstet  836239 Jan 28 18:48 subscriber*
-rw-rw-r-- 1 oberstet oberstet 1537536 Jan 28 18:48 subscriber.o
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ ldd ./build/examples/callee
    linux-vdso.so.1 =>  (0x00007ffc889d9000)
    libboost_thread.so.1.60.0 => /home/oberstet/boost_1_60_0/stage/lib/libboost_thread.so.1.60.0 (0x00007ff8db0e3000)
    libboost_system.so.1.60.0 => /home/oberstet/boost_1_60_0/stage/lib/libboost_system.so.1.60.0 (0x00007ff8daedf000)
    libboost_program_options.so.1.60.0 => /home/oberstet/boost_1_60_0/stage/lib/libboost_program_options.so.1.60.0 (0x00007ff8dac69000)
    libmsgpackc.so.2 => /home/oberstet/msgpack_clang/lib/libmsgpackc.so.2 (0x00007ff8daa63000)
    libc++.so.1 => /usr/lib/x86_64-linux-gnu/libc++.so.1 (0x00007ff8da771000)
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007ff8da46b000)
    libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007ff8da255000)
    libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007ff8da037000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007ff8d9c72000)
    librt.so.1 => /lib/x86_64-linux-gnu/librt.so.1 (0x00007ff8d9a6a000)
    /lib64/ld-linux-x86-64.so.2 (0x00007ff8db30c000)
```

## Run

The following describes how to run the examples together with [Crossbar.io](http://crossbar.io). It should also work with other [WAMP compliant routers](http://wamp-proto.org/implementations/#routers), such as [Thruway](https://github.com/voryx/Thruway) or [Bonefish](https://github.com/tplgy/bonefish).

### Install Crossbar.io

> For full installation docs, please refer to the [Crossbar.io documentation](http://crossbar.io/docs/).

You will need Python 2.7+ or Python 3.3+. Then do:

```console
pip install crossbar[all]
```

This should give you

```console
(cpy351_2) oberstet@corei7ub1310:~$ which crossbar
/home/oberstet/cpy351_2/bin/crossbar
(cpy351_2) oberstet@corei7ub1310:~$ crossbar version
Automatically choosing optimal Twisted reactor
Running on Linux and optimal reactor (epoll) was installed.
     __  __  __  __  __  __      __     __
    /  `|__)/  \/__`/__`|__) /\ |__)  |/  \
    \__,|  \\__/.__/.__/|__)/~~\|  \. |\__/
                                        
 Crossbar.io        : 0.12.1
   Autobahn         : 0.12.1 (with JSON, MessagePack, CBOR)
   Twisted          : 15.5.0-EPollReactor
   LMDB             : 0.89/lmdb-0.9.18
   Python           : 3.5.1/CPython
 OS                 : Linux-3.13.0-77-generic-x86_64-with-debian-jessie-sid
 Machine            : x86_64

(cpy351_2) oberstet@corei7ub1310:~$ 
```

### Start Crossbar.io

In a terminal, got to the examples forder and start [Crossbar.io](http://crossbar.io/):

```console
(cpy351_1) oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp/examples$ crossbar start
2016-01-28T18:50:24+0100 [Controller  17146] Automatically choosing optimal Twisted reactor
2016-01-28T18:50:24+0100 [Controller  17146] Running on Linux and optimal reactor (epoll) was installed.
2016-01-28T18:50:24+0100 [Controller  17146]      __  __  __  __  __  __      __     __
2016-01-28T18:50:24+0100 [Controller  17146]     /  `|__)/  \/__`/__`|__) /\ |__)  |/  \
2016-01-28T18:50:24+0100 [Controller  17146]     \__,|  \\__/.__/.__/|__)/~~\|  \. |\__/
2016-01-28T18:50:24+0100 [Controller  17146]                                         
2016-01-28T18:50:24+0100 [Controller  17146]     Crossbar.io Version: 0.12.0
2016-01-28T18:50:24+0100 [Controller  17146]     Node Public Key: f39c1f92e4e6e5032002b3e9c54c36d00e961fa973166803997c204856dc088b
2016-01-28T18:50:24+0100 [Controller  17146] 
2016-01-28T18:50:24+0100 [Controller  17146] Running from node directory '/home/oberstet/scm/crossbario/autobahn-cpp/examples/.crossbar'
2016-01-28T18:50:24+0100 [Controller  17146] Controller process starting (CPython-EPollReactor) ..
2016-01-28T18:50:24+0100 [Controller  17146] Node configuration loaded from 'config.json'
2016-01-28T18:50:24+0100 [Controller  17146] Node ID 'corei7ub1310' set from hostname
2016-01-28T18:50:24+0100 [Controller  17146] Using default node shutdown triggers ['shutdown_on_worker_exit']
2016-01-28T18:50:24+0100 [Controller  17146] Joined realm 'crossbar' on node management router
2016-01-28T18:50:24+0100 [Controller  17146] Starting Router with ID 'worker1'...
2016-01-28T18:50:25+0100 [Router      17150] Worker process starting (CPython-EPollReactor) ..
2016-01-28T18:50:25+0100 [Controller  17146] Router with ID 'worker1' and PID 17150 started
2016-01-28T18:50:25+0100 [Router      17150] Realm 'realm1' started
2016-01-28T18:50:25+0100 [Controller  17146] Router 'worker1': realm 'realm1' (named 'realm1') started
2016-01-28T18:50:25+0100 [Controller  17146] Router 'worker1': role 'role1' (named 'anonymous') started on realm 'realm1'
2016-01-28T18:50:25+0100 [Router      17150] Site starting on 8080
2016-01-28T18:50:25+0100 [Controller  17146] Router 'worker1': transport 'transport1' started
2016-01-28T18:50:25+0100 [Router      17150] WampRawSocketServerFactory starting on 8000
2016-01-28T18:50:25+0100 [Controller  17146] Router 'worker1': transport 'transport2' started
...
```

### Remote Procedure Calls

In another terminal, start the example `callee`:

```console
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ ./build/examples/callee
Boost: 106000
Connecting to realm: realm1
starting io service
connected to server
session started
joined realm: 415466130717734
```

In another third terminal, run the `caller` example:

```console
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ ./build/examples/caller
Boost: 106000
starting io service
connected to server
joined realm: 5194933483267387
call result: 800
left session (wamp.close.normal)
stopped io service
stopped session
```

Point a Web browser to [http://localhost:8080], watching the JavaScript console output. You should see the callee (the C++ component) being invoked from the browser.


### Publish & Subscribe

In another terminal, start the example `subscriber`:

```console
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ ./build/examples/subscriber 
Boost: 106000
Connecting to realm: realm1
starting io service
connected to server
session started
joined realm: 1634243825069581
```

In another terminal, run the example `publisher`:

```console
oberstet@corei7ub1310:~/scm/crossbario/autobahn-cpp$ ./build/examples/publisher 
Boost: 106000
starting io service
connected to server
session started
joined realm: 7116274268112534
event published
left session (wamp.close.normal)
stopped io service
stopped session
```

You should see the subscriber receiving the event, printing to console.

Point a Web browser to [http://localhost:8080], watching the JavaScript console output. You should see the subscriber (JS component) receiving the event published by the C++ publisher. Same as for the C++ subscriber.


## Debugging

To turn on wire-level WAMP logging in Crossbar.io:

```console
crossbar start --loglevel=trace
```

To turn on wire-level WAMP logging in AutobahnCpp, start the examples like this:

```console
```
