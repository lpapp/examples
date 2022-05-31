# examples
Testbed examples

The way to run these examples typically goes like on my machine. No need for
the conan step if not using conan.

conan install -pr ~/conan/profiles/Release -if build/x64-Release .

cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -Bbuild/x64-Release .

Or something like this for Qt 6:

cmake -GNinja -DCMAKE_PREFIX_PATH=~/Qt/6.2.4/macos -DCMAKE_BUILD_TYPE=RelWithDebInfo -Bbuild/x64-Release . 

cmake --build build/x64-Release
