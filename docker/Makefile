SUBDIRS = x86_64 armhf aarch64

subdirs: $(SUBDIRS)

BUILDDIRS = $(SUBDIRS:%=build-%)
BUILDGCCDIRS = $(SUBDIRS:%=build-gcc-%)
BUILDCLANGDIRS = $(SUBDIRS:%=build-clang-%)
VERSIONDIRS = $(SUBDIRS:%=version-%)
TESTDIRS = $(SUBDIRS:%=test-%)

PUBLISHDIRS = $(SUBDIRS:%=publish-%)
PUBLISHGCCDIRS = $(SUBDIRS:%=publish-gcc-%)
PUBLISHCLANGDIRS = $(SUBDIRS:%=publish-clang-%)

build: $(BUILDDIRS)
build_gcc: $(BUILDGCCDIRS)
build_clang: $(BUILDCLANGDIRS)

version: $(VERSIONDIRS)

test: $(TESTDIRS)

publish: $(PUBLISHDIRS)
publish_gcc: $(PUBLISHGCCDIRS)
publish_clang: $(PUBLISHCLANGDIRS)

.PHONY: subdirs $(BUILDDIRS) $(BUILDGCCDIRS) $(BUILDCLANGDIRS) $(VERSIONDIRS) $(TESTDIRS) $(PUBLISHDIRS) $(PUBLISHGCCDIRS) $(PUBLISHCLANGDIRS)
.PHONY: build version test publish

$(BUILDDIRS):
	$(MAKE) -C $(@:build-%=%) build

$(BUILDGCCDIRS):
	$(MAKE) -C $(@:build-gcc-%=%) build_gcc

$(BUILDCLANGDIRS):
	$(MAKE) -C $(@:build-clang-%=%) build_clang

$(VERSIONDIRS):
	$(MAKE) -C $(@:version-%=%) version

$(TESTDIRS):
	$(MAKE) -C $(@:test-%=%) test

$(PUBLISHDIRS):
	$(MAKE) -C $(@:publish-%=%) publish

$(PUBLISHGCCDIRS):
	$(MAKE) -C $(@:publish-gcc-%=%) publish_gcc

$(PUBLISHCLANGDIRS):
	$(MAKE) -C $(@:publish-clang-%=%) publish_clang

list:
	docker images crossbario/autobahn-cpp*:*

clean:
	./removeall.sh
