/*
 * sdl3_gfx_compat.cpp - implementations of the SDL2_gfx primitives used by
 * saland (line, rectangle outline, circle outline, ellipse outline).
 *
 * These are simple, non-antialiased implementations on top of SDL3's
 * renderer primitives. They are good enough for the debug overlays and
 * tile-cursor visualisations the game uses.
 */

#include "sdl3_gfx_compat.h"

#include <vector>

namespace {

void apply_state(SDL_Renderer* renderer, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
}

void push_point(std::vector<SDL_FPoint>& v, float x, float y) {
	SDL_FPoint p;
	p.x = x;
	p.y = y;
	v.push_back(p);
}

} // namespace

extern "C" int lineRGBA(SDL_Renderer* renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                        Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	apply_state(renderer, r, g, b, a);
	return SDL_RenderLine(renderer,
	                     static_cast<float>(x1), static_cast<float>(y1),
	                     static_cast<float>(x2), static_cast<float>(y2)) ? 0 : -1;
}

extern "C" int rectangleRGBA(SDL_Renderer* renderer, Sint16 x1, Sint16 y1, Sint16 x2, Sint16 y2,
                             Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	apply_state(renderer, r, g, b, a);
	// SDL2_gfx tolerated swapped corners.
	if (x2 < x1) {
		Sint16 t = x1;
		x1 = x2;
		x2 = t;
	}
	if (y2 < y1) {
		Sint16 t = y1;
		y1 = y2;
		y2 = t;
	}
	SDL_FRect rect;
	rect.x = static_cast<float>(x1);
	rect.y = static_cast<float>(y1);
	rect.w = static_cast<float>(x2 - x1 + 1);
	rect.h = static_cast<float>(y2 - y1 + 1);
	return SDL_RenderRect(renderer, &rect) ? 0 : -1;
}

extern "C" int circleRGBA(SDL_Renderer* renderer, Sint16 cx, Sint16 cy, Sint16 radius,
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	apply_state(renderer, r, g, b, a);
	if (radius < 0) {
		return -1;
	}
	if (radius == 0) {
		SDL_FPoint p = { static_cast<float>(cx), static_cast<float>(cy) };
		return SDL_RenderPoints(renderer, &p, 1) ? 0 : -1;
	}
	// Midpoint circle algorithm.
	std::vector<SDL_FPoint> pts;
	pts.reserve(static_cast<size_t>(radius) * 8 + 8);
	int x = radius;
	int y = 0;
	int err = 1 - x;
	while (x >= y) {
		push_point(pts, cx + x, cy + y);
		push_point(pts, cx + y, cy + x);
		push_point(pts, cx - y, cy + x);
		push_point(pts, cx - x, cy + y);
		push_point(pts, cx - x, cy - y);
		push_point(pts, cx - y, cy - x);
		push_point(pts, cx + y, cy - x);
		push_point(pts, cx + x, cy - y);
		++y;
		if (err < 0) {
			err += 2 * y + 1;
		}
		else {
			--x;
			err += 2 * (y - x) + 1;
		}
	}
	return SDL_RenderPoints(renderer, pts.data(), static_cast<int>(pts.size())) ? 0 : -1;
}

extern "C" int ellipseRGBA(SDL_Renderer* renderer, Sint16 cx, Sint16 cy, Sint16 rx, Sint16 ry,
                           Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	apply_state(renderer, r, g, b, a);
	if (rx < 0 || ry < 0) {
		return -1;
	}
	if (rx == 0 && ry == 0) {
		SDL_FPoint p = { static_cast<float>(cx), static_cast<float>(cy) };
		return SDL_RenderPoints(renderer, &p, 1) ? 0 : -1;
	}
	std::vector<SDL_FPoint> pts;
	pts.reserve(static_cast<size_t>(rx + ry) * 4 + 8);

	long long rx2 = static_cast<long long>(rx) * rx;
	long long ry2 = static_cast<long long>(ry) * ry;
	long long two_rx2 = 2 * rx2;
	long long two_ry2 = 2 * ry2;
	long long p1;
	long long px = 0;
	long long py = two_rx2 * ry;
	int x = 0;
	int y = ry;
	// Region 1
	p1 = static_cast<long long>(ry2 - rx2 * ry + 0.25 * rx2);
	while (px < py) {
		push_point(pts, cx + x, cy + y);
		push_point(pts, cx - x, cy + y);
		push_point(pts, cx + x, cy - y);
		push_point(pts, cx - x, cy - y);
		++x;
		px += two_ry2;
		if (p1 < 0) {
			p1 += ry2 + px;
		}
		else {
			--y;
			py -= two_rx2;
			p1 += ry2 + px - py;
		}
	}
	// Region 2
	long long p2 = static_cast<long long>(
	    ry2 * (x + 0.5) * (x + 0.5) +
	    rx2 * (y - 1) * (y - 1) -
	    rx2 * ry2);
	while (y >= 0) {
		push_point(pts, cx + x, cy + y);
		push_point(pts, cx - x, cy + y);
		push_point(pts, cx + x, cy - y);
		push_point(pts, cx - x, cy - y);
		--y;
		py -= two_rx2;
		if (p2 > 0) {
			p2 += rx2 - py;
		}
		else {
			++x;
			px += two_ry2;
			p2 += rx2 - py + px;
		}
	}
	return SDL_RenderPoints(renderer, pts.data(), static_cast<int>(pts.size())) ? 0 : -1;
}
