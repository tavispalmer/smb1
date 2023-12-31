cmake_minimum_required(VERSION 3.0.0)
project(smb1 VERSION 0.1.0)

include(CTest)
enable_testing()

# https://cmake.org/cmake/help/book/mastering-cmake/chapter/Custom%20Commands.html
add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/rom.c
    COMMAND python3
        ${PROJECT_SOURCE_DIR}/scripts/gen_rom_src.py
        ${PROJECT_SOURCE_DIR}/smb1.nes
        ${PROJECT_BINARY_DIR}/rom.c
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
)

add_subdirectory(notsofatso)

if(WIN32)
    add_subdirectory(glfw)
    add_subdirectory(openal-soft)
endif()

set(GLAD_SRC
    src/glad/gl.c
)

add_executable(smb1
    ${GLAD_SRC}
    src/apu.cpp
    src/cpu.c
    src/main.c
    src/ppu.c
    ${PROJECT_BINARY_DIR}/rom.c
    src/smb1.c
)

if(WIN32)
    # windows
    target_include_directories(smb1 PRIVATE include glfw/include openal-soft/include notsofatso)
    target_link_libraries(smb1 PRIVATE glfw OpenAL notsofatso)
else()
    # unix
    target_include_directories(smb1 PRIVATE include notsofatso)
    target_link_libraries(smb1 PRIVATE m glfw openal notsofatso)
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
