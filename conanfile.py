from conans import ConanFile, CMake


class UboostcoroutineConan(ConanFile):
    name = "uboost_coroutine"
    version = "0.1.0"
    license = "Boost 1.0"
    author = "Andreas Wass wass.andreas@gmail.com"
    url = ""
    description = "Stackful coroutines for embedded targets."
    topics = ("embedded", "c++", "coroutine")
    settings = "os", "compiler", "build_type", "arch"
    options = {"impl": ["boost", "arm_none_eabi_cortex_nofp"]}
    default_options = {"impl": "boost"}
    generators = "cmake"
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
        self.cpp_info.libs = ["uboost_coroutine"]
