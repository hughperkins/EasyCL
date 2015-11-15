# Quick Test

This is for doing testing of EasyCL, when you have problems with eg DeepCL, cltorch etc.

## Procedure

```
git clone --recursive https://github.com/hughperkins/EasyCL.git
cd EasyCL
mkdir build
cd build
ccmake ..
# press 'c' for configure, set:
#  BUILD_TESTS=ON, 
#  CMAKE_BUILD_TYPE=Debug
# press 'c' for configure
# press 'g' for generate
# => ccmake should exit.  Then do:
make -j 4 install
cp ../test/*.cl .
./easycl_unittests
```

