chcp 65001
REM 在windows平台编译arm7的包，工具链的下载地址http://gnutoolchains.com/raspberry/
set CONAN_REVISIONS_ENABLED=1
conan create . daixian/stable --profile=./win_to_rpi -o shared=False --build=missing
conan upload cvsystem/3.0.0@daixian/stable --all -r=conan-local