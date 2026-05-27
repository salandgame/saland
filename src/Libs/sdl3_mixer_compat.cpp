/*
 * sdl3_mixer_compat.cpp - implementation of the saland SDL3_mixer
 * compatibility helpers. See sdl3_mixer_compat.h for details.
 */

#include "sdl3_mixer_compat.h"

#include <unordered_map>

MIX_Mixer* g_saland_mixer = nullptr;

namespace {

// Cache one MIX_Track per MIX_Audio we have played. SDL3_mixer requires
// every playback to go through a track; rather than create+destroy a
// track per Mix_PlayChannel call we keep one around per sound. The
// tracks are owned by the mixer and get destroyed automatically when
// the mixer is destroyed at shutdown.
std::unordered_map<MIX_Audio*, MIX_Track*> g_audio_tracks;

MIX_Track* get_track_for(MIX_Audio* audio) {
	if (!g_saland_mixer || !audio) {
		return nullptr;
	}
	auto it = g_audio_tracks.find(audio);
	if (it != g_audio_tracks.end()) {
		return it->second;
	}
	MIX_Track* track = MIX_CreateTrack(g_saland_mixer);
	if (!track) {
		return nullptr;
	}
	if (!MIX_SetTrackAudio(track, audio)) {
		MIX_DestroyTrack(track);
		return nullptr;
	}
	g_audio_tracks.emplace(audio, track);
	return track;
}

} // namespace

extern "C" int Mix_PlayChannel(int /*channel*/, Mix_Chunk* audio, int loops) {
	if (!g_saland_mixer || !audio) {
		return -1;
	}
	MIX_Track* track = get_track_for(audio);
	if (!track) {
		return -1;
	}
	SDL_PropertiesID props = SDL_CreateProperties();
	if (loops != 0) {
		// SDL2_mixer used -1 for infinite, otherwise the count is the
		// number of loops in addition to the initial play. SDL3_mixer's
		// MIX_PROP_PLAY_LOOPS_NUMBER uses the same convention (-1 for
		// infinite, 0 for none, N for N additional plays).
		SDL_SetNumberProperty(props, MIX_PROP_PLAY_LOOPS_NUMBER, loops);
	}
	bool ok = MIX_PlayTrack(track, props);
	SDL_DestroyProperties(props);
	return ok ? 0 : -1;
}

extern "C" void Mix_HaltMusic(void) {
	if (!g_saland_mixer) {
		return;
	}
	for (auto& kv : g_audio_tracks) {
		if (kv.second) {
			MIX_StopTrack(kv.second, 0);
		}
	}
}
