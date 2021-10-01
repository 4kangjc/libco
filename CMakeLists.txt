cmake_minimum_required(VERSION 2.8)
project(libco)

# Set cflags
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} -g -fno-strict-aliasing -O2 -Wall -export-dynamic -Wall -pipe  -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64)

# Use c and asm
enable_language(C ASM)

include_directories(./)

set(LIB_SRC
    libco/coctx.cpp
    libco/coctx_swap.S
    libco/co_routine.cpp
)


add_library(shared_co SHARED ${LIB_SRC})

add_executable(test tests/test.cc)
target_link_libraries(test shared_co)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)