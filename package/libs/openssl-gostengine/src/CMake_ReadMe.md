## CMake Config

Configuring with `cmake` is can very simply be done like this:

~~~ bash
cmake .
~~~

If you want to build against a specific OpenSSL installation (if you have
more than one, or your own private install, or...), you can use the `cmake`
variable `CMAKE_C_FLAGS`:

~~~ bash
cmake -DCMAKE_C_FLAGS='-I/PATH/TO/OPENSSL/include -L/PATH/TO/OPENSSL/lib' .
~~~

Build example:

~~~ bash
mkdir build
cd build
cmake -DCMAKE_C_FLAGS='-I/PATH/TO/OPENSSL/include -L/PATH/TO/OPENSSL/lib' ..
make -j 8
cd ../bin
~~~
