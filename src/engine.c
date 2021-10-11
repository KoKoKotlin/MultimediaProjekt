#include "engine.h"

/*
    Helper funtions for getting the width of the matrix of a piece depending on its shape.
*/
int get_piece_size(const int* piece)
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
        default:
            dprintf(2, "Piece with unknown piece ID: %d!\n", piece[0]);
            exit(-1);
    };
}

void align_y(struct GameData* game_data) {
    int size = get_piece_size(game_data->current_piece);
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0) return;
        }
        game_data->position_y = game_data->position_y - 1;
    }
}

void align_x(struct GameData* game_data) {
    int size = get_piece_size(game_data->current_piece);
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0) return;
        }
        game_data->position_x = game_data->position_x - 1;
    }
}

struct GameData init_gamedata(uint32_t initial_seed)
{
    int* arena = (int*)calloc(sizeof(int), 10 * 20);
    int* piece_count = (int*)calloc(sizeof(int), 7);

    if (arena == NULL || piece_count == NULL) {
        dprintf(2, "Couldn't allocate memory for the arena or the piece counter! Exiting...");
        exit(ENOMEM);
    }

    struct GameData gameData = {
        .gameState = PLAYING,
        .current_piece = NULL,
        .next_piece = NULL,
        .arena = arena,
        .position_x = START_POSITION_X,
        .position_y = START_POSITION_Y,
        .fast_drop = false,
        .score = 0,
        .level = 0,
        .piece_count = piece_count,
        .accumulated_time = 0.0,
        .is_defeat = false,
        .seed = (initial_seed == 0) ? time(NULL) : initial_seed,    // <--- trailing comma from rust
    };

    srand(gameData.seed);

    gameData.next_piece = generate_next_piece();
    spawn_new_piece(&gameData);

    return gameData;
}

void free_gamedata(struct GameData *game_data)
{
    free(game_data->arena);
    if (game_data->current_piece != NULL) free(game_data->current_piece);
    if (game_data->next_piece != NULL) free(game_data->next_piece);
    free(game_data->piece_count);
}

