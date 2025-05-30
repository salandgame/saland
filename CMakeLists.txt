cmake_minimum_required(VERSION 3.10.2)
project (saland)

# Define install paths
set(INSTALL_BIN_DIR "bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "share/salandgame" CACHE STRING "Install data to this directory")
set(INSTALL_MAN_DIR "share/man/man6" CACHE STRING "Install the man page to this directory")
set(INSTALL_APPLICATIONS_DIR "share/applications" CACHE STRING "Install the .desktop file to this directory")
set(INSTALL_ICONS_DIR "share/icons/hicolor" CACHE STRING "Install the icon in a subfolder in this directory")
set(INSTALL_LOCALE_DIR "${INSTALL_DATA_DIR}/locale/" CACHE STRING "Install translation to this dir")
set(INSTALL_METAINFO_DIR "share/metainfo" CACHE STRING "Install appstream matedata to this directory")

option(SALAND_USE_EMBEDDED_FMT "Use the embedded version of {fmt}" ON)
option(SALAND_USE_EMBEDDED_PLATFORM_FOLDERS "Use the embedded version of PlatformFolders" ON)

if (NOT WIN32 AND NOT STANDALONE)
	#The path to the data dir must be compiled into the binary
	if (IS_ABSOLUTE ${INSTALL_DATA_DIR})
		add_definitions(-DSHAREDIR=\"${INSTALL_DATA_DIR}\")
	else()
		add_definitions(-DSHAREDIR=\"${CMAKE_INSTALL_PREFIX}/${INSTALL_DATA_DIR}\")
	endif()
	if (IS_ABSOLUTE ${INSTALL_DATA_DIR})
		add_definitions(-DLOCALEDIR=\"${INSTALL_LOCALE_DIR}\")
	else()
		add_definitions(-DLOCALEDIR=\"${CMAKE_INSTALL_PREFIX}/${INSTALL_LOCALE_DIR}\")
	endif()
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsuggest-override -std=c++17 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

if (WIN32)
	SET(GUI_TYPE WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
	if(NOT _WIN32_WINNT AND NOT WINVER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 -DWINVER=0x0601")
	endif()
endif()

find_package(Boost COMPONENTS program_options REQUIRED)

# box2d is not API compatible between versions (not even minor versions), so this explicit version must always be used.
add_subdirectory("embedded_libs/box2d-2.4.1" EXCLUDE_FROM_ALL)

if (SALAND_USE_EMBEDDED_FMT)
	message("Using embedded {fmt}")
	add_subdirectory("embedded_libs/fmt-8.1.1" EXCLUDE_FROM_ALL)
	message("Done processing {fmt}")
endif()

if (SALAND_USE_EMBEDDED_PLATFORM_FOLDERS)
	message("Using embedded PlatformFolders")
	add_subdirectory("embedded_libs/PlatformFolders-4.1.0" EXCLUDE_FROM_ALL)
	message("Done processing PlatformFolders")
endif()

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

include_directories(SYSTEM "src/Libs/imgui")

file(GLOB SOURCES "src/*.cpp" "src/*.h*" "src/*/*.cpp" "src/*/*.h*" "src/*/*/*.cpp" "src/*/*/*.h*" "src/*/*/*/*.cpp" "src/Libs/*.c*" "src/Libs/*.h")

add_executable(saland ${SOURCES})
TARGET_LINK_LIBRARIES( saland ${Boost_LIBRARIES} )
target_link_libraries( saland ${SDL2_LIBRARIES})
target_link_libraries( saland physfs z box2d fmt platform_folders)
target_link_libraries( saland ${SDL2MIXER_LIBRARIES} ${SDL2IMAGE_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2GFX_LIBRARIES})
