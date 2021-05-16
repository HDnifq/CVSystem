#!/usr/bin/env python3
# coding=utf-8

import os
import sys
import io
from conans import ConanFile, CMake, tools

# sys.stdout = io.TextIOWrapper(sys.stdout.buffer, encoding='gbk')

os.system("chcp 65001")


class CVSystemConan(ConanFile):
    name = "cvsystem"
    version = "3.3.35"
    license = "私有库"
    author = "daixian<amano_tooko@qq.com>"
    url = "https://github.com/daixian/CVSystem"
    description = "基础的opencv采图系统"
    topics = ("opencv", "daixian")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False]}
    default_options = {"shared": False,
                       "dlog:shared": False,
                       "opencv:shared": True,
                       "boost:without_test": True,
                       "poco:enable_data_sqlite": False}
    generators = "cmake"
    exports_sources = "src/*"

    def requirements(self):
        # self.requires("boost/1.71.0")
        self.requires("eigen/3.3.7")
        self.requires("opencv/4.3.0@daixian/stable")
        self.requires("dlog/[>=2.5.2]@daixian/stable")
        self.requires("xuexuejson/[>=1.3.0]@daixian/stable")
        self.requires("xuexuemath/[>=0.0.5]@daixian/stable")
        self.requires("sqlitecpp/2.5.0@daixian/stable")  # 使用加密数据库的版本
        self.requires("xuexuesharp/[>=0.0.4]@daixian/stable")
        self.requires("Crypto/[>=0.0.1]@daixian/stable")

    def _configure_cmake(self):
        '''
        转换python的设置到CMake
        '''
        cmake = CMake(self)
        # 目前没有写导出类,所以实际上不能支持BUILD_SHARED
        cmake.definitions["CVSYSTEM_BUILD_SHARED"] = self.options.shared
        return cmake

    def build(self):
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
        if self.settings.os == "Windows":
            self.cpp_info.libs.append("baseclasses")
