#ifndef AUDIO_H_
#define AUDIO_H_

#include <stdbool.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#include "user_data.h"
#include "error.h"

struct WavData {
    SDL_AudioSpec wav_spec;
    Uint32 wav_length;
    Uint8* wav_buffer;
};

void init_tetris_audio();
struct WavData* load_wav_file(const char* path);
void free_wav_file(struct WavData* wav_data);

SDL_AudioDeviceID open_audio_device(const struct WavData* data);
void close_audio_device(SDL_AudioDeviceID deviceId);

void queue_audio(SDL_AudioDeviceID deviceId, const struct WavData* data);

void play(SDL_AudioDeviceID deviceId, const struct WavData* data, bool loop);
void pause(SDL_AudioDeviceID deviceId);
void unpause(SDL_AudioDeviceID deviceId);

void queue_audio_if_empty(SDL_AudioDeviceID deviceId, struct WavData* wav_data);

#endif