from conans import ConanFile, CMake, tools


class autobahn_cppConan(ConanFile):
    name = "autobahn-cpp"
    version = "v20.8.1"
    license = "Boost Software License - Version 1.0 - August 17th, 2003"
    author = "Crossbar.io Technologies GmbH and contributors"
    description = "WAMP for C++ on Boost/ASIO"
    url = "https://github.com/crossbario/autobahn-cpp"
    requires = "openssl/3.0.1", "botan/2.19.1", "boost/1.78.0", "msgpack-cxx/4.0.3", "websocketpp/0.8.2", "catch2/2.13.8"
    generators = "cmake"
    scm = {
        "type": "git",
        "subfolder": ".",
        "url": "auto",
        "revision": "auto"
    }
    no_copy_source = True

    def package(self):
        self.copy("*.hpp", dst="include/autobahn", src="autobahn")
        self.copy("*.ipp", dst="include/autobahn", src="autobahn")

    def package_id(self):
        self.info.header_only()
