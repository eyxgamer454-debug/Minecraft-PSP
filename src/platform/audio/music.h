#ifndef MCPSP_PLATFORM_AUDIO_MUSIC_H
#define MCPSP_PLATFORM_AUDIO_MUSIC_H

// Background music, streamed from disk (not loaded whole into RAM — the
// track is far too long for that). 8-bit signed mono PCM @ 22050 Hz only,
// matching the engine's existing sound-effect format.

void musicPlay(const char* path);   // starts (or restarts) looping playback
void musicStop(void);

// Call once per frame from the MAIN thread. Tops up the ring buffer from
// disk a little at a time, and loops back to the start of the file at EOF.
void musicRefill(void);

void musicSetVolume(float volume);

// Audio-thread side: mixes buffered samples into the mixer's accumulation
// buffer. Called only from soundMixBlock(). Never blocks — if the ring
// buffer has underrun (refill fell behind), it just contributes silence
// for the rest of the block rather than stalling the audio thread.
void musicMixInto(int* mix, int count);

#endif
