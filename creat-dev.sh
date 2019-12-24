#! /bin/bash
chcp 65001
echo "生成一个开发用的VS工程目录"
python3 -V
python3 ./tools/DownloadLib.py -d"$HOME/dxlib/download" -l"$HOME/dxlib/lib" -p"linux"  "concurrentqueue"

mkdir build
cd build
conan install .. -s build_type=Release --build missing