#include "platform/audio/music.h"
#include "platform/path.h"

#include <cstdio>
#include <cstring>

// ~3 seconds of headroom at 22050 Hz mono 8-bit — comfortably absorbs a
// slow disk read or a frame hitch without an audible dropout.
#define RING_BYTES    (22050 * 3)
#define REFILL_CHUNK  (22050 / 4)   // read ~0.25s per musicRefill() call

static FILE*             s_file = 0;
static signed char       s_ring[RING_BYTES];
static volatile unsigned s_writePos = 0;  // producer: main thread (musicRefill)
static volatile unsigned s_readPos  = 0;  // consumer: audio thread (musicMixInto)
static volatile int      s_playing  = 0;
static float             s_volume   = 0.45f;  // ambient — sits under sound effects
static int               s_upSample = 0;      // 2x sample-and-hold: 22050 -> 44100
static int               s_lastMixed = 0;

static unsigned ringUsed(void) {
    return (s_writePos - s_readPos) % RING_BYTES;
}

void musicSetVolume(float volume) {
    s_volume = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
}

void musicPlay(const char* path) {
    musicStop();

    FILE* f = fopen(assetPath(path), "rb");
    if (!f) f = fopen(path, "rb");
    if (!f) return;

    s_file      = f;
    s_writePos  = 0;
    s_readPos   = 0;
    s_upSample  = 0;
    s_lastMixed = 0;
    s_playing   = 1;
}

void musicStop(void) {
    s_playing = 0;
    if (s_file) { fclose(s_file); s_file = 0; }
}

void musicRefill(void) {
    if (!s_playing || !s_file) return;

    unsigned used = ringUsed();
    unsigned free = RING_BYTES - used - 1;  // keep 1 byte gap (full/empty disambiguation)
    unsigned want = free < REFILL_CHUNK ? free : (unsigned)REFILL_CHUNK;
    if (!want) return;

    unsigned wp = s_writePos % RING_BYTES;
    unsigned untilWrap = RING_BYTES - wp;
    unsigned first = want < untilWrap ? want : untilWrap;

    size_t got = fread(s_ring + wp, 1, first, s_file);
    unsigned total = (unsigned)got;

    if (got == first && want > first) {
        size_t got2 = fread(s_ring, 1, want - first, s_file);
        total += (unsigned)got2;
    }

    s_writePos += total;

    if (feof(s_file)) rewind(s_file);  // loop back to the start
}

void musicMixInto(int* mix, int count) {
    if (!s_playing) return;

    int volFixed = (int)(s_volume * 4096.0f);

    for (int i = 0; i < count; i++) {
        if (!s_upSample) {
            if (ringUsed() == 0) break;  // underrun: leave the rest of the block silent
            unsigned rp = s_readPos % RING_BYTES;
            signed char sample = s_ring[rp];
            s_readPos++;
            s_lastMixed = ((int)sample << 8) * volFixed >> 12;
            s_upSample = 1;
        } else {
            s_upSample = 0;
        }
        mix[i] += s_lastMixed;
    }
}
