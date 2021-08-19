#ifndef ENGINE_H_
#define ENGINE_H_

#include <inttypes.h>
#include <stdlib.h>


enum GameState {
    PAUSE, PLAYING, GAME_OVER
};

struct GameData {
    enum GameState gameState;

    uint8_t *current_piece;           // current piece, no fixed size
    uint8_t *next_piece;              // save the next piece for the display
    uint8_t *arena;                   // width: 10, height: 20 -> 200 uints

    uint8_t position_x;               // position of the current piece inside the arena
    uint8_t position_y;

    uint8_t fast_drop;                // flag that determines whether the pieces move slow or fast downwards
                                      // enabled when player presses the down button

    uint32_t score;
    uint32_t level;                   // level for piece velocity

    uint32_t *piece_count;            // saves the number of times the piece have shown up

    double accumulated_time;          // current playing time

    uint32_t seed;                         // for playing a certain game
};

// List of Pieces:
//
// first number in matrix says which block it is (scales one greater than below)
//
//  O-Block   L-Block     J-Block      T-Block
//  { 1 1     { 0 2 0     { 0 3 0      { 0 0 0
//    1 1 }     0 2 0       0 3 0        4 4 4
//              0 2 2 }     3 3 0 }      0 4 0 }
//
//  I-Block      Z-Block    S-Block
//  { 0 0 5 0    { 0 0 0    { 0 0 0
//    0 0 5 0      6 6 0      0 7 7
//    0 0 5 0      0 6 6 }    7 7 0 }
//    0 0 5 0 }
//

enum Piece {
    PIECE_O, PIECE_L, PIECE_J, PIECE_T, PIECE_I, PIECE_Z, PIECE_S
};

enum Direction {
    LEFT, RIGHT
};

struct GameData init_gamedata(uint32_t);
void free_gamedata(struct GameData*);

void array_index_to_coords(size_t, size_t, size_t*);
size_t coords_to_array_index(size_t, size_t, size_t);

uint8_t* generate_next_piece();
uint8_t* rotate_piece(uint8_t*, enum Direction);

uint8_t check_collision(struct GameData*);
void move(struct GameData*);
void drop(struct GameData*);

#endif