#ifndef ENGINE_H_
#define ENGINE_H_

#include <inttypes.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <memory.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>

#define ARENA_WIDTH  10
#define ARENA_HEIGHT 20

#define START_POSITION_X 4
#define START_POSITION_Y 0

#define FAST_DROP_TIME 0.05

#ifdef DEBUG
    #define BASE_TIME .2
#else
    #define BASE_TIME   1.0
#endif
#define TIME_OFFSET .05

#define COORDS_TO_ARENA_INDEX(x, y) (coords_to_array_index((x), (y), ARENA_WIDTH))

enum GameState {
    PAUSE,      // Game paused while in menu
    PLAYING,    // Main gamestate where the pieces are moving
    GAME_OVER   // After losing the game, this gamestate is reached
};

struct GameData {
    enum GameState gameState;

    int* current_piece;             // current piece, no fixed size
    int* next_piece;                // save the next piece for the display
    int* arena;                     // width: 10, height: 20 -> 200 uints

    int position_x;                 // position of the current piece inside the arena
    int position_y;

    bool fast_drop;                 // flag that determines whether the pieces move slow or fast downwards
                                    // enabled when player presses the down button

    uint32_t score;
    uint32_t level;                 // level for piece velocity
    uint32_t cleared_lines;

    int* piece_count;               // saves the number of times the piece have shown up

    double accumulated_time;        // current playing time

    bool is_defeat;                 // detemins wheter the player has lost

    uint32_t seed;                  // for playing a certain game
};

/* List of Pieces:

first number in matrix says which block it is (scales one greater than below)

 O-Block   L-Block     J-Block      T-Block
 { 1 1     { 0 2 0     { 0 3 0      { 0 0 0
   1 1 }     0 2 0       0 3 0        4 4 4
             0 2 2 }     3 3 0 }      0 4 0 }

 I-Block      Z-Block    S-Block
 { 0 0 5 0    { 0 0 0    { 0 0 0
   0 0 5 0      6 6 0      0 7 7
   0 0 5 0      0 6 6 }    7 7 0 }
   0 0 5 0 }
*/

enum Piece {
    PIECE_O, PIECE_L, PIECE_J, PIECE_T, PIECE_I, PIECE_Z, PIECE_S
};

enum Direction {
    LEFT, RIGHT
};

/*
    Initializes an instance of the game_data struct which stores information about the gamestate,
    the arena and meta information of the game.
    An initial seed can be given or the sentinal value 0.
    When 0 is given as the initial_seed, then the current time will become a "pseudo-random" seed.
    If memory couldn't be allocated for the various arrays, the programs exits with ENOMEM.
*/
struct GameData init_gamedata(uint32_t initial_seed);

/*
    Frees the heap allocated arrays of the given instance of the GameData struct.
    The pointer given is not freed itself.
*/
void free_gamedata(struct GameData* game_data);


// Helper funtions for array index conversion: /////////////////////////////////

/*
    Takes an one-dimensional array index and the width of the matrix and calulates
    the 2 dimensional coordinates of the element. The result is saved in the pointers x and y if provided.
*/
void array_index_to_coords(size_t index, size_t width, size_t* x, size_t* y);

/*
    Takes the x and y coordinates of an element of a matrix and its width and
    calculates the corresponding one dimensional array index.
*/
size_t coords_to_array_index(size_t x, size_t y, size_t width);

////////////////////////////////////////////////////////////////////////////////

/*
    Generate the next tetris piece as an heap allocated array of integer values.
    The array consits of the type of the piece on the first position followed by the values
    which have to be interpreted as a 2d matrix.
    When memory couldn't be allocated the program exits with error code ENOMEM.
*/
int* generate_next_piece();

/*
    Rotates a tetris piece clockwise or counter-clockwise depending on the given dir.

    dir == LEFT  => rotate counter-clockwise
    dir == RIGHT => rotate clockwise
*/
void rotate_piece(struct GameData* game_data, enum Direction dir);

/*
    Checks if the current piece collides with the wall of the arena or if out of bounds of the arena.
    Returns true when the collision/out of bounds happens and false if it doesn't.
*/
bool check_collision_side(const struct GameData* game_data);

/*
    Checks if the current piece collides with any other piece currently placed in the arena.
    Returns true if a collision happens and false if it doesn't.
*/
bool check_collision_arena_pices(const struct GameData* game_data);

/*
    Moves the current piece to the right or left according to the given dir.
    If the piece would collide with the arena or other pieces the movement will not happen.
*/
void move(struct GameData* game_data, enum Direction dir);

/*
    Moves the current piece down one step.
    If the piece collides with pieces below it
        * The current piece is written into the arena
        * Filled rows will be removed and score will be added accordingly
        * The next piece will become the current piece and a new next piece is generated
*/
size_t drop(struct GameData* game_data);

/*
    Generates an array which contains all arena pieces and the current piece copied in to the
    correct location. This is used for rendering the pieces.

    The values will be written into the provided pointer. It should have size 200 or the program might crash.
*/
void generate_block_positions(const struct GameData* game_data, int* block_positions);

void check_defeat(struct GameData* game_data);

static inline double calc_drop_time(const struct GameData* game_data)
{
    return BASE_TIME - log((game_data->level + 1)) * TIME_OFFSET;
}

#endif