#! /bin/bash
# PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
# export PATH

conan create . daixian/stable --build missing
conan upload cvsystem/3.0.0@daixian/stable --all -r=conan-local