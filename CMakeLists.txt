cmake_minimum_required(VERSION 3.15)
project(kord C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(MSYS2_ROOT "D:/msys64" CACHE PATH "Path to MSYS2 root directory")

    if(NOT EXISTS "${MSYS2_ROOT}")
        set(MSYS2_ROOT "C:/msys64" CACHE PATH "Path to MSYS2 root directory" FORCE)
    endif()

    if(EXISTS "${MSYS2_ROOT}/ucrt64")
        set(CMAKE_C_COMPILER "${MSYS2_ROOT}/ucrt64/bin/gcc.exe")
        set(CMAKE_CXX_COMPILER "${MSYS2_ROOT}/ucrt64/bin/g++.exe")

        set(ZLIB_ROOT "${MSYS2_ROOT}/ucrt64")
        set(zstd_ROOT "${MSYS2_ROOT}/ucrt64")
        set(LLVM_DIR "${MSYS2_ROOT}/ucrt64/lib/cmake/llvm")

        list(APPEND CMAKE_PREFIX_PATH "${MSYS2_ROOT}/ucrt64")
    endif()
elseif(UNIX AND NOT APPLE)
    find_program(LLVM_CONFIG NAMES llvm-config llvm-config-19 llvm-config-18 llvm-config-17 llvm-config-16)
    if(LLVM_CONFIG)
        execute_process(COMMAND ${LLVM_CONFIG} --cmakedir
                OUTPUT_VARIABLE LLVM_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)
    endif()
endif()

find_package(ZLIB REQUIRED)
find_package(zstd REQUIRED)
find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(kord
        src/main.c
        include/staticDefinitions.h
        include/kstring.h
        include/kvariant.h
        include/ktoken.h
        include/kfile.h
        include/kerror.h
        include/kast.h
        include/kmemory.h
        include/kparse.h
)

target_include_directories(kord PRIVATE include)

llvm_map_components_to_libnames(llvm_libs core native)

if(WIN32)
    target_link_libraries(kord PRIVATE
            -Wl,--start-group
            ${llvm_libs}
            -Wl,--end-group
            ZLIB::ZLIB
            zstd
            stdc++
            psapi
            version
            ole32
            uuid
            shell32
            advapi32
    )

    set(MSYS2_BIN_DIR "${MSYS2_ROOT}/ucrt64/bin")
    set(REQUIRED_RUNTIME_DLLS
            "libzstd.dll"
            "libstdc++-6.dll"
            "libgcc_s_seh-1.dll"
            "libwinpthread-1.dll"
            "libz-1.dll"
    )

    foreach(DLL ${REQUIRED_RUNTIME_DLLS})
        add_custom_command(TARGET kord POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MSYS2_BIN_DIR}/${DLL}"
                "$<TARGET_FILE_DIR:kord>/${DLL}"
                COMMENT "Copying runtime dependency: ${DLL}"
        )
    endforeach()

else()
    target_link_libraries(kord PRIVATE
            ${llvm_libs}
            ZLIB::ZLIB
            zstd
            stdc++
    )
endif()