cmake_minimum_required(VERSION 3.10)
project(NexusTitan CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT MSVC)
    add_compile_options(-Wall -O3)
endif()


set(SOURCES
    src/common.cpp
    src/networking.cpp
    src/gui.cpp
    src/gui_win32.cpp
    src/gui_dialogs.cpp
    src/html.cpp
    src/interpreter_core.cpp
    src/stdlib_basic.cpp
    src/stdlib_io.cpp
    src/stdlib_gui.cpp
    src/parser.cpp
    src/main.cpp
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nexus ${SOURCES})

if(WIN32)
    target_link_libraries(nexus wininet)
else()
    find_package(X11 REQUIRED)
    find_package(OpenSSL REQUIRED)
    
    target_include_directories(nexus PRIVATE ${X11_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(nexus ${X11_LIBRARIES} OpenSSL::SSL OpenSSL::Crypto)
endif()
