# Dependencies
Install the following tools:
* conan
* cmake

# Setup and build
```
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install ..
cmake ..
cmake --build .
```

For Windows, use `cmake .. -G "Visual Studio 15 2017 Win64"` instead of `cmake ..`.
