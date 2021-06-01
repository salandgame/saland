/*
===========================================================================
 * Saland Adventures
Copyright (C) 2014-2018 Poul Sander

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/

Source information and contacts persons can be found at
https://github.com/sago007/saland
===========================================================================
*/

#include "os.hpp"
#include <iostream>
#include <physfs.h>
#include "sago/platform_folders.h"
#include "version.h"

static sago::PlatformFolders pf;


static std::string overrideSavePath = "";

/**
 * Returns the path to where all settings must be saved.
 * On unix-like systems this is the home-folder under: ~/.local/share/GAMENAME
 * In Windows it is My Documents/My Games
 * Consider changing this for Vista that has a special save games folder
 */
std::string getPathToSaveFiles() {
	if (overrideSavePath.length() > 0) {
		return overrideSavePath;
	}
	return pf.getSaveGamesFolder1()+"/"+GAMENAME;
}

void setPathToSaveFiles(const std::string& path) {
	overrideSavePath = path;
}

bool OsPathIsRelative(const std::string& path) {
#if defined(_WIN32)
	return PathIsRelativeW(win32_utf8_to_utf16(path.c_str()).c_str());
#else
	return path[0] != '/';
#endif
}

void OsCreateFolder(const std::string& path) {
#if defined(__unix__)
	std::string cmd = "mkdir -p '"+path+"/'";
	int retcode = system(cmd.c_str());
	if (retcode != 0) {
		std::cerr << "Failed to create: " << path+"/" << "\n";
	}
#elif defined(_WIN32)
	//Now for Windows NT/2k/xp/2k3 etc.
	CreateDirectoryW(win32_utf8_to_utf16(pf.getSaveGamesFolder1().c_str()).c_str(), nullptr);
	std::string tempA = path;
	CreateDirectoryW(win32_utf8_to_utf16(tempA.c_str()).c_str(), nullptr);
#else
	std::cerr << "Failed to create: \"" << path << "\"\n";
#endif
}

void OsCreateSaveFolder() {
#if defined(__unix__)
	std::string cmd = "mkdir -p '"+getPathToSaveFiles()+"/'";
	int retcode = system(cmd.c_str());
	if (retcode != 0) {
		std::cerr << "Failed to create: " << getPathToSaveFiles()+"/" << "\n";
	}
#elif defined(_WIN32)
	//Now for Windows NT/2k/xp/2k3 etc.
	CreateDirectory(pf.getSaveGamesFolder1().c_str(), nullptr);
	std::string tempA = getPathToSaveFiles();
	CreateDirectory(tempA.c_str(),nullptr);
#endif
}
