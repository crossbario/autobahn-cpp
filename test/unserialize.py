import sys
import binascii
import msgpack
import struct
from autobahn.wamp.serializer import MsgPackSerializer
from autobahn.wamp import message
from autobahn.wamp import role



serializer = MsgPackSerializer()
serializer._serializer.ENABLE_V5 = False

payload = sys.stdin.read()

print
print len(payload)
print binascii.hexlify(payload)
print

i = 0
while i < len(payload):
   l = struct.unpack("!I", payload[i:i+4])[0]
   print "len = ", l
   obj = serializer.unserialize(payload[i+4:i+4+l])
   print obj
   i += 4 + l
