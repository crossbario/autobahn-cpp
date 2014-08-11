Examples
========

The Autobahn|Cpp repository contains a number of `examples <https://github.com/tavendo/AutobahnCpp/tree/master/examples>`_ that demonstrate all 4 basic patterns of using WAMP:

* `Call 2 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/call2.cpp>`_
* `Call 1 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/call1.cpp>`_
* `Call 3 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/call3.cpp>`_
* `Register 1 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/register1.cpp>`_
* `Register 2 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/register2.cpp>`_
* `Publish 1 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/publish1.cpp>`_
* `Publish 2 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/publish2.cpp>`_
* `Publish 3 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/publish3.cpp>`_
* `Subscribe 1 <https://github.com/tavendo/AutobahnCpp/blob/master/examples/subscribe1.cpp>`_


Building the Examples
---------------------

For building the examples, add the following to your ``~/.profile``:


.. code-block:: console

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


For building with GCC, use the following

.. code-block:: console

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


Now build all examples:

.. code-block:: console

   cd autobahn/examples
   scons -j 4


The examples will get built in ``autobahn/build/examples``.


Running the Examples
--------------------

The examples include a `Autobahn|Python <http://autobahn.ws/python>`_ based WAMP router and example backend.

To run this, you need `Python <http://python.org>`_ and `pip <http://www.pip-installer.org/en/latest/installing.html>`_ already installed.

Then, to install **Autobahn|Python**

.. code-block:: console

   pip install autobahn[twisted]


Start the example router in a first terminal

.. code-block:: console

   cd autobahn/examples
   python server.py


Then start one of the built C++ examples in a second terminal

.. code-block:: console

   cd autobahn
   ./build/examples/call1

