#! /bin/bash
export PATH=$PATH:/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin:~/bin
 
conan create . daixian/stable --profile=./linux_arm -s build_type=Release --build missing
