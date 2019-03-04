# libh5pp
libh5pp is a C++ wrapper for HDF5 that focuses on simplicity for the end-user. 

## Features
* Support for common data types:
    - `ìnt`, `float`, `double` in unsigned and long versions.
    - any of the above types in std::complex<> form.
    - `std::string`
    - `std::vector`
    - `Eigen` types such as `Matrix`, `Array` and `Tensor` (from the unsupported module), with automatic conversion to/from row-major storage.
* Standard CMake build, install and linking. 
* Automated install of dependencies. 

## Usage

```c++

#include <iostream>
#include <h5pp/h5pp.h>
using namespace std::complex_literals;

int main(){
    
    # Initialize a file
    h5pp::File file("someFile.h5", "outputDir");

    // Write a vector with std::complex<double>
    std::vector<std::complex<double>> testvector (5, 10.0 + 5.0i);
    file.write_dataset(testvector,"testvector");

    // Write an Eigen matrix with std::complex<double>
    Eigen::MatrixXcd testmatrix (2,2);
    testdata3 << 1.0 + 2.0i,  3.0 + 4.0i, 5.0+6.0i , 7.0+8.0i;
    file.write_dataset(testmatrix,"someGroup/testmatrix");


    return 0;
}

```


## Requirements
* C++17 capable compiler.
* CMake 3.11
* Optional dependencies:
    - [**HDF5**](https://support.hdfgroup.org/HDF5/) (tested with version >= 1.10).
    - [**Eigen**](http://eigen.tuxfamily.org) (tested with version >= 3.3.4).

The build process will attempt to find the libraries above installed on the system.
On failure it wll download and install them into the given install-directory (default: `project-dir/install/`).
To modify this behavior see the available build options.

During the build process the dependency [**spdlog**](https://github.com/gabime/spdlog) will be downloaded and installed in a local subdirectory.


## Installation
Build the library just as any CMake project:

```bash
    mkdir build
    cmake ../
    make
    make install
```

By default all dependencies are downloaded and installed in `project-dir/install/` if not found in the system.

### Build options

The `cmake` step above takes several options, `cmake [-DOPTIONS=var] ../ `:
* `-DCMAKE_INSTALL_PREFIX:PATH=<install-dir>` to specify install directory (default: `project-dir/install/`).
* `-DDOWNLOAD_HDF5:BOOL=<ON/OFF>` to toggle automatic installation of HDF5 (default: `OFF`).
* `-DDOWNLOAD_EIGEN3:BOOL=<ON/OFF>` to toggle automatic installation of HDF5 (default: `OFF`).
* `-DDOWNLOAD_ALL:BOOL=<ON/OFF>` to toggle automatic installation of third-party dependencies (default: `OFF`).
* `-DCMAKE_BUILD_TYPE=Release/Debug` to specify build type (default: `Release`)
* `-DBUILD_SHARED_LIBS:BOOL=<ON/OFF>` to toggle shared/static library (default: `OFF`)
* `-DMARCH=<micro-architecture>` to specify compiler micro-architecture (default: `native`)

In addition, the following variables can be set to help guide CMake's `find_package()` to your preinstalled software (no defaults):

* `-DEigen3_DIR:PATH=<path to Eigen3Config.cmake>` 
* `-DEigen3_ROOT:PATH=<path to Eigen3 install-dir>` 
* `-DEigen3_ROOT_DIR:PATH=<path to Eigen3 install-dir>` 
* `-DEIGEN3_INCLUDE_DIR:PATH=<path to Eigen3 include-dir>`
* `-DHDF5_DIR:PATH=<path to HDF5Config.cmake>` 
* `-DHDF5_ROOT:PATH=<path-to HDF5 install-dir>` 



### Linking 

After installing the library it is easily imported using CMake's `find_package()`, just point it to the install directory.

```cmake
    cmake_minimum_required(VERSION 3.10)
    project(FooProject)
    
    add_executable(${PROJECT_NAME} foo.cpp)
    
    find_package(h5pp HINTS <path to h5pp-install-dir> REQUIRED)
    
    if (h5pp_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE h5pp::h5pp)
    endif()
```
