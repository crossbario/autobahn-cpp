.PHONY: build

HOSTIP=$(shell ip route get 1 | awk '{print $$NF;exit}')

qemu_deps:
	mkdir -p ./.qemu
	cp /usr/bin/qemu-aarch64-static ./.qemu/

build: build_gcc build_clang

build_gcc:
	time docker build \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp-aarch64 \
		-t crossbario/autobahn-cpp-aarch64:gcc \
		-t crossbario/autobahn-cpp-aarch64:gcc-${AUTOBAHN_CPP_VERSION} \
		-f Dockerfile.gcc .

build_clang:
	time docker build \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp-aarch64:clang \
		-t crossbario/autobahn-cpp-aarch64:clang-${AUTOBAHN_CPP_VERSION} \
		-f Dockerfile.clang .

use_gcc: build_gcc
	docker run -it --rm \
		--net=host \
		-v ${PWD}/.build:/build \
		-e AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		crossbario/autobahn-cpp-aarch64:gcc \
		/usr/local/bin/build_scons

shell_clang:
	docker run -it --rm --net=host -e AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} crossbario/autobahn-cpp-aarch64:clang bash

test_gcc:
	docker run -it --rm crossbario/autobahn-cpp-aarch64:gcc bash -c "cd /usr/local/app && make && ./client ws://$(HOSTIP):8080/ws realm1"

test_clang:
	docker run -it --rm crossbario/autobahn-cpp-aarch64:clang bash -c "cd /usr/local/app && make && ./client ws://$(HOSTIP):8080/ws realm1"

publish: publish_gcc publish_clang

publish_gcc:
	docker push crossbario/autobahn-cpp-aarch64
	docker push crossbario/autobahn-cpp-aarch64:gcc
	docker push crossbario/autobahn-cpp-aarch64:gcc-${AUTOBAHN_CPP_VERSION}

publish_clang:
	docker push crossbario/autobahn-cpp-aarch64:clang
	docker push crossbario/autobahn-cpp-aarch64:clang-${AUTOBAHN_CPP_VERSION}

list:
	-docker images crossbario/autobahn-cpp-aarch64:*

clean:
	./removeall.sh
