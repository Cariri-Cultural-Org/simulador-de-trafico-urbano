#ifndef CELL_H
#define CELL_H

#include "pthread_compat.h"

// Forward declaration de Vehicle.
struct Vehicle;

typedef struct
{
    int row;
    int column;
    int occupied;
    char occupant_symbol;
    struct Vehicle *vehicle;
    pthread_mutex_t mutex;
} Cell;

void cell_init(Cell *cell, int row, int column);
void cell_destroy(Cell *cell);
int cell_try_occupy(Cell *cell, struct Vehicle *vehicle);
int cell_try_occupy_with_symbol(Cell *cell, struct Vehicle *vehicle, char symbol);
void cell_release(Cell *cell);

#endif
