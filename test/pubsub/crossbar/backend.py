import os
import signal

from autobahn.twisted.wamp import ApplicationSession
from twisted.internet import reactor
from twisted.internet.defer import Deferred
from twisted.internet.defer import inlineCallbacks


class BackendSession(ApplicationSession):
    counter = 0

    @inlineCallbacks
    def onJoin(self, details):
        print("Backend session joined: {}".format(details))

        def pub():
            self.counter += 1
            self.publish('com.example.topic', "data='{}'".format(self.counter))
            return self.counter

        try:
            yield self.register(pub, 'com.example.publish')
        except Exception as e:
            print("could not register procedure: {}".format(e))
