cmake_minimum_required(VERSION 3.6)

set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_BINARY_DIR}/DevTestGame-master/SFML")
set (PROJECT_NAME, M3Game)
set (TARGET_NAME Match3Game)

project("${TARGET_NAME}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
configure_file(src/resources.h.in ${CMAKE_CURRENT_BINARY_DIR}/src/resources.h @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/src)

find_package(SFML 2 COMPONENTS graphics window system)
include_directories(${SFML_INCLUDE_DIR})

if(${SFML_FOUND}) 
    file(GLOB srcs src/*.h src/*.cpp src/*.hpp)
    add_executable(${TARGET_NAME} ${srcs})
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET_NAME})

    target_link_libraries(${TARGET_NAME} ${SFML_LIBRARIES})
else() 
    message(SEND_ERROR "Could not find SFML library. Please refer to: https://www.sfml-dev.org/")
    if(WIN32)
	    message("You should consider adding SFML library path to 'PATH' environment variable")
	endif()
endif()