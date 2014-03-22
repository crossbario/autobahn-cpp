all:
	scons
	./build/test/test2 | python -u unserialize.py