int* generate_next_piece()
{
    int* new_piece;

    switch (((uint32_t)(rand())) % 7)
    {
        case PIECE_O: {
            new_piece = (int*)calloc(1 + 4, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_O;
            new_piece[1] = 1;
            new_piece[2] = 1;
            new_piece[3] = 1;
            new_piece[4] = 1;

            break;
        }

        case PIECE_L: {
            new_piece = (int*)calloc(1 + 9, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_L;
            new_piece[1] = 0;
            new_piece[2] = 2;
            new_piece[3] = 0;
            new_piece[4] = 0;
            new_piece[5] = 2;
            new_piece[6] = 0;
            new_piece[7] = 0;
            new_piece[8] = 2;
            new_piece[9] = 2;

            break;
        }

        case PIECE_J: {
            new_piece = (int*)calloc(1 + 9, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_J;
            new_piece[1] = 0;
            new_piece[2] = 3;
            new_piece[3] = 0;
            new_piece[4] = 0;
            new_piece[5] = 3;
            new_piece[6] = 0;
            new_piece[7] = 3;
            new_piece[8] = 3;
            new_piece[9] = 0;

            break;
        }

        case PIECE_T: {
            new_piece = (int*)calloc(1 + 9, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_T;
            new_piece[1] = 0;
            new_piece[2] = 0;
            new_piece[3] = 0;
            new_piece[4] = 4;
            new_piece[5] = 4;
            new_piece[6] = 4;
            new_piece[7] = 0;
            new_piece[8] = 4;
            new_piece[9] = 0;

            break;
        }

        case PIECE_I: {
            new_piece = (int*)calloc(1 + 16, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_I;
            new_piece[1] = 0;
            new_piece[2] = 0;
            new_piece[3] = 5;
            new_piece[4] = 0;
            new_piece[5] = 0;
            new_piece[6] = 0;
            new_piece[7] = 5;
            new_piece[8] = 0;
            new_piece[9] = 0;
            new_piece[10] = 0;
            new_piece[11] = 5;
            new_piece[12] = 0;
            new_piece[13] = 0;
            new_piece[14] = 0;
            new_piece[15] = 5;
            new_piece[16] = 0;

            break;
        }

        case PIECE_Z: {
            new_piece = (int*)calloc(1 + 9, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_Z;
            new_piece[1] = 0;
            new_piece[2] = 0;
            new_piece[3] = 0;
            new_piece[4] = 6;
            new_piece[5] = 6;
            new_piece[6] = 0;
            new_piece[7] = 0;
            new_piece[8] = 6;
            new_piece[9] = 6;
            break;
        }

        case PIECE_S: {
            new_piece = (int*)calloc(1 + 9, sizeof(int));
            if (new_piece == NULL) goto ERROR_GEN_PIECE;

            new_piece[0] = PIECE_S;
            new_piece[1] = 0;
            new_piece[2] = 0;
            new_piece[3] = 0;
            new_piece[4] = 0;
            new_piece[5] = 7;
            new_piece[6] = 7;
            new_piece[7] = 7;
            new_piece[8] = 7;
            new_piece[9] = 0;
            break;
        }

        default:
            exit(-1);
    }

    return new_piece;

ERROR_GEN_PIECE:
    dprintf(2, "Couldn't allocate memory for the next tetris piece! Exiting...");
    exit(ENOMEM);
}

void array_index_to_coords(size_t index, size_t width, size_t* x, size_t* y)
{
    if (x == NULL || y == NULL) return;

    *x = index / width;
    *y = index % width;
}

size_t coords_to_array_index(size_t x, size_t y, size_t width)
{ 
    return y * width + x; 
}

bool check_collision_arena_wall(const struct GameData* game_data)
{
    // find the left most non zero entry in the matrix
    int size = get_piece_size(game_data->current_piece);
    int non_zero_index_left = 0;
    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0) {
                non_zero_index_left = x;
                goto LOOP_END1;
            }
        }
    }
LOOP_END1:;

    // find the right most non zero entry in the matrix
    int non_zero_index_right = 0;
    for (int x = size - 1; x >= 0; x--) {
        for (int y = 0; y < size; y++) {
            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0) {
                non_zero_index_right = x;
                goto LOOP_END2;
            }
        }
    }
LOOP_END2:;

    // check if the left most and right most entries are in bounds of the arena
    return non_zero_index_left + game_data->position_x < 0 || non_zero_index_right + game_data->position_x >= ARENA_WIDTH;
}

// TODO: if problems move bottom check to separate function
bool check_collision_arena_pieces(const struct GameData* game_data)
{
    int size = get_piece_size(game_data->current_piece);

    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            if (x + game_data->position_x >= ARENA_WIDTH) continue;

            if (game_data->current_piece[coords_to_array_index(x, y, size) + 1] != 0
            && (game_data->position_y + y >= ARENA_HEIGHT || game_data->arena[COORDS_TO_ARENA_INDEX(game_data->position_x + x, game_data->position_y + y)] != 0)) return true;
        }
    }
    return false;
}

bool check_collision_side(const struct GameData* game_data)
{
    // combine checks for piece and arena collision when moving side-to-side
    return check_collision_arena_wall(game_data) || check_collision_arena_pieces(game_data);
}

/*
    Helper function for rotation a piece right once.
    This is done by first transposing the matrix of the piece and then reversing the rows.
*/
void rotate_piece_right(int** piece)
{
    // the O piece does not change when rotated by 90 degrees
    if ((*piece)[0] == PIECE_O) return;

    size_t size = get_piece_size(*piece);

    // buffer for the rotated piece
    int* buffer = (int*)calloc(sizeof(int), 1 + size * size);

    buffer[0] = (*piece)[0];
    for (size_t j = 0; j < size; j++) {
        for (size_t i = 0; i < size; i++) {
            // transpose a_ij => a_ji
            // rev rows  a_ij => a_(size-1-i)j
            // together  a_ij => a_(size-1-j)i
            size_t from_index = coords_to_array_index(i, j, size) + 1;
            size_t to_index   = coords_to_array_index(size - 1 -j, i, size) + 1;
            buffer[to_index] = (*piece)[from_index];
        }
    }

    free(*piece);
    *piece = buffer;
}

void rotate_piece_left(int** piece) {
    rotate_piece_right(piece);       // cool trick to save some code
    rotate_piece_right(piece);       // 3 right rotations == 1 left rotation
    rotate_piece_right(piece);       // look group theory
}

void rotate_piece(struct GameData* game_data, enum Direction dir) {
    if (dir == RIGHT)     rotate_piece_right(&game_data->current_piece);
    else if (dir == LEFT) rotate_piece_left(&game_data->current_piece);

    // if a collision occurs rotate back
    if (check_collision_arena_pieces(game_data) || check_collision_arena_wall(game_data)) {
        if (dir == LEFT)     rotate_piece_right(&game_data->current_piece);
        else if (dir == RIGHT) rotate_piece_left(&game_data->current_piece);
    }
}

