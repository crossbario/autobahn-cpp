.PHONY: build clean publish

HOSTIP=$(shell ip route get 1 | awk '{print $$NF;exit}')

#
# Don't forget: set and source docker/versions.sh before
# building and publishing the toolchain images!
#

# build toolchain images
build: build_gcc build_clang

build_gcc:
	time docker build --no-cache \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp \
		-t crossbario/autobahn-cpp:gcc \
		-t crossbario/autobahn-cpp:gcc-${AUTOBAHN_CPP_VERSION} \
		-f docker/Dockerfile.gcc .

build_clang:
	time docker build --no-cache \
		--build-arg BUILD_DATE=${BUILD_DATE} \
		--build-arg AUTOBAHN_CPP_VCS_REF=${AUTOBAHN_CPP_VCS_REF} \
		--build-arg AUTOBAHN_CPP_VERSION=${AUTOBAHN_CPP_VERSION} \
		-t crossbario/autobahn-cpp:clang \
		-t crossbario/autobahn-cpp:clang-${AUTOBAHN_CPP_VERSION} \
		-f docker/Dockerfile.clang .

# publish toolchain images
publish: publish_gcc publish_clang

publish_gcc:
	docker push crossbario/autobahn-cpp
	docker push crossbario/autobahn-cpp:gcc
	docker push crossbario/autobahn-cpp:gcc-${AUTOBAHN_CPP_VERSION}

publish_clang:
	docker push crossbario/autobahn-cpp:clang
	docker push crossbario/autobahn-cpp:clang-${AUTOBAHN_CPP_VERSION}

# list/clean (local) toolchain images
list:
	-docker images crossbario/autobahn-cpp:*

clean:
	./docker/removeall.sh
