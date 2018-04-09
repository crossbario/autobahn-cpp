.PHONY: build

HOSTIP=$(shell ip route get 1 | awk '{print $$NF;exit}')

qemu_deps:
	mkdir -p ./.qemu
	cp /usr/bin/qemu-arm-static ./.qemu/

build: build_gcc build_clang

build_gcc:
	time docker build \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp-armhf \
		-t crossbario/autobahn-cpp-armhf:gcc \
		-t crossbario/autobahn-cpp-armhf:gcc-${AUTOBAHN_CPP_VERSION} \
		-f Dockerfile.gcc .

build_clang:
	time docker build \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp-armhf:clang \
		-t crossbario/autobahn-cpp-armhf:clang-${AUTOBAHN_CPP_VERSION} \
		-f Dockerfile.clang .

use_gcc: build_gcc
	docker run -it --rm \
		--net=host \
		-v ${PWD}/.build:/build \
		-e AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		crossbario/autobahn-cpp-armhf:gcc \
		/usr/local/bin/build_scons

shell_clang:
	docker run -it --rm --net=host -e AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} crossbario/autobahn-cpp-armhf:clang bash

test_gcc:
	docker run -it --rm crossbario/autobahn-cpp-armhf:gcc bash -c "cd /usr/local/app && make && ./client ws://$(HOSTIP):8080/ws realm1"

test_clang:
	docker run -it --rm crossbario/autobahn-cpp-armhf:clang bash -c "cd /usr/local/app && make && ./client ws://$(HOSTIP):8080/ws realm1"

publish: publish_gcc publish_clang

publish_gcc:
	docker push crossbario/autobahn-cpp-armhf
	docker push crossbario/autobahn-cpp-armhf:gcc
	docker push crossbario/autobahn-cpp-armhf:gcc-${AUTOBAHN_CPP_VERSION}

publish_clang:
	docker push crossbario/autobahn-cpp-armhf:clang
	docker push crossbario/autobahn-cpp-armhf:clang-${AUTOBAHN_CPP_VERSION}

list:
	-docker images crossbario/autobahn-cpp-armhf:*

clean:
	./removeall.sh
