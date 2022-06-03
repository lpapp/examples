# examples
Example applications

The way to run these examples typically goes like this on my machine. No need
for the conan step if not using conan.

conan install -pr ~/conan/profiles/Release -if build/x64-Release .

cmake -GNinja -DCMAKE_TOOLCHAIN_FILE=conan_paths.cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -Bbuild/x64-Release .

Or something like this for Qt 6 without conan:

cmake -GNinja -DCMAKE_PREFIX_PATH=~/Qt/6.3.0/macos -DCMAKE_BUILD_TYPE=RelWithDebInfo -Bbuild/x64-Release . 

cmake --build build/x64-Release

The Qt examples depend on Qt 6. It would be easy to add the Qt 5 support back,
but these example propagate the use of modern software.

The examples also use modern cmake and C++.
