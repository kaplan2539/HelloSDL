#!/usr/bin/env python

import sys
import os
import subprocess

#cmake_generator="Ninja"
#make_cmd="ninja"
cmake_generator="Unix Makefiles"
make_cmd="make"
cmake_build_dir="build"
cmake_source_dir=os.getcwd()

cmake_cmd = ["cmake","-DCMAKE_BUILD_TYPE=Debug","-G",cmake_generator,cmake_source_dir]


if not os.path.exists(cmake_build_dir):
    os.mkdir(cmake_build_dir)

os.chdir(cmake_build_dir)

subprocess.check_call(cmake_cmd)
subprocess.check_call(make_cmd)
