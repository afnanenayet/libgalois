cmake_minimum_required(VERSION 3.1)
enable_language(CXX)
find_package(fmt)
include(GNUInstallDirs)
project(libgalois
    VERSION 1.0
    DESCRIPTION "A Galois field library for base 2 fields"
    LANGUAGES CXX)
add_library(galois
    # src
    src/galois.cpp

    # includes
    include/galois.h)
set_target_properties(galois PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER include/galois.h)
target_include_directories(galois PUBLIC include)
target_link_libraries(galois PRIVATE fmt::fmt)
target_compile_features(galois PUBLIC cxx_std_17)
configure_file(cmake/galois.pc.in galois.pc @ONLY)
install(TARGETS galois
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/galois.pc DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
