cmake_minimum_required(VERSION 3.5.0)
project (saland)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${saland_SOURCE_DIR}/src/cmake")

# -Wno-deprecated-declarations is needed until all supported platforms are using PhysFS 3.0.1+.
# This will happen once Debian 10 is realeased
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsuggest-override -Wno-deprecated-declarations -std=c++11 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

if (WIN32)
      SET(GUI_TYPE WIN32)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
	if(NOT _WIN32_WINNT AND NOT WINVER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 -DWINVER=0x0601")
	endif()
endif()

find_package(Boost COMPONENTS program_options REQUIRED)

add_subdirectory("embedded_libs/box2d-2.4.1" EXCLUDE_FROM_ALL)

#Setup SDL2
find_package(SDL2 REQUIRED CONFIG)
include_directories(${SDL2_INCLUDE_DIRS})

#Setup things that use pkg-config
find_package(PkgConfig REQUIRED)
pkg_search_module(SDL2MIXER REQUIRED SDL2_mixer)
include_directories(${SDL2MIXER_INCLUDE_DIRS})
pkg_search_module(SDL2IMAGE REQUIRED SDL2_image)
include_directories(${SDL2IMAGE_INCLUDE_DIRS})
pkg_search_module(SDL2TTF REQUIRED SDL2_ttf)
include_directories(${SDL2TTF_INCLUDE_DIRS})
pkg_search_module(SDL2GFX REQUIRED SDL2_gfx)
include_directories(${SDL2GFX_INCLUDE_DIRS})


include_directories(SYSTEM "src/Libs/include")

file(GLOB SOURCES "src/*.cpp" "src/*/*.cpp" "src/*/*/*.cpp" "src/Libs/*.c")

add_executable(saland ${SOURCES})
TARGET_LINK_LIBRARIES( saland ${Boost_LIBRARIES} )
target_link_libraries( saland ${SDL2_LIBRARIES})
target_link_libraries( saland physfs z box2d)
target_link_libraries( saland ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})
