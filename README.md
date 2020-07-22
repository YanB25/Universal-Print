# C++ Project Template

A C++ template for quick start.

## Compile

``` bash
mkdir build; cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..
# or cmake -DCMAKE_BUILD_TYPE=Release ..
# for more options, see CMakeLists.txt
make -j8
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

`clang-format` is an option if you would like to use.

``` bash
sudo apt install clang-format
```
