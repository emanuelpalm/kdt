cmake_minimum_required(VERSION 3.8)
project(kdt)

set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__FILE_REL__='\"$(subst ${CMAKE_SOURCE_DIR}/src/,,$(abspath $<))\"'")

# Libraries.

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(OpenSSL REQUIRED)
list(APPEND MAIN_DEFINITIONS KDT_USE_OPENSSL)
list(APPEND MAIN_INCLUDE_DIRS ${OPENSSL_INCLUDE_DIR})
list(APPEND MAIN_LIBRARIES ${OPENSSL_LIBRARIES})

find_package(LMDB REQUIRED)
list(APPEND MAIN_DEFINITIONS KDT_USE_LMDB)
list(APPEND MAIN_INCLUDE_DIRS ${LMDB_INCLUDE_DIR})
list(APPEND MAIN_LIBRARIES ${LMDB_LIBRARIES})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
list(APPEND MAIN_DEFINITIONS KDT_USE_POSIX)
list(APPEND MAIN_LIBRARIES Threads::Threads)

# Client.

list(APPEND MAIN_INCLUDE_DIRS src/main)
set(MAIN_SOURCE
    src/main/kdt/kdm/internal/bucket.c
    src/main/kdt/kdm/internal/bucket.h
    src/main/kdt/kdm/internal/cli.c
    src/main/kdt/kdm/internal/cli.h
    src/main/kdt/kdm/internal/cursor.c
    src/main/kdt/kdm/internal/cursor.h
    src/main/kdt/kdm/internal/message.c
    src/main/kdt/kdm/internal/message.h
    src/main/kdt/kdm/internal/protocol.c
    src/main/kdt/kdm/internal/protocol.h
    src/main/kdt/kdm/internal/table.c
    src/main/kdt/kdm/internal/table.h
    src/main/kdt/kdm/contact.c
    src/main/kdt/kdm/contact.h
    src/main/kdt/kdm/kdm.h
    src/main/kdt/kdm/kdm.c
    src/main/kdt/pnet/internal/event.h
    src/main/kdt/pnet/internal/header.h
    src/main/kdt/pnet/internal/message.h
    src/main/kdt/pnet/internal/receiver.c
    src/main/kdt/pnet/internal/receiver.h
    src/main/kdt/pnet/internal/sender.c
    src/main/kdt/pnet/internal/sender.h
    src/main/kdt/pnet/internal/server.c
    src/main/kdt/pnet/internal/server.h
    src/main/kdt/pnet/internal/socket.c
    src/main/kdt/pnet/internal/socket.h
    src/main/kdt/pnet/internal/socket_set.c
    src/main/kdt/pnet/internal/socket_set.h
    src/main/kdt/pnet/event.h
    src/main/kdt/pnet/message.h
    src/main/kdt/pnet/host.c
    src/main/kdt/pnet/host.h
    src/main/kdt/pnet/pnet.c
    src/main/kdt/pnet/pnet.h
    src/main/kdt/bitset.c
    src/main/kdt/bitset.h
    src/main/kdt/cbuf.c
    src/main/kdt/cbuf.h
    src/main/kdt/def.h
    src/main/kdt/endian.h
    src/main/kdt/err.c
    src/main/kdt/err.h
    src/main/kdt/kint.c
    src/main/kdt/kint.h
    src/main/kdt/kvs.c
    src/main/kdt/kvs.h
    src/main/kdt/log.c
    src/main/kdt/log.h
    src/main/kdt/mem.c
    src/main/kdt/mem.h
    src/main/kdt/mtx.c
    src/main/kdt/mtx.h
    src/main/kdt/options.c
    src/main/kdt/options.h
    src/main/kdt/tims.c
    src/main/kdt/tims.h
    src/main/main.c)
add_executable(kdt ${MAIN_SOURCE})
target_compile_definitions(kdt PRIVATE ${MAIN_DEFINITIONS})
target_include_directories(kdt PRIVATE ${MAIN_INCLUDE_DIRS})
target_link_libraries(kdt ${MAIN_LIBRARIES})

# Unit tests.

set(TEST_DEFINITIONS
    ${MAIN_DEFINITIONS}
    KDT_TEST)
list(APPEND TEST_INCLUDES ${MAIN_INCLUDE_DIRS} src/test)
set(TEST_OPTIONS
    -DKDT_B=128
    -DKDT_B1=64
    -DKDT_K=4)
list(FILTER MAIN_SOURCE EXCLUDE REGEX "main.c$")
set(TEST_SOURCE
    ${MAIN_SOURCE}
    src/test/kdt/kdm/internal/bucket.unit.c
    src/test/kdt/kdm/internal/table.unit.c
    src/test/kdt/kdm/contact.unit.c
    src/test/kdt/pnet/host.unit.c
    src/test/kdt/cbuf.unit.c
    src/test/kdt/bitset.unit.c
    src/test/kdt/kint.unit.c
    src/test/kdt/kvs.unit.c
    src/test/unit/unit.c
    src/test/unit/unit.h
    src/test/main.c)
add_executable(kdt-test ${TEST_SOURCE})
target_compile_definitions(kdt-test PRIVATE ${TEST_DEFINITIONS})
target_compile_options(kdt-test PRIVATE ${TEST_OPTIONS})
target_include_directories(kdt-test PRIVATE ${TEST_INCLUDES})
target_link_libraries(kdt-test ${MAIN_LIBRARIES})
