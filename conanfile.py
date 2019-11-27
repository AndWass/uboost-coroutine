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
    options = {"impl": ["boost", "aarch32_aapcs_nofp"], "build_tests": [True, False],
        "build_samples": [True, False]}
    default_options = {"impl": "boost", "build_tests": False, "build_samples": False}
    exports_sources = "CMakeLists.txt", "src/*", "include/*"

    def requirements(self):
        if self.options.impl == "boost":
            self.requires("boost/[>=1.70]@conan/stable")
        if self.options.build_tests == True:
            self.requires("doctest/[>=2.3]@bincrafters/stable")

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["UBOOST_CORO_IMPL"] = self.options.impl
        cmake.definitions["UBOOST_CORO_BUILD_TESTS"] = self.options.build_tests
        cmake.definitions["UBOOST_CORO_BUILD_SAMPLES"] = self.options.build_samples
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
