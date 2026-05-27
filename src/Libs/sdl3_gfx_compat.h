/*
 * sdl3_gfx_compat - small drop-in replacements for the few SDL2_gfx
 * primitives used by saland. There is no official SDL3 port of SDL2_gfx,
 * so we implement these directly on top of SDL3's renderer primitives.
 *
 * Only the four primitives that saland actually used are exposed here:
 *   lineRGBA, rectangleRGBA, circleRGBA, ellipseRGBA
 *
 * They preserve the original SDL2_gfx signatures (int return value, Sint16
 * coordinates, Uint8 colors) so call sites do not have to change.
 *
 * A small helper for converting integer SDL_Rect to float SDL_FRect is also
 * exposed because SDL3's renderer APIs take SDL_FRect.
 */

#ifndef SALAND_SDL3_GFX_COMPAT_H
#define SALAND_SDL3_GFX_COMPAT_H

#include <SDL3/SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

int lineRGBA(SDL_Renderer* renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
             Uint8 r, Uint8 g, Uint8 b, Uint8 a);

int rectangleRGBA(SDL_Renderer* renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                  Uint8 r, Uint8 g, Uint8 b, Uint8 a);

int circleRGBA(SDL_Renderer* renderer, Sint16 cx, Sint16 cy, Sint16 radius,
               Uint8 r, Uint8 g, Uint8 b, Uint8 a);

int ellipseRGBA(SDL_Renderer* renderer, Sint16 cx, Sint16 cy, Sint16 rx, Sint16 ry,
                Uint8 r, Uint8 g, Uint8 b, Uint8 a);

#ifdef __cplusplus
}

// Conversion helper used at the SDL3 boundary: most of saland uses integer
// SDL_Rect for its logical coordinate system, but SDL3's renderer APIs take
// SDL_FRect (floats).
static inline SDL_FRect toFRect(const SDL_Rect& r) {
	SDL_FRect out;
	out.x = static_cast<float>(r.x);
	out.y = static_cast<float>(r.y);
	out.w = static_cast<float>(r.w);
	out.h = static_cast<float>(r.h);
	return out;
}

#endif

#endif /* SALAND_SDL3_GFX_COMPAT_H */
