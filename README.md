# C++ Project Template

A C++ template to quickly start your own project.

This template includes a simplest runnable helloworld program, which breaks down into a `helloworld` lib (see src/ and include/helloworld/) and an executable (see bin/main.cpp).

## Bootstrap

``` bash
$ ./bootstrap.sh
```

## Compile

``` bash
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
# or cmake -DCMAKE_BUILD_TYPE=Release ..
# for more options, see CMakeLists.txt
make -j
```

## Run

``` bash
$ ./bin/main
hello world!
```

## Install & Uninstall

``` bash
# to install libs and headers
sudo make install
# to uninstall
cat install_manifest.txt | sudo xargs rm -rf
```

## clang-format

`clang-format` is an opt-in if you would like to use.

``` bash
sudo apt install clang-format
```

## Doxygen

[doxygen](https://www.doxygen.nl/index.html) is supported if needed.

To generate a document and open it

``` bash
doxygen Doxyfile
open html/index.html
```
