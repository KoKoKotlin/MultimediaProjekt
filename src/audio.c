#include "audio.h"

#ifdef SOUND

void init_tetris_audio()
{
    check_error(SDL_Init(SDL_INIT_AUDIO) >= 0, "SDL_Init(SDL_INIT_AUDIO) < 0\n");
}

void teardown_tetris_audio()
{
    SDL_Quit();
}

struct WavData* load_wav_file(const char* path)
{
    struct WavData* wav_data = calloc(1, sizeof(struct WavData));

    // load the wave and check for errors
    if (SDL_LoadWAV(path, &wav_data->wav_spec, &wav_data->wav_buffer, &wav_data->wav_length) != NULL) {
        return wav_data;
    }

    // if the wav couldn't be loaded (maybe SDL2 uninitilized) => free the memory and return NULL
    free(wav_data);
    return NULL;
}

void free_wav_file(struct WavData* wav_data)
{
    if (wav_data == NULL) return;

    SDL_FreeWAV(wav_data->wav_buffer);
    free(wav_data);
}

SDL_AudioDeviceID open_audio_device(const struct WavData* data)
{
    check_error_non_closing(data != NULL, "WavData object was NULL when trying to create a new device!");
    if (data == NULL) return 0;

    SDL_AudioSpec desired;
    SDL_AudioSpec obtained;

    SDL_zero(desired);
    desired.freq = data->wav_spec.freq;
    desired.format = data->wav_spec.format;
    desired.channels = 2;
    desired.samples = 4096;
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    
    check_error_non_closing(deviceId != 0, "Couldn't obtain audio device!");
    if (deviceId != 0) SDL_PauseAudioDevice(deviceId, 0);

    return deviceId;
}

void close_audio_device(SDL_AudioDeviceID deviceId)
{
    SDL_CloseAudioDevice(deviceId);
}

void unpause(SDL_AudioDeviceID deviceId)
{
    SDL_PauseAudioDevice(deviceId, 0);
}

void pause(SDL_AudioDeviceID deviceId)
{
    SDL_PauseAudioDevice(deviceId, 1);
}

void queue_audio(SDL_AudioDeviceID deviceId, const struct WavData* data)
{
    check_error_non_closing(data != NULL, "WavData object was null when trying to queue it!");
    if (data == NULL) return;
    int success = SDL_QueueAudio(deviceId, data->wav_buffer, data->wav_length);
    check_error_non_closing(success >= 0, "Failed to queue audio!\n");
}

void queue_audio_if_empty(SDL_AudioDeviceID deviceId, struct WavData* wav_data)
{
    if (SDL_GetQueuedAudioSize(deviceId) == 0)
        queue_audio(deviceId, wav_data);
}

#else

#pragma GCC diagnostic ignored "-Wunused-parameter"

void init_tetris_audio()
{}

void teardown_tetris_audio();
{}

struct WavData* load_wav_file(const char* path)
{ return NULL; }

void free_wav_file(struct WavData* wav_data)
{}

SDL_AudioDeviceID open_audio_device(const struct WavData* data)
{ return 0; }

void close_audio_device(SDL_AudioDeviceID deviceId)
{}

void pause(SDL_AudioDeviceID deviceId)
{}

void unpause(SDL_AudioDeviceID deviceId)
{}

void queue_audio(SDL_AudioDeviceID deviceId, const struct WavData* data)
{}

void queue_audio_if_empty(SDL_AudioDeviceID deviceId, struct WavData* wav_data)
{}

#endif