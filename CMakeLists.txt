cmake_minimum_required(VERSION 3.0.0)
project(smb1 VERSION 0.1.0)

include(CTest)
enable_testing()

# todo: change this to use find_package(Python)
if(WIN32)
    set(PYTHON3 py)
else()
    set(PYTHON3 python3)
endif()

# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Custom%20Commands.html
add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/rom.c
    COMMAND ${PYTHON3}
        ${PROJECT_SOURCE_DIR}/scripts/gen_rom_src.py
        ${PROJECT_SOURCE_DIR}/smb1.nes
        ${PROJECT_BINARY_DIR}/rom.c
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

# for msvc
set(NSF_CORE_ONLY true)

add_subdirectory(notsofatso)

add_executable(smb1 WIN32
    src/apu.cpp
    src/cpu.c
    src/main.c
    src/ppu.c
    ${PROJECT_BINARY_DIR}/rom.c
    src/smb1.c
)

if(WIN32)
    # windows
    target_include_directories(smb1 PRIVATE include lib/SDL2/include notsofatso)
    target_link_directories(smb1 PRIVATE ${PROJECT_SOURCE_DIR}/lib/SDL2/lib/x64)
    target_link_libraries(smb1 PRIVATE SDL2 SDL2main notsofatso)
else()
    # unix
    target_include_directories(smb1 PRIVATE include notsofatso)
    target_link_libraries(smb1 PRIVATE SDL2 notsofatso)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
