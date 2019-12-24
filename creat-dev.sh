#!/bin/sh
chcp 65001
echo "生成一个开发用的VS工程目录"
python3 -V
python3 ./tools/DownloadLib.py -d"$HOME/dxlib/download" -l"$HOME/dxlib/lib" -p"linux"  "concurrentqueue"

mkdir build
cd build
conan install .. -s build_type=Release --build missing
# cmake ../src -G "Visual Studio 15 Win64" -DCVSYSTEM_BUILD_SHARED:BOOL="0" -DCVSYSTEM_BUILD_TESTS:BOOL="1" -DCMAKE_CONFIGURATION_TYPES:STRING="Release;RelWithDebInfo"