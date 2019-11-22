from conans import ConanFile, CMake


class UboostcoroutineConan(ConanFile):
    name = "uboost_coroutine"
    version = "0.1.0"
    license = "BSL-1.0"
    author = "Andreas Wass wass.andreas@gmail.com"
    url = "https://gitlab.com/AndWass/uboost-coroutine"
    description = "Stackful coroutines for embedded targets."
    topics = ("embedded", "c++", "coroutine")
    settings = "os", "compiler", "build_type", "arch"
    options = {"impl": ["boost", "aarch32_aapcs_nofp"]}
    default_options = {"impl": "boost"}
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def requirements(self):
        if self.options.impl == "boost":
            self.requires("boost/[1.x]")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["UBOOST_CORO_IMPL"] = self.options.impl
        cmake.configure()
        return cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()

    def package_info(self):
        if self.options.impl == "boost":
            self.cpp_info.defines = ["UBOOST_USE_BOOST"]
        else:
            self.cpp_info.libs = ["uboost_coroutine"]
