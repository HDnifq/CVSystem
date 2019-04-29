python -V
python ./tools/DownloadLib.py -d "C:/dxlib/download" -l "C:/dxlib/lib" "concurrentqueue" "dlog" "gtest" "boost" "opencv3" "eigen" "rapidjson"
mkdir build
cd build
mkdir Win64
cd Win64
REM cmake.exe -G "Visual Studio 15 2017 Win64" "../../" -DBUILD_GMOCK:BOOL="0" -DINSTALL_GTEST:BOOL="0" -Dgtest_force_shared_crt:BOOL="1" -DBUILD_TESTING:BOOL="0" -DENABLE_TEST:BOOL="1" -DCMAKE_CONFIGURATION_TYPES:STRING="Release;RelWithDebInfo"
REM pause
REM cd ../
REM mkdir Win32
REM cd Win32
REM cmake.exe -G "Visual Studio 15 2017" "../../" -DBUILD_GMOCK:BOOL="0" -DINSTALL_GTEST:BOOL="0" -DBUILD_TESTING:BOOL="0" -DCMAKE_INSTALL_PREFIX:PATH="./install" -DENABLE_TEST:BOOL="1" -DCMAKE_CONFIGURATION_TYPES:STRING="Release;RelWithDebInfo" -Dgtest_force_shared_crt:BOOL="1"
REM pause
