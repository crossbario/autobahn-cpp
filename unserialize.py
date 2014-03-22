import sys
import binascii
import msgpack
from autobahn.wamp.serializer import MsgPackSerializer

serializer = MsgPackSerializer()
serializer._serializer.ENABLE_V5 = False

payload = sys.stdin.read()

print
print len(payload)
print binascii.hexlify(payload)
print


obj = serializer.unserialize(payload)
print obj

# obj = [1, "realm2", {"roles": {"caller": True, "callee": True, "publisher": True, "subscriber": True}}]
# obj = [1, "realm2"]


# payload = serializer._serializer.serialize(obj)
# #payload = msgpack.packb(obj, use_bin_type = False)

# print len(payload)
# print binascii.hexlify(payload)

