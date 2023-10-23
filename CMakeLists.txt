cmake_minimum_required(VERSION 3.10)
project (Images2Video)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_PDB_OUTPUT_DIRECTORY     "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(jtk)
add_subdirectory(Images2Video)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_target_properties (jtk PROPERTIES FOLDER jtk)
set_target_properties (jtk.static.tests PROPERTIES FOLDER jtk)
set_target_properties (jtk.tests PROPERTIES FOLDER jtk)
