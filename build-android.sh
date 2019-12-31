#! /bin/bash
conan create . daixian/stable --profile=./android  -o dlog:shared=True  -o shared=False --build missing