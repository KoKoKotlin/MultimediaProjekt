#include "helper.h"

void print_piece(const int* piece, int size) {

    for(int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            printf("%d ", piece[x + y * size + 1]);
        }

        printf("\n");
    }

    printf("\n");
}
