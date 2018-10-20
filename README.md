# Dependencies
Install the following build dependencies
* meson
* ninja
* conan

Easiest way is using pip with Python3: `pip install meson ninja conan`.
On Linux, use sudo and pip3: `sudo pip3 install meson ninja conan`

# Setup and build
```
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install ..
conan build ..
ninja
```
