import sys
import binascii
import msgpack
import struct
from autobahn.wamp.serializer import MsgPackSerializer

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

# obj = [1, "realm2", {"roles": {"caller": True, "callee": True, "publisher": True, "subscriber": True}}]
# obj = [1, "realm2"]


# payload = serializer._serializer.serialize(obj)
# #payload = msgpack.packb(obj, use_bin_type = False)

# print len(payload)
# print binascii.hexlify(payload)

