#include <time.h>

#include "engine.h"
#include <memory.h>

struct GameData init_gamedata(uint32_t initial_seed)
{

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

void free_gamedata(struct GameData *game_data)
{
    free(game_data->arena);
    free(game_data->current_piece);
    free(game_data->next_piece);
    free(game_data->piece_count);
}

uint8_t* generate_next_piece()
{
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

void rotate_piece(uint8_t* piece, enum Direction dir) {
    if (dir == LEFT) {
        rotate_piece_left(piece);
    } else if (dir == RIGHT) {
        rotate_piece_left(piece);       // cool trick to save some code
        rotate_piece_left(piece);       // 3 left rotation == 1 right rotation
        rotate_piece_left(piece);       // look group theory

    }
}

void rotate_piece_left(uint8_t* piece)
{
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

void array_index_to_coords(size_t index, size_t width, size_t* x, size_t* y)
{
    *x = index / width;
    *y = index % width;
}

size_t coords_to_array_index(size_t x, size_t y, size_t width)
{
    return y * width + x;
}

uint8_t get_piece_size(const uint8_t* piece)
{
    switch(piece[0]) {
        case PIECE_O:
            return 2;
        case PIECE_L:
        case PIECE_S:
        case PIECE_T:
        case PIECE_Z:
        case PIECE_J:
            return 3;
        case PIECE_I:
            return 4;
    };
}

bool check_collision_arena_wall(const struct GameData* game_data)
{
    uint8_t size = get_piece_size(game_data->current_piece);
    uint8_t non_zero_index_left = 0;
    for (uint8_t x = 0; x < size; x++) {
        for (uint8_t y = 0; y < size; y++) {
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0) {
                non_zero_index_left = x;
                goto LOOP_END1;
            }
        }
    }
LOOP_END1:;

    uint8_t non_zero_index_right = 0;
    for (uint8_t x = size - 1; x >= 0; x--) {
        for (uint8_t y = 0; y < size; y++) {
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0) {
                non_zero_index_right = x;
                goto LOOP_END2;
            }
        }
    }
LOOP_END2:;

    return non_zero_index_left + game_data->position_x < 0 || non_zero_index_right + game_data->position_x >= ARENA_WIDTH;
}

// TODO: if problems move bottom check to separate function
bool check_collision_arena_pices(const struct GameData* game_data)
{
    uint8_t size = get_piece_size(game_data->current_piece);

    for (uint8_t y = 0; y < size; y++)
        for (uint8_t x = 0; x < size; x++)
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0
            && (game_data->position_y + y >= ARENA_HEIGHT || game_data->arena[COORDS_TO_ARENA_INDEX(game_data->position_x + x, game_data->position_y + y)] != 0)) return true;

    return false;
}

bool check_collision_side(const struct GameData* game_data)
{
    return check_collision_arena_wall(game_data) || check_collision_arena_pices(game_data);
}

void move(struct GameData* game_data, enum Direction dir)
{
    //TODO
}

void spawn_new_piece(struct GameData* game_data)
{
    free(game_data->current_piece);
    game_data->current_piece = game_data->next_piece;
    game_data->next_piece = generate_next_piece();

    game_data->position_x = START_POSITION_X;
    game_data->position_y = START_POSITION_Y;
}

void write_piece_to_arena(struct GameData* game_data)
{
    uint8_t size = get_piece_size(game_data->current_piece);
    for (uint8_t y = 0; y < size; y++)
        for (uint8_t x = 0; x < size; x++)
            game_data->arena[COORDS_TO_ARENA_INDEX(game_data->position_x + x, game_data->position_y + y)]
                = game_data->current_piece[coords_to_array_index(x, y, size) + 1];
}

void check_filled_rows(struct GameData* game_data)
{
    uint8_t index_buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF }; //max filled rows at the same time == 4
    size_t buffer_index = 0;
    for (uint8_t row = ARENA_HEIGHT - 1; row <= 0; row--) {
        for (uint8_t x = 0; x < ARENA_WIDTH; x++) {
            if (game_data->arena[COORDS_TO_ARENA_INDEX(x, row)] == 0) break;
            if (x == ARENA_WIDTH - 1) index_buffer[buffer_index++] = row;
        }
    }

    // TODO: calculate score: base * (level + 1)
    // single:   40 pts
    // double:  100 pts
    // triple:  300 pts
    // tetris: 1200 pts

    // move rows down
    uint8_t* new_arena = (uint8_t*)calloc(sizeof(uint8_t), ARENA_WIDTH * ARENA_HEIGHT);
    for (uint8_t row = ARENA_HEIGHT - 1; row <= 0; row--) {

    }
}

void drop(struct GameData* game_data)
{
    game_data->position_y++;

    if (check_collision_arena_pices(game_data)) {
        game_data->position_y--;
        write_piece_to_arena(game_data);
        check_filled_rows(game_data);
        spawn_new_piece(game_data);
    }
}