void spawn_new_piece(struct GameData* game_data)
{
    if (game_data->current_piece != NULL) free(game_data->current_piece);
    game_data->current_piece = game_data->next_piece;
    game_data->next_piece = generate_next_piece();

    game_data->position_x = START_POSITION_X;
    game_data->position_y = START_POSITION_Y;

    align_y(game_data);
    align_x(game_data);

    game_data->piece_count[game_data->current_piece[0]]++;

    if (check_collision_arena_pieces(game_data)) game_data->is_defeat = true;
}

/*
    Helper functions that writes the current piece to the correct spot into the arena.
*/
void write_piece_to_arena(struct GameData* game_data)
{
    int size = get_piece_size(game_data->current_piece);
    for (int y = 0; y < size; y++) {
        if (y + game_data->position_y >= ARENA_HEIGHT) continue;
        for (int x = 0; x < size; x++) {
            if (x + game_data->position_x >= ARENA_WIDTH) continue;
            game_data->arena[COORDS_TO_ARENA_INDEX(game_data->position_x + x, game_data->position_y + y)]
                += game_data->current_piece[coords_to_array_index(x, y, size) + 1];
        }
    }
}

/*
    Helper functions that checks for filled rows, deletes them and adds to the score
    depending on the number of simultanious rows cleared and the current level.

    returns the number of cleared lines
*/
size_t check_filled_rows(struct GameData* game_data)
{
    // find the indecies of filled rows (which can be at most 4) from bottom to top
    int row_buffer[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
    size_t buffer_index = 0;
    for (int row = ARENA_HEIGHT - 1; row >= 0; row--) {
        for (int x = 0; x < ARENA_WIDTH; x++) {
            if (game_data->arena[COORDS_TO_ARENA_INDEX(x, row)] == 0) break;
            if (x == ARENA_WIDTH - 1) row_buffer[buffer_index++] = row;
        }
    }

    // single:   40 pts
    // double:  100 pts
    // triple:  300 pts
    // tetris: 1200 pts
    switch (buffer_index) {
        case 0: return 0; // no rows cleared
        case 1: game_data->score += 40 * (game_data->level + 1);
                break;
        case 2: game_data->score += 100 * (game_data->level + 1);
                break;
        case 3: game_data->score += 300 * (game_data->level + 1);
                break;
        case 4: game_data->score += 1200 * (game_data->level + 1);
                break;
    }

    // copy the not cleared rows into an arena buffer skipping the cleared ones
    int* new_arena = (int*)calloc(sizeof(int), ARENA_WIDTH * ARENA_HEIGHT);
    size_t current_row_index = ARENA_HEIGHT - 1;
    buffer_index = 0;
    for (int row = ARENA_HEIGHT - 1; row >= 0; row--) {
        if (row == row_buffer[buffer_index]) {
            buffer_index++;
            continue;
        }

        memcpy(new_arena + ARENA_WIDTH * current_row_index, game_data->arena + ARENA_WIDTH * row, sizeof(int) * ARENA_WIDTH);
        current_row_index--;
    }

    // swap the buffers and free the old arena
    free(game_data->arena);
    game_data->arena = new_arena;

    return buffer_index;
}

void move(struct GameData* game_data, enum Direction dir)
{
    game_data->position_x += (dir == LEFT) ? -1 : 1;
    if (check_collision_side(game_data)) game_data->position_x -= (dir == LEFT) ? -1 : 1;
}

void level_up(struct GameData* game_data)
{
    game_data->level = game_data->cleared_lines / 10;
}

size_t drop(struct GameData* game_data)
{
    game_data->position_y++;
    size_t rows = 0;

    if (check_collision_arena_pieces(game_data)) {
        game_data->position_y--;

        write_piece_to_arena(game_data);
        rows = check_filled_rows(game_data);
        game_data->cleared_lines += rows;
        spawn_new_piece(game_data);
        level_up(game_data);
    }

    return rows;
}

void generate_block_positions(const struct GameData* game_data, int* block_positions)
{
    if (block_positions == NULL) return;

    // copy in the arena pieces
    memcpy(block_positions, game_data->arena, sizeof(int) * ARENA_WIDTH * ARENA_HEIGHT);

    int size = get_piece_size(game_data->current_piece);

    // place the current piece into the buffer at the correct location
    // this has to be done with the plus operation so that no other pieces are
    // overwritten by the zeros in the piece matrix
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++) {
            block_positions[COORDS_TO_ARENA_INDEX(game_data->position_x + x, game_data->position_y + y)] +=
                game_data->current_piece[coords_to_array_index(x, y, size) + 1];
        }
}
