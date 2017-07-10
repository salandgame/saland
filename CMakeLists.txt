cmake_minimum_required(VERSION 2.8.9)
project (saland)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${saland_SOURCE_DIR}/src/cmake")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++11 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

find_package(Boost COMPONENTS program_options REQUIRED)

#Setup SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

#Setup things that use pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)
pkg_search_module(SDL2GFX REQUIRED SDL2_gfx)

include_directories("src/Libs/include")

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/Libs/*.c")


add_executable(saland ${SOURCES})
TARGET_LINK_LIBRARIES( saland ${Boost_LIBRARIES} )
target_link_libraries( saland ${SDL2_LIBRARY})
target_link_libraries( saland physfs z b64)
target_link_libraries( saland ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})
