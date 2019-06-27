cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
find_package(fmt)
project(libgalois
    VERSION 1.0
    DESCRIPTION "A Galois field library for base 2 fields"
    LANGUAGES CXX)
add_library(galois
    # src
    src/galois.cpp

    # includes
    include/galois.h)
target_include_directories(galois PUBLIC include)
target_link_libraries(galois PRIVATE fmt::fmt)
target_compile_features(galois PUBLIC cxx_std_17)
