import sys
import binascii
import msgpack
import struct

payload = sys.stdin.read()

print
print len(payload)
print binascii.hexlify(payload)
print

obj = msgpack.unpackb(payload)

print obj
