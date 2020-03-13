# coding: utf-8
from conans import ConanFile, CMake, tools

import os
import sys
import io
# sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='gbk')

os.system("chcp 65001")


class CVSystemConan(ConanFile):
    name = "cvsystem"
    version = "3.0.1"
    license = "私有库"
    author = "daixian<amano_tooko@qq.com>"
    url = "https://github.com/daixian/CVSystem"
    description = "基础的opencv采图系统"
    topics = ("opencv", "daixian")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "build_test": [True, False]}
    default_options = {"shared": False,
                       "build_test": True,
                       "dlog:shared": True,
                       "opencv:shared": True,
                       "boost:without_test": True}
    generators = "cmake"
    exports_sources = "src/*"

    def requirements(self):
        self.requires.add("boost/1.71.0")
        self.requires.add("eigen/3.3.7")
        self.requires.add("xuexuejson/1.0.0@daixian/stable")
        self.requires.add("opencv/3.4.5@daixian/stable")
        self.requires.add("dlog/2.5.0@daixian/stable")
        self.requires.add("poco/1.9.4")

    def build_requirements(self):
        self.build_requires.add("gtest/1.8.1@bincrafters/stable")

    def _configure_cmake(self):
        '''
        转换python的设置到CMake
        '''
        cmake = CMake(self)
        cmake.definitions["CVSYSTEM_BUILD_SHARED"] = self.options.shared
        cmake.definitions["CVSYSTEM_BUILD_TESTS"] = self.options.build_test
        return cmake

    def build(self):
        print("进入了build...")
        cmake = self._configure_cmake()
        cmake.configure(source_folder="src")
        cmake.build()

        # Explicit way:
        # self.run('cmake %s/hello %s'
        #          % (self.source_folder, cmake.command_line))
        # self.run("cmake --build . %s" % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include", src="src")
        self.copy("*.hpp", dst="include", src="src")
        
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["CVSystem"]
