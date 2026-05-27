cmake_minimum_required(VERSION 3.16..99.0)
project (saland)

# Define install paths
set(INSTALL_BIN_DIR "bin" CACHE STRING "Install executable in this directory")
set(INSTALL_DATA_DIR "share/salandgame" CACHE STRING "Install data to this directory")
set(INSTALL_MAN_DIR "share/man/man6" CACHE STRING "Install the man page to this directory")
set(INSTALL_APPLICATIONS_DIR "share/applications" CACHE STRING "Install the .desktop file to this directory")
set(INSTALL_ICONS_DIR "share/icons/hicolor" CACHE STRING "Install the icon in a subfolder in this directory")
set(INSTALL_LOCALE_DIR "${INSTALL_DATA_DIR}/locale/" CACHE STRING "Install translation to this dir")
set(INSTALL_METAINFO_DIR "share/metainfo" CACHE STRING "Install appstream matedata to this directory")

option(SALAND_USE_EMBEDDED_PLATFORM_FOLDERS "Use the embedded version of PlatformFolders" ON)
option(SALAND_USE_SYSTEM_SDL3 "Use system-provided SDL3/SDL3_image/SDL3_mixer/SDL3_ttf via find_package instead of building them from source" OFF)

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsuggest-override -std=c++20 -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

if (WIN32)
	SET(GUI_TYPE WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=i686")
	if(NOT _WIN32_WINNT AND NOT WINVER)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0601 -DWINVER=0x0601")
	endif()
endif()

if(POLICY CMP0167)
  cmake_policy(SET CMP0167 NEW)
endif()
find_package(Boost COMPONENTS program_options REQUIRED)

# box2d is not API compatible between versions (not even minor versions), so this explicit version must always be used.
add_subdirectory("embedded_libs/box2d-2.4.1" EXCLUDE_FROM_ALL)


if (SALAND_USE_EMBEDDED_PLATFORM_FOLDERS)
	message("Using embedded PlatformFolders")
	add_subdirectory("embedded_libs/PlatformFolders-4.1.0" EXCLUDE_FROM_ALL)
	message("Done processing PlatformFolders")
endif()

# ---------------------------------------------------------------------------
# SDL3 + SDL3_image + SDL3_mixer + SDL3_ttf
# ---------------------------------------------------------------------------
# Two modes are supported:
#  * SALAND_USE_SYSTEM_SDL3=ON  -> use find_package(SDL3 …) on the host. Requires
#    that SDL3, SDL3_image, SDL3_mixer and SDL3_ttf are installed system-wide.
#    Useful on machines that already provide SDL3 (or for offline builds where
#    the developer pre-installed the libraries).
#  * SALAND_USE_SYSTEM_SDL3=OFF (default) -> the libraries are built from the
#    upstream tarballs. The build first looks for the tarballs in
#    ${CMAKE_SOURCE_DIR}/embedded_libs/downloads/ (offline mode); if a file is
#    missing it falls back to downloading from the upstream GitHub release.
# ---------------------------------------------------------------------------

if (SALAND_USE_SYSTEM_SDL3)
	find_package(SDL3 CONFIG REQUIRED)
	find_package(SDL3_image CONFIG REQUIRED)
	find_package(SDL3_mixer CONFIG REQUIRED)
	find_package(SDL3_ttf CONFIG REQUIRED)
else()
	include(FetchContent)

	set(_saland_sdl3_download_dir "${CMAKE_SOURCE_DIR}/embedded_libs/downloads")

	# Helper: pick a local file:// URL if the tarball is staged in
	# embedded_libs/downloads/, otherwise fall back to the upstream URL.
	function(_saland_pick_sdl_url out_var file_name upstream_url)
		set(_local "${_saland_sdl3_download_dir}/${file_name}")
		if (EXISTS "${_local}")
			set(${out_var} "${_local}" PARENT_SCOPE)
			message(STATUS "Using local ${file_name} from ${_local}")
		else()
			set(${out_var} "${upstream_url}" PARENT_SCOPE)
			message(STATUS "Will download ${file_name} from ${upstream_url}")
		endif()
	endfunction()

	# Build all SDL3 components as static libraries linked into saland directly.
	set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
	set(SDL_SHARED OFF CACHE BOOL "" FORCE)
	set(SDL_STATIC ON CACHE BOOL "" FORCE)
	set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
	set(SDL_TESTS OFF CACHE BOOL "" FORCE)
	set(SDL_INSTALL OFF CACHE BOOL "" FORCE)

	# SDL3_image: use system libpng/jpeg (the release tarball ships only the
	# external/download.sh helper, not the vendored sources themselves).
	set(SDLIMAGE_INSTALL OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_SAMPLES OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_TESTS OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_VENDORED OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_AVIF OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_TIF OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_WEBP OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_JXL OFF CACHE BOOL "" FORCE)
	set(SDLIMAGE_PNG ON CACHE BOOL "" FORCE)
	set(SDLIMAGE_JPG ON CACHE BOOL "" FORCE)
	set(SDLIMAGE_BMP ON CACHE BOOL "" FORCE)

	# SDL3_mixer: use system libogg/libvorbis.
	set(SDLMIXER_INSTALL OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_SAMPLES OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_VENDORED OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_OPUS OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_FLAC OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_MOD OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_MIDI OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_WAVPACK OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_GME OFF CACHE BOOL "" FORCE)
	set(SDLMIXER_VORBIS_VORBISFILE ON CACHE BOOL "" FORCE)
	set(SDLMIXER_WAVE ON CACHE BOOL "" FORCE)

	# SDL3_ttf: use system freetype.
	set(SDLTTF_INSTALL OFF CACHE BOOL "" FORCE)
	set(SDLTTF_SAMPLES OFF CACHE BOOL "" FORCE)
	set(SDLTTF_VENDORED OFF CACHE BOOL "" FORCE)
	set(SDLTTF_HARFBUZZ OFF CACHE BOOL "" FORCE)
	set(SDLTTF_PLUTOSVG OFF CACHE BOOL "" FORCE)

	_saland_pick_sdl_url(_sdl3_url
		"SDL3-3.4.8.tar.gz"
		"https://github.com/libsdl-org/SDL/releases/download/release-3.4.8/SDL3-3.4.8.tar.gz")
	FetchContent_Declare(SDL3
		URL "${_sdl3_url}"
	)

	_saland_pick_sdl_url(_sdl3_image_url
		"SDL3_image-3.4.4.tar.gz"
		"https://github.com/libsdl-org/SDL_image/releases/download/release-3.4.4/SDL3_image-3.4.4.tar.gz")
	FetchContent_Declare(SDL3_image
		URL "${_sdl3_image_url}"
	)

	_saland_pick_sdl_url(_sdl3_mixer_url
		"SDL3_mixer-3.2.2.tar.gz"
		"https://github.com/libsdl-org/SDL_mixer/releases/download/release-3.2.2/SDL3_mixer-3.2.2.tar.gz")
	FetchContent_Declare(SDL3_mixer
		URL "${_sdl3_mixer_url}"
	)

	_saland_pick_sdl_url(_sdl3_ttf_url
		"SDL3_ttf-3.2.2.tar.gz"
		"https://github.com/libsdl-org/SDL_ttf/releases/download/release-3.2.2/SDL3_ttf-3.2.2.tar.gz")
	FetchContent_Declare(SDL3_ttf
		URL "${_sdl3_ttf_url}"
	)

	# SDL3 must be available first because the other components depend on it.
	FetchContent_MakeAvailable(SDL3)
	FetchContent_MakeAvailable(SDL3_image)
	FetchContent_MakeAvailable(SDL3_mixer)
	FetchContent_MakeAvailable(SDL3_ttf)
endif()


include_directories(SYSTEM "src/Libs/include")

include_directories(SYSTEM "src/Libs/imgui")

file(GLOB SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h*" "src/*/*.cpp" "src/*/*.h*" "src/*/*/*.cpp" "src/*/*/*.h*" "src/*/*/*/*.cpp" "src/Libs/*.c*" "src/Libs/*.h")

add_executable(saland ${SOURCES})
target_link_libraries(saland ${Boost_LIBRARIES})
target_link_libraries(saland physfs z box2d platform_folders)
target_link_libraries(saland
	SDL3::SDL3
	SDL3_image::SDL3_image
	SDL3_mixer::SDL3_mixer
	SDL3_ttf::SDL3_ttf
)
