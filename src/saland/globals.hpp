#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#include "../Libs/NFont.h"
#include "../sago/SagoSpriteHolder.hpp"
#include <memory>

struct GlobalData {
	NFont nf_standard_font;
	bool isShuttingDown = false;
	SDL_Renderer* screen = nullptr;
	std::unique_ptr<sago::SagoSpriteHolder> spriteHolder;
};

extern GlobalData globalData;

#endif /* GLOBALS_HPP */

