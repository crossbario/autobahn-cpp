###############################################################################
##
##  Copyright (C) 2014 Tavendo GmbH
##
##  Licensed under the Apache License, Version 2.0 (the "License");
##  you may not use this file except in compliance with the License.
##  You may obtain a copy of the License at
##
##      http://www.apache.org/licenses/LICENSE-2.0
##
##  Unless required by applicable law or agreed to in writing, software
##  distributed under the License is distributed on an "AS IS" BASIS,
##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
##  See the License for the specific language governing permissions and
##  limitations under the License.
##
###############################################################################

import sys
import datetime

from twisted.python import log
from twisted.internet import reactor
from twisted.internet.defer import Deferred, \
                                   inlineCallbacks, \
                                   returnValue
from twisted.internet.endpoints import serverFromString

from autobahn.twisted.util import sleep
from autobahn.wamp.router import RouterFactory
from autobahn.wamp.serializer import MsgPackSerializer
from autobahn.twisted.wamp import ApplicationSession
from autobahn.twisted.wamp import RouterSessionFactory
from autobahn.twisted.websocket import WampWebSocketServerFactory
from autobahn.twisted.rawsocket import WampRawSocketServerFactory



class MyBackendComponent(ApplicationSession):
   """
   Application code goes here. This is an example component that provides
   a simple procedure which can be called remotely from any WAMP peer.
   It also publishes an event every second to some topic.
   """
   def onConnect(self):
      self.join("realm1")

   @inlineCallbacks
   def onJoin(self, details):

      ## subscribe to a topic
      ##
      def onevent(*args, **kwargs):
         print("Got event: {} {}".format(args, kwargs))

      yield self.subscribe(onevent, 'com.myapp.topic2')

      ## register a procedure for remote calling
      ##
      def utcnow():
         print("Someone is calling me;)")
         now = datetime.datetime.utcnow()
         return now.strftime("%Y-%m-%dT%H:%M:%SZ")

      yield self.register(utcnow, 'com.timeservice.now')

      def square(x):
         return x * x

      yield self.register(square, 'com.math.square')

      @inlineCallbacks
      def slowsquare(x, delay = 1):
         print("slowsquare with delay = {}".format(delay))
         yield sleep(delay)
         returnValue(x * x)

      yield self.register(slowsquare, 'com.math.slowsquare')

      def add2(a, b):
         return a + b

      yield self.register(add2, 'com.mathservice.add2')

      def ping():
         return

      def add2(a, b):
         return a + b

      def stars(nick = "somebody", stars = 0):
         return "{} starred {}x".format(nick, stars)

      def orders(product, limit = 5):
         return ["Product {}".format(i) for i in range(50)][:limit]

      def arglen(*args, **kwargs):
         return [len(args), len(kwargs)]

      yield self.register(ping, 'com.arguments.ping')
      yield self.register(add2, 'com.arguments.add2')
      yield self.register(stars, 'com.arguments.stars')
      yield self.register(orders, 'com.arguments.orders')
      yield self.register(arglen, 'com.arguments.arglen')


      def add_complex(a, ai, b, bi):
         return CallResult(c = a + b, ci = ai + bi)

      yield self.register(add_complex, 'com.myapp.add_complex')

      def split_name(fullname):
         forename, surname = fullname.split()
         return CallResult(forename, surname)

      yield self.register(split_name, 'com.myapp.split_name')


      def numbers(start, end, prefix = "Number: "):
         res = []
         for i in range(start, end):
            res.append(prefix + str(i))
         return res

      yield self.register(numbers, 'com.arguments.numbers')

      ## publish events to a topic
      ##
      counter = 0
      while True:
         self.publish('com.myapp.topic1', counter)
         print("Published event: {}".format(counter))
         counter += 1
         yield sleep(1)




if __name__ == '__main__':

   ## 0) start logging to console
   log.startLogging(sys.stdout)

   ## 1) create a WAMP router factory
   router_factory = RouterFactory()

   ## 2) create a WAMP router session factory
   session_factory = RouterSessionFactory(router_factory)

   ## 3) Optionally, add embedded WAMP application sessions to the router
   session_factory.add(MyBackendComponent())

   ## 4) create a WAMP-over-WebSocket transport server factory
   transport_factory1 = WampWebSocketServerFactory(session_factory, debug = False)

   ## 5) start the server from a Twisted endpoint
   server1 = serverFromString(reactor, "tcp:8080")
   server1.listen(transport_factory1)

   ## 6) create a WAMP-over-RawSocket-MsgPack transport server factory
   serializer = MsgPackSerializer()
   serializer._serializer.ENABLE_V5 = False
   transport_factory2 = WampRawSocketServerFactory(session_factory, serializer, debug = True)

   ## 7) start the server from a Twisted endpoint
   server2 = serverFromString(reactor, "tcp:8090")
   server2.listen(transport_factory2)

   ## 8) now enter the Twisted reactor loop
   reactor.run()
