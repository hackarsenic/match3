cmake_minimum_required(VERSION 3.19.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(TARGET_NAME Match3Game)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
option(USE_ASAN "Use Address Sanitizer" ON)
option(USE_MSAN "Use Memory Sanitizer" OFF)
option(BUILD_SHARED_LIBS "Build SFML as shared library" FALSE)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "install_dir" CACHE PATH "..." FORCE)
endif()

project("${TARGET_NAME}")

# disable sanitizers when releasing executables without explicitly requested debug info
# use generator expressions to set flags correctly in both single and multi config generators
set(debug_mode "$<CONFIG:Debug>")

# custom functions
function(detect_libcpp)
    set(OLD_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(TEST_STDLIB_SRC [=[
    #include <iostream>
    int main() {}
    ]=])
    try_compile(HAS_LIBCPP SOURCE_FROM_CONTENT test_stdlib.cpp ${TEST_STDLIB_SRC})
    set(CMAKE_CXX_FLAGS ${OLD_CMAKE_CXX_FLAGS})
    unset(OLD_CMAKE_CXX_FLAGS)
endfunction()


function(set_custom_stdlib_and_sanitizers target)
    if(MSVC)
        if(USE_ASAN)
            target_compile_options(${target} PRIVATE "$<${debug_mode}:/Zi>")
            target_compile_options(${target} PRIVATE "$<${debug_mode}:/D AERON_SANITIZE_ENABLED>")
            target_compile_options(${target} PRIVATE "$<${debug_mode}:/fsanitize=address>")
        endif()
        return()
    endif()

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND NOT WIN32)
        detect_libcpp()
        if(HAS_LIBCPP)
            target_compile_options(${target} PRIVATE -stdlib=libc++)
            target_link_options(${target} PRIVATE -stdlib=libc++)
        else()
            # fall back to libstdc++
            target_compile_options(${target} PRIVATE -stdlib=libstdc++)
            target_link_options(${target} PRIVATE -stdlib=libstdc++)
        endif()
    endif()

    if(UNIX)
        if(USE_ASAN)
            target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined,leak>")
            target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=address,undefined,leak>")
        elseif(USE_MSAN AND "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            # use semi-colons instead of spaces to separate arguments
            # it is recommended to quote generator expressions in order to avoid unintentional splitting
            target_compile_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=memory,undefined;-fsanitize-recover=memory,undefined;-fsanitize-memory-track-origins>")
            target_link_options(${target} PRIVATE "$<${debug_mode}:-fsanitize=memory,undefined;-fsanitize-recover=memory,undefined;-fsanitize-memory-track-origins;-Wl,-rpath,tools/llvm-project/build/lib>")
        endif()
    endif()
endfunction()

function(set_custom_compile_flags target)
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        # debug build flags (overwrite default cmake debug flags)
        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -ggdb")

        # release build flags (overwrite default cmake release flags)
        set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

        # generic flags
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wshadow -Wpedantic")
    endif()
endfunction()

set(SFML_VERSION 2.6.0)
if(WIN32)
    # external dependencies with FetchContent
    include(FetchContent)

    FetchContent_Declare(
            SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG        ${SFML_VERSION}
            GIT_SHALLOW    1  # works only with branches or tags, not with arbitrary commit hashes
    )

    FetchContent_MakeAvailable(SFML)

    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    else()
        set_custom_stdlib_and_sanitizers(sfml-system)
        set_custom_stdlib_and_sanitizers(sfml-window)
        set_custom_stdlib_and_sanitizers(sfml-graphics)
    endif()
    set(SFML_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
else()
    include(ExternalProject)

    set(SFML_URL "https://github.com/SFML/SFML/releases/download/${SFML_VERSION}/SFML-${SFML_VERSION}-linux-gcc-64-bit.tar.gz")
    set(SFML_DEST "${CMAKE_BINARY_DIR}")

    # ExternalProject to download and extract the tar.gz file
    ExternalProject_Add(
            SFML
            URL ${SFML_URL}
            PREFIX ${SFML_DEST}
            CONFIGURE_COMMAND ""
            BUILD_COMMAND ""
            INSTALL_COMMAND ""
    )

    # Define include and library directories
    ExternalProject_Get_Property(SFML SOURCE_DIR)
    set(SFML_INCLUDE_DIR ${SOURCE_DIR}/include)
    set(SFML_LIBRARY_DIR ${SOURCE_DIR}/lib)

    add_custom_command(TARGET SFML
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SFML_LIBRARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/lib
    )

    # Add the imported libraries
    add_library(SFML_GRAPHICS SHARED IMPORTED)
    set_target_properties(SFML_GRAPHICS PROPERTIES IMPORTED_LOCATION lib/libsfml-graphics.so)

    add_library(SFML_SYSTEM SHARED IMPORTED)
    set_target_properties(SFML_SYSTEM PROPERTIES IMPORTED_LOCATION lib/libsfml-system.so)

    add_library(SFML_WINDOW SHARED IMPORTED)
    set_target_properties(SFML_WINDOW PROPERTIES IMPORTED_LOCATION lib/libsfml-window.so)
endif()

file(COPY resources DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)
file(GLOB srcs src/*.h src/*.cpp src/*.hpp)
add_executable(${TARGET_NAME} ${srcs})
if (UNIX)
    add_dependencies(${TARGET_NAME} SFML)
endif()
target_include_directories(${TARGET_NAME} PRIVATE ${SFML_INCLUDE_DIR})
set_custom_stdlib_and_sanitizers(${TARGET_NAME})
set_custom_compile_flags(${TARGET_NAME})

if(WIN32)
    if(MSVC)
        target_link_libraries(${TARGET_NAME} PRIVATE sfml-main sfml-graphics sfml-window sfml-system)
    else()
        target_link_libraries(${TARGET_NAME} PRIVATE sfml-graphics sfml-window sfml-system)
    endif()
else()
    target_link_libraries(${TARGET_NAME} SFML_GRAPHICS SFML_SYSTEM SFML_WINDOW)
endif()
