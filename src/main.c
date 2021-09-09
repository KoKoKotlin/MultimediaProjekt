#include <stdio.h>
#include <stdlib.h>

#include "render.h"
#include "engine.h"
#include "audio.h"

#define TETRIS_BACKGROUND_MUSIC "sfx/tetris_bg_music.wav"
#define TETRIS_DEFEAT_SOUND "sfx/defeat.wav"
#define TETRIS_FOR_JEFF "sfx/tetris_for_jeff.wav"

#define NUMBER_OF_AUDIO_FILES 3

void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s (%d)\n", description, error);
    exit(EXIT_FAILURE);
}

void framebuffer_size_callback(GLFWwindow* window, int fb_width, int fb_height)
{
    glViewport(0, 0, fb_width, fb_height);
    gl_check_error("glViewport");
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
    user_data_t* user_data = glfwGetWindowUserPointer(window);

    user_data->window_width = width;
    user_data->window_height = height;
}

// eventhandler for the keyboard
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	user_data_t* user_data = glfwGetWindowUserPointer(window);

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_A)          move(&user_data->gameData, LEFT);
        else if (key == GLFW_KEY_D)     move(&user_data->gameData, RIGHT);
        else if (key == GLFW_KEY_RIGHT) rotate_piece(&user_data->gameData, RIGHT);
        else if (key == GLFW_KEY_LEFT)  rotate_piece(&user_data->gameData, LEFT);
        else if (key == GLFW_KEY_S)     user_data->gameData.fast_drop = true;
        else if (key == GLFW_KEY_P)     {
            if (user_data->gameData.gameState == GAME_OVER) return;

            user_data->gameData.gameState = (user_data->gameData.gameState == PLAYING) ? PAUSE : PLAYING;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_S)     user_data->gameData.fast_drop = false;
    }
}

void load_audio_files(struct WavData** data)
{
    data[0] = load_wav_file(TETRIS_BACKGROUND_MUSIC);
    data[1] = load_wav_file(TETRIS_DEFEAT_SOUND);
    data[2] = load_wav_file(TETRIS_FOR_JEFF);
}

void free_audio_files(struct WavData** data)
{
    for (size_t i = 0; i < NUMBER_OF_AUDIO_FILES; i++) {
        free_wav_file(data[i]);
    }
}

int main(void)
{
    init_tetris_audio();
    struct WavData** wav_data = calloc(NUMBER_OF_AUDIO_FILES, sizeof(struct WavData*));
    load_audio_files(wav_data);

    SDL_AudioDeviceID deviceId = open_audio_device(wav_data[0]);
    SDL_AudioDeviceID deviceId2 = open_audio_device(wav_data[1]);

    // Create our user data struct:
    user_data_t user_data =
    {
        .window_width = 800,
        .window_height = 600,

        .background_device = deviceId,
        .effect_device = deviceId2,
        .wav_data = wav_data,
    };

    // Specify our error callback func:
    // Then initialize GLFW itself.
    printf("Initializing GLFW ...\n");

    glfwSetErrorCallback(error_callback);
    check_error(glfwInit(), "Failed to initialize GLFW.");

    // We want at least OpenGL 4.1:
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Enable forward-compatibility and use the core profile:
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window:
    printf("Creating window ...\n");

    GLFWwindow* window = glfwCreateWindow(user_data.window_width, user_data.window_height, "Tetris", NULL, NULL);
    check_error(window != NULL, "Failed to create window.");

    // Make the OpenGL context of the window the current one:
    glfwMakeContextCurrent(window);

    // Loader function:
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Try to swap on every screen update:
    glfwSwapInterval(1);

    // Specify remaining callbacks:
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Store a pointer to our user data inside the window:
    glfwSetWindowUserPointer(window, &user_data);

    // Initialize everything related to OpenGL:
    init_gl(window);

    while (!glfwWindowShouldClose(window))
    {
        // Update the model:
        update_gl(window);

        // Draw the next frame:
        draw_gl(window);

        // Swap the buffers to avoid tearing:
        glfwSwapBuffers(window);

        // React to the window manager's messages (e.g. close button):
        glfwPollEvents();
    }

    // Deinitialize the OpenGL stuff:
    teardown_gl(window);

    // Destroy the window:
    glfwDestroyWindow(window);

    // Terminate GLFW:
    glfwTerminate();

    free_audio_files(wav_data);
    close_audio_device(deviceId);
    close_audio_device(deviceId2);

    return 0;
}
