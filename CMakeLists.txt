cmake_minimum_required(VERSION 3.27)
project(nimble_linux)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)

file(GLOB NIMBLE_SRC_FILES
        nimble/src/*.c
        nimble/host/src/*.c
        nimble/host/util/src/*.c
        nimble/host/services/ans/src/*.c
        nimble/host/services/bas/src/*.c
        nimble/host/services/gap/src/*.c
        nimble/host/services/gatt/src/*.c
        nimble/host/services/dis/src/*.c
        nimble/host/services/ias/src/*.c
        nimble/host/services/lls/src/*.c
        nimble/host/services/tps/src/*.c
        nimble/host/store/ram/src/*.c
        nimble/transport/src/*.c
        ext/tinycrypt/src/*.c
        porting/nimble/src/*.c)
file(GLOB_RECURSE LINUX_PORT_FILES porting/npl/linux/src/*.c porting/npl/linux/src/*.cc nimble/transport/socket/src/*.c)
list(FILTER NIMBLE_SRC_FILES EXCLUDE REGEX  "hal_timer.c")
add_library(nimble_linux ${LINUX_PORT_FILES} ${NIMBLE_SRC_FILES})
target_include_directories(nimble_linux PUBLIC include
        ext/tinycrypt/include
        porting/npl/linux/include
        nimble/transport/socket/include
        nimble/include
        nimble/host/include
        nimble/host/services/ans/include
        nimble/host/services/bas/include
        nimble/host/services/bleuart/include
        nimble/host/services/gap/include
        nimble/host/services/gatt/include
        nimble/host/services/ias/include
        nimble/host/services/dis/include
        nimble/host/services/lls/include
        nimble/host/services/tps/include
        nimble/host/store/ram/include
        nimble/host/util/include
        nimble/transport/include
        porting/nimble/include)