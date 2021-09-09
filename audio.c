
#include "audio.h"

void init_tetris_audio()
{
    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init(SDL_INIT_AUDIO) < 0\n");
        exit(EXIT_FAILURE);
    }
}

struct WavData* load_wav_file(const char* path)
{
    struct WavData* wav_data = calloc(1, sizeof(struct WavData));

    /* Load the WAV */
    if (SDL_LoadWAV(path, &wav_data->wav_spec, &wav_data->wav_buffer, &wav_data->wav_length) == NULL) {
        fprintf(stderr, "Could not open test.wav: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return wav_data;
}

void free_wav_file(struct WavData* wav_data)
{
    SDL_FreeWAV(wav_data->wav_buffer);
    free(wav_data);
}

SDL_AudioDeviceID open_audio_device(const struct WavData* data)
{
    SDL_AudioSpec desired;
    SDL_AudioSpec obtained;

    SDL_zero(desired);
    desired.freq = data->wav_spec.freq;
    desired.format = data->wav_spec.format;
    desired.channels = 2;
    desired.samples = 4096;
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);

    if (deviceId) SDL_PauseAudioDevice(deviceId, 0);
    else fprintf(stderr, "Couldn't obtain audio device! %s\n", SDL_GetError());

    return deviceId;
}

void close_audio_device(SDL_AudioDeviceID deviceId)
{
    SDL_CloseAudioDevice(deviceId);
}

void pause(SDL_AudioDeviceID deviceId)
{
    SDL_PauseAudioDevice(deviceId, 1);
}

void queue_audio(SDL_AudioDeviceID deviceId, const struct WavData* data)
{
    int success = SDL_QueueAudio(deviceId, data->wav_buffer, data->wav_length);
    if (success < 0)
        fprintf(stderr, "Failed to queue audio! %s\n", SDL_GetError());
}