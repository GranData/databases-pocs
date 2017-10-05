
# C++ - Proof of Concept

## Getting started

### Installing and building LLVM

    ```
    $ mkdir <installation-dir-path> && cd <installation-dir-path>
    $ wget http://llvm.org/releases/3.6.2/llvm-3.6.2.src.tar.xz
    $ tar xf llvm-3.6.2.src.tar.xf && cd llvm-3.6.2.src
    $ mkdir build && cd build
    $ ../configure --enable-jit --enable-optimized --enable-targets=host --enable-assertions --prefix=<installation-dir-path>/llvm-3.6.2
    $ make -j3 && sudo make install
    ```

### Building SimpleDB
1. Go to the project source directory
2. Run

    `mkdir build && cd build`

3. Build the project

    ```
    $ cmake .. -DLLVM_DIR=<installation-dir-path>/llvm-3.6.2/share/llvm/cmake
    $ make -j3
    ```

### Running

    `./SimpleDB <project-folder>/resources/random-int32-<size>.column`
