#include <time.h>

#include "engine.h"

struct GameData init_gamedata(uint32_t initial_seed) {

    uint8_t arena = (uint8_t*)calloc(sizeof(uint8_t), 10 * 20);
    uint32_t piece_count = (uint32_t)calloc(sizeof(uint32_t), 7);

    struct GameData gameData = {
        .gameState = PLAYING,
        .current_piece = NULL,
        .next_piece = NULL,
        .arena = arena,
        .position_x = 5,
        .position_y = 0,
        .fast_drop = 0,
        .score = 0,
        .level = 0,
        .piece_count = piece_count,
        .accumulated_time = 0.0,
        .seed = (initial_seed == 0) ? time(NULL) : initial_seed,    // <--- trailing comma from rust
    };

    srand(gameData.seed);

    return gameData;
}

void free_gamedata(struct GameData *game_data) {
    free(game_data->arena);
    free(game_data->current_piece);
    free(game_data->next_piece);
    free(game_data->piece_count);
}

uint8_t* generate_next_piece() {
    uint8_t *new_piece;

    switch ((uint32_t)random() % 7)
    {
        case PIECE_O:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 4);

            uint8_t piece[] = {PIECE_O, 1, 1, 1, 1};
            *new_piece = piece;
            break;

        case PIECE_L:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 9);

            uint8_t piece[] = {PIECE_L, 0, 1, 0, 0, 1, 0, 0, 1, 1};
            *new_piece = piece;
            break;

        case PIECE_J:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 9);

            uint8_t piece[] = {PIECE_J, 0, 1, 0, 0, 1, 0, 1, 1, 0};
            *new_piece = piece;
            break;

        case PIECE_T:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 9);

            uint8_t piece[] = {PIECE_T, 0, 0, 0, 1, 1, 1, 0, 1, 0};
            *new_piece = piece;
            break;

        case PIECE_I:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 16);

            uint8_t piece[] = {PIECE_I, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0};
            *new_piece = piece;
            break;

        case PIECE_Z:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 9);

            uint8_t piece[] = {PIECE_Z, 0, 0, 0, 1, 1, 0, 0, 1, 1};
            *new_piece = piece;
            break;

        case PIECE_S:
            new_piece = (uint8_t*)calloc(sizeof(uint8_t), 1 + 9);

            uint8_t piece[] = {PIECE_T, 0, 0, 0, 0, 1, 1, 1, 1, 0};
            *new_piece = piece;
            break;
    }

    return new_piece;
}

uint8_t* rotate_piece(uint8_t* piece, enum Direction dir) {
    if (piece[0] == PIECE_O) return;

    size_t size;
    if (piece[0] == PIECE_I) size = 4;
    else size = 3;

    // index == y * w + x
    uint8_t* buffer = (uint8_t*)calloc(sizeof(uint8_t), 1 + size * size);
    buffer[0] = piece[0];
    for (size_t j = 0; j < size; j++) {
        for (size_t i = 0; i < size; i++) {
            // transpose a_ij => a_ji
            // rev rows  a_ij => a_(size-1-i)j
            // together  a_ij => a_(size-1-j)i
            size_t from_index = coords_to_array_index(i, j, size) + 1;
            size_t to_index   = coords_to_array_index(size - 1 -j, i, size) + 1;
            buffer[to_index] = piece[from_index];
        }
    }

    free(piece);
    piece = buffer;
}

void array_index_to_coords(size_t index, size_t width, size_t* x, size_t* y) {
    *x = index / width;
    *y = index % width;
}

size_t coords_to_array_index(size_t x, size_t y, size_t width) {
    return y * width + x;
}