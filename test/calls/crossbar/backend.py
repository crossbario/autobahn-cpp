import os
import signal

from autobahn.twisted.wamp import ApplicationSession
from twisted.internet import reactor
from twisted.internet.defer import Deferred
from twisted.internet.defer import inlineCallbacks


class BackendSession(ApplicationSession):

    @inlineCallbacks
    def onJoin(self, details):
        print("Backend session joined: {}".format(details))

        ## REGISTER a procedure for remote calling
        ##
        def add2(x, y):
            print("add2() called with {} and {}".format(x, y))
            return x + y

        try:
            reg = yield self.register(add2, 'com.example.add2')
            print("procedure add2() registered")
        except Exception as e:
            print("could not register procedure: {}".format(e))
