###############################################################################
##
##  Copyright (C) 2011-2014 Tavendo GmbH
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

import datetime

from autobahn.twisted.wamp import ApplicationSession



class TestService(ApplicationSession):

   def __init__(self, realm = "realm1"):
      ApplicationSession.__init__(self)
      self._realm = realm


   def onConnect(self):
      self.join(self._realm)


   def onJoin(self, details):

      def utcnow():
         now = datetime.datetime.utcnow()
         return now.strftime("%Y-%m-%dT%H:%M:%SZ")

      self.register(utcnow, 'com.timeservice.now')

      def add2(a, b):
         return a + b

      self.register(add2, 'com.mathservice.add2')



if __name__ == '__main__':

   import sys, os, argparse

   from twisted.python import log


   ## parse command line arguments
   ##
   parser = argparse.ArgumentParser()

   parser.add_argument("-d", "--debug", action = "store_true",
                       help = "Enable debug output.")

   parser.add_argument("-w", "--worker", type = str,
                       help = "Path to worker program.")

   args = parser.parse_args()


   ## start Twisted logging to stdout
   ##
   log.startLogging(sys.stdout)


   ## we use an Autobahn utility to install the "best" available Twisted reactor
   ##
   from autobahn.twisted.choosereactor import install_reactor
   reactor = install_reactor()
   if args.debug:
      print("Running on reactor {}".format(reactor))


   ## create a WAMP router factory
   ##
   from autobahn.wamp.router import RouterFactory
   router_factory = RouterFactory()


   ## create a WAMP router session factory
   ##
   from autobahn.twisted.wamp import RouterSessionFactory
   session_factory = RouterSessionFactory(router_factory)
   session_factory.add(TestService())


   from autobahn.wamp.serializer import MsgPackSerializer
   serializer = MsgPackSerializer()

   from autobahn.twisted.rawsocket import WampRawSocketClientFactory, WampRawSocketClientProtocol
   transport_factory = WampRawSocketClientFactory(session_factory, serializer, debug = args.debug)

   class WorkerClientProtocol(WampRawSocketClientProtocol):

      def connectionMade(self):
         WampRawSocketClientProtocol.connectionMade(self)


      def connectionLost(self, reason):
         print "X"*10
         WampRawSocketClientProtocol.connectionLost(self, reason)
         reactor.stop()

   transport_factory.protocol = WorkerClientProtocol


   executable = os.path.abspath(os.path.join(os.getcwd(), args.worker))
   print("Starting worker {}".format(executable))

   args = [executable]

   if False:

      from twisted.internet.endpoints import ProcessEndpoint, StandardErrorBehavior

      ep = ProcessEndpoint(reactor,
                           executable,
                           args,
                           #childFDs = {0: 'w', 1: 'r', 2: 2}, # does not work on Windows
                           errFlag = StandardErrorBehavior.LOG,
                           env = os.environ)
   else:
      from crossbar.process import CustomProcessEndpoint

      ep = CustomProcessEndpoint(reactor,
                                 executable,
                                 args,
                                 name = "Worker",
                                 env = os.environ)


   d = ep.connect(transport_factory)

   def onconnect(res):
      pid = res.transport.pid
      log.msg("Worker forked with PID {}".format(pid))

   def onerror(err):
      log.msg("Could not fork worker: {}".format(err.value))

   d.addCallbacks(onconnect, onerror)



   ## now enter the Twisted reactor loop
   ##
   reactor.run()
