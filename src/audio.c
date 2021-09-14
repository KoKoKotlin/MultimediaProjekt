#include "audio.h"

void init_tetris_audio()
{
    check_error(SDL_Init(SDL_INIT_AUDIO) >= 0, "SDL_Init(SDL_INIT_AUDIO) < 0\n");
}

struct WavData* load_wav_file(const char* path)
{
    struct WavData* wav_data = calloc(1, sizeof(struct WavData));

    /* Load the WAV */
    check_error(SDL_LoadWAV(path, &wav_data->wav_spec, &wav_data->wav_buffer, &wav_data->wav_length) != NULL, "Could not open wav file!");
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
    
    check_error(deviceId != 0, "Couldn't obtain audio device!");
    
    SDL_PauseAudioDevice(deviceId, 0);

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
    check_error(success >= 0, "Failed to queue audio!\n");
}

void queue_audio_if_empty(SDL_AudioDeviceID deviceId, struct WavData* wav_data)
{
    if (SDL_GetQueuedAudioSize(deviceId) == 0)
        queue_audio(deviceId, wav_data);
}