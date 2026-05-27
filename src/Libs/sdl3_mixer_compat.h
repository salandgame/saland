/*
 * sdl3_mixer_compat - thin compatibility layer for the few SDL2_mixer
 * style helpers that saland uses, implemented on top of the redesigned
 * SDL3_mixer 3.2+ API (MIX_Mixer / MIX_Audio / MIX_Track).
 *
 * SDL3_mixer dropped the distinction between Mix_Music and Mix_Chunk in
 * favour of a single MIX_Audio object. It also replaced the implicit
 * global mixer/channels with explicit MIX_Mixer and MIX_Track objects.
 *
 * To keep the call sites in saland tidy, this header defines Mix_Music
 * and Mix_Chunk as aliases for MIX_Audio, and provides small inline
 * wrappers that take their MIX_Mixer pointer from a single global
 * variable (`g_saland_mixer`) instead of from every call site.
 *
 * The compatibility wrappers only cover the subset of the original
 * SDL2_mixer API that saland still calls.
 */

#ifndef SALAND_SDL3_MIXER_COMPAT_H
#define SALAND_SDL3_MIXER_COMPAT_H

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#ifdef __cplusplus
extern "C" {
#endif

/* The lifetime of this mixer is managed by saland.cpp::runGame(). It is
 * created with MIX_CreateMixerDevice() right after MIX_Init() and
 * destroyed before MIX_Quit().
 *
 * If audio fails to initialise the pointer stays NULL and the helper
 * functions become no-ops.
 */
extern MIX_Mixer* g_saland_mixer;

typedef MIX_Audio Mix_Music;
typedef MIX_Audio Mix_Chunk;

static inline void Mix_FreeMusic(Mix_Music* m) {
	if (m) {
		MIX_DestroyAudio(m);
	}
}

static inline void Mix_FreeChunk(Mix_Chunk* m) {
	if (m) {
		MIX_DestroyAudio(m);
	}
}

static inline Mix_Music* Mix_LoadMUS_IO(SDL_IOStream* rw, bool freerw) {
	if (!g_saland_mixer) {
		if (freerw && rw) {
			SDL_CloseIO(rw);
		}
		return NULL;
	}
	return MIX_LoadAudio_IO(g_saland_mixer, rw, false /* predecode */, freerw);
}

static inline Mix_Chunk* Mix_LoadWAV_IO(SDL_IOStream* rw, bool freerw) {
	if (!g_saland_mixer) {
		if (freerw && rw) {
			SDL_CloseIO(rw);
		}
		return NULL;
	}
	return MIX_LoadAudio_IO(g_saland_mixer, rw, true /* predecode */, freerw);
}

/*
 * Play `audio` once. Channel argument is kept for source compatibility
 * but ignored: SDL3_mixer no longer uses channels. The returned int is
 * 0 on success and -1 on failure, matching the SDL2_mixer semantics
 * closely enough for the (boolean-style) call sites in saland.
 */
int Mix_PlayChannel(int channel, Mix_Chunk* audio, int loops);

/*
 * Halt music playback. SDL3_mixer does not have a singleton music
 * concept any more; this implementation stops every track owned by the
 * saland mixer.
 */
void Mix_HaltMusic(void);

#ifdef __cplusplus
}
#endif

#endif /* SALAND_SDL3_MIXER_COMPAT_H */
