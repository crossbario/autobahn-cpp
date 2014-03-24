build:
	scons

test_unser:
	scons
	./build/test/test7 | python -u unserialize.py
#	./build/test/test2 | python -u unserialize.py

test_ser:
	scons
	python -u serialize.py | ./build/test/test3


test_foo:
	scons
	./build/test/test4 | python -u rawunser.py
