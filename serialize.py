import sys
import binascii
import msgpack
import struct
from autobahn.wamp.serializer import MsgPackSerializer
from autobahn.wamp import message
from autobahn.wamp import role



serializer = MsgPackSerializer()
serializer._serializer.ENABLE_V5 = False

def send(msg):
   bytes, _ = serializer.serialize(msg)
   l = struct.pack("!I", len(bytes))
   sys.stdout.write(l)
   sys.stdout.write(bytes)

msgs = []


## HELLO
##
roles = [
   role.RolePublisherFeatures(),
   role.RoleSubscriberFeatures(),
   role.RoleCallerFeatures(),
   role.RoleCalleeFeatures()
]

msgs.append(message.Hello("foobar", roles))

## CHALLENGE
##
msgs.append(message.Challenge("cookie"))


## HEARTBEAT
##
msgs.append(message.Heartbeat(3, 7, "throw me away"))


for msg in msgs:
   send(msg)
