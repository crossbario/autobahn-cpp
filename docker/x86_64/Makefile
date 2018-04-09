.PHONY: build

HOSTIP=$(shell ip route get 1 | awk '{print $$NF;exit}')

build: build_gcc build_clang

build_gcc:
	time docker build \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp \
		-t crossbario/autobahn-cpp:gcc \
		-t crossbario/autobahn-cpp:gcc-${AUTOBAHN_CPP_VERSION} \
		-f Dockerfile.gcc .

build_clang:
	time docker build \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp:clang \
		-t crossbario/autobahn-cpp:clang-${AUTOBAHN_CPP_VERSION} \
		-f Dockerfile.clang .

build_test_gcc:
	docker run -it --rm \
		--net=host \
		-v ${PWD}/.build:/build \
		-e AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		crossbario/autobahn-cpp:gcc \
		/usr/local/bin/build_scons

build_test_clang:
	docker run -it --rm \
		--net=host \
		-v ${PWD}/.build:/build \
		-e AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		crossbario/autobahn-cpp:clang \
		/usr/local/bin/build_scons

test_gcc:
	docker run -it --rm crossbario/autobahn-cpp:gcc bash -c "cd /usr/local/app && make && ./client ws://$(HOSTIP):8080/ws realm1"

test_clang:
	docker run -it --rm crossbario/autobahn-cpp:clang bash -c "cd /usr/local/app && make && ./client ws://$(HOSTIP):8080/ws realm1"


publish: publish_gcc publish_clang

publish_gcc:
	docker push crossbario/autobahn-cpp
	docker push crossbario/autobahn-cpp:gcc
	docker push crossbario/autobahn-cpp:gcc-${AUTOBAHN_CPP_VERSION}

publish_clang:
	docker push crossbario/autobahn-cpp:clang
	docker push crossbario/autobahn-cpp:clang-${AUTOBAHN_CPP_VERSION}

list:
	-docker images crossbario/autobahn-cpp:*

clean:
	./removeall.sh
