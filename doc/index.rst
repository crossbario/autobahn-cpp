|ab|
====

|ab| is an open-source networking library for C++ created by the `Autobahn project <http://autobahn.ws/>`_ that implements the `Web Application Messaging Protocol (WAMP) <http://wamp.ws/>`_ for creating C++ components communicating with a WAMP router via Unix pipes.

Web Application Messaging Protocol (WAMP)
-----------------------------------------

WAMP is ideal for distributed, multi-client and server applications, such as multi-user database-drive business applications, sensor networks (IoT), instant messaging or MMOGs (massively multi-player online games) .

WAMP enables application architectures with application code distributed freely across processes and devices according to functional aspects. Since WAMP implementations exist for multiple languages, WAMP applications can be polyglot. Application components can be implemented in a language and run on a device which best fit the particular use case.

To enable this, WAMP implements both the Remote Procedure Call (RPC) and  and the Publish and Subscribe (PubSub) messaging pattern. WAMP is open source, as is |ab| (Apache 2.0 license).

|ab| supports the following application roles:

* **Caller**
* **Callee**
* **Publisher**
* **Subscriber**


Show me some code!
------------------

Here is what programming with C++ and |ab| looks like.

**Calling a remote Procedure**

.. code-block:: cpp

   auto c1 = session.call("com.mathservice.add2", {23, 777})
   .then(
      [&](future<any> f) {
         cout << "Got call result " << any_cast<uint64_t> (f.get()) << endl;
      });



**Registering a remoted Procedure**

.. code-block:: cpp

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


**Publishing an Event**

.. code-block:: cpp

   session.publish("com.myapp.topic2", {23, true, string("hello")});


**Publishing an Event (acknowledged)**

.. code-block:: cpp

   auto opts = PublishOptions();
   opts.acknowledge = True;

   auto p1 = session.publish("com.myapp.topic2", {23, true, string("hello")}, opts)
   .then(
      [](future<publication> pub) {
         cout << "Published with ID " << pub.get().id << endl;
      });


**Subscribing to a Topic**

.. code-block:: cpp

   auto s1 = session.subscribe("com.myapp.topic1",
      [](const anyvec& args, const anymap& kwargs) {
         cout << "Got event: " << any_cast<uint64_t>(args[0]) << endl;
      })
   .then(
      [](future<subscription> sub) {
         cout << "Subscribed with ID " << sub.get().id << endl;
      });



Here is JavaScript running in Chrome call into C++ running on command line. Both are connected via a WAMP router, in this case `Autobahn|Python <http://autobahn.ws/python>`_ based.

.. image:: /_static/img/cpp_from_js.png

* `Example C++ code <https://github.com/tavendo/AutobahnCpp/blob/master/examples/register2.cpp>`_
* `Example JavaScript code <https://github.com/tavendo/AutobahnCpp/blob/master/examples/index.html>`_


Download
--------

To get |ab| library and examples, clone the repo

.. code-block:: console

   shell
   cd $HOME
   git clone git@github.com:tavendo/AutobahnCpp.git
   cd AutobahnCpp


The library is "header-only", means there isn't anything to compile or build. Just include the relevant headers.


Technology
----------

The API and implementation make use of modern C++ 11 and new asynchronous idioms using (upcoming) features of the standard C++ library, in particular **Futures**, `Continuations <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2013/n3634.pdf>`_ and **Lambdas**.

.. note:: `Continuations <http://en.wikipedia.org/wiki/Continuation>`_ are *one* way of managing control flow in an asynchronous program. Other styles include: asynchronous `Callbacks <http://en.wikipedia.org/wiki/Callback_%28computer_programming%29>`_, `Coroutines <http://en.wikipedia.org/wiki/Coroutine>`_ (`yield` or `await`), Actors (`Erlang/OTP <http://www.erlang.org/>`_, `Scala <http://www.scala-lang.org/>`_/`Akka <http://akka.io/>`_ or `Rust <http://www.scala-lang.org/>`_) and `Transactional memory <http://en.wikipedia.org/wiki/Transactional_Synchronization_Extensions>`_.

|ab| supports running WAMP (``rawsocket-msgpack``) over **TCP(-TLS)**, **Unix domain sockets** or **pipes** (``stdio``). The library is "header-only", light-weight (< 2k code lines) and **depends on** the following:

* C++ 11 compiler
* `boost::future <http://www.boost.org/doc/libs/1_55_0/doc/html/thread/synchronization.html#thread.synchronization.futures>`_
* `boost::any <http://www.boost.org/doc/libs/1_55_0/doc/html/any.html>`_
* `boost::asio <http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio.html>`_



Get in touch
------------

Get in touch on IRC `#autobahn` on `chat.freenode.net`, follow us on `Twitter <https://twitter.com/autobahnws>`_ or join the `mailing list <http://groups.google.com/group/autobahnws>`_.


Resources
---------

* `ASIO C++11 Examples <http://www.boost.org/doc/libs/1_55_0/doc/html/boost_asio/examples/cpp11_examples.html>`_
* `Using Asio with C++11 <http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2012/n3388.pdf>`_
* `C++17: I See a Monad in Your Future! <http://bartoszmilewski.com/2014/02/26/c17-i-see-a-monad-in-your-future/>`_
* `Boost Thread <http://www.boost.org/doc/libs/1_55_0/doc/html/thread.html>`_
* `Boost Issue: when_all <https://svn.boost.org/trac/boost/ticket/7447>`_
* `Boost Issue. when_any <https://svn.boost.org/trac/boost/ticket/7446>`_
* `Boost Issue: future fires twice <https://svn.boost.org/trac/boost/ticket/9711>`_
* `Boost C++ 1y <http://www.boost.org/doc/libs/1_55_0/doc/html/thread/compliance.html#thread.compliance.cxx1y.async>`_
* `Asynchronous API in C++ and the Continuation Monad <https://www.fpcomplete.com/blog/2012/06/asynchronous-api-in-c-and-the-continuation-monad>`_



.. toctree::
   :maxdepth: 2
   :hidden:

   Home <self>
   gettingstarted
   building
   examples
   reference


Indices and tables
==================

* :ref:`genindex`
* :ref:`search`

