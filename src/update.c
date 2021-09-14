#include "update.h"

void update_gl(GLFWwindow* window)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    // Calculate the frame delta time and update the timestamp:
    double frame_time = glfwGetTime();
    double delta_time = frame_time - user_data->last_frame_time;
    if (user_data->gameData.gameState == PLAYING) {
        // accumulate delta time
        user_data->time_since_last_drop += delta_time;

        if ((!user_data->gameData.fast_drop && (user_data->time_since_last_drop >= calc_drop_time(&user_data->gameData))) ||
            (user_data->gameData.fast_drop && user_data->time_since_last_drop >= FAST_DROP_TIME)) {
            size_t cleared_rows = drop(&user_data->gameData);

            // FIXME: maybe bugged => doesn't play if 4 lines cleared
            if (cleared_rows == 4) queue_audio_if_empty(user_data->effect_device, user_data->wav_data[2]);
            user_data->time_since_last_drop = 0.0;

            if (user_data->gameData.is_defeat) {
                queue_audio_if_empty(user_data->effect_device, user_data->wav_data[1]);
                user_data->gameData.gameState = GAME_OVER;
            }
        }
    }
    user_data->last_frame_time = frame_time;

    SDL_PauseAudioDevice(user_data->background_device, (user_data->gameData.gameState == PAUSE) ? 1 : 0);
}