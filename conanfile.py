from importlib.metadata import requires
from logging import exception
from conans import ConanFile, CMake, tools
from conan.tools.cmake import CMakeToolchain


class BoostEchoServer(ConanFile):
    name = "boost_echo_server"
    version = "0.1.0"
	
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"

    scm = {"type": "git", "url": "auto", "revision": "auto"}

    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {
        "shared": False,
        "fPIC": True,
        "boost:header_only": False
    }
	
    requires = [
        'boost/1.79.0',
        'spdlog/1.10.0'
	]

    
    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

        if tools.get_env("CONAN_RUN_TESTS", True):
            with tools.run_environment(self):
                cmake.test(output_on_failure=True)

    def package(self):
        pass

    def package_info(self):
        pass
