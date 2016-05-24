cmake_minimum_required(VERSION 2.8)

project(HelloSDL)

#generates compilations database for vim-youcompleteme
set( CMAKE_EXPORT_COMPILE_COMMANDS 1 )

set( SOURCES
   main.cpp
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake )

message( "CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}" )
message( "CMAKE_SOURCE_DIR=${CMAKE_SOURCE_DIR}" )
message( "CMAKE_BINARY_DIR=${CMAKE_BINARY_DIR}" )

find_package( SDL2 REQUIRED )
find_package( SDL2_image REQUIRED )
include_directories( ${SDL2_INCLUDE_DIR} )
include_directories( ${SDL2_IMAGE_INCLUDE_DIR} )

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(
   ${PROJECT_NAME}
   ${SDL2_LIBRARY}
   ${SDL2_IMAGE_LIBRARY}
) 
