#include <stdlib.h>
#include "Cell.h"

void cell_init(Cell *cell, int row, int column)
{
    cell->row = row;
    cell->column = column;
    cell->occupied = 0;
    cell->occupant_symbol = ' ';
    cell->vehicle = NULL;
    pthread_mutex_init(&cell->mutex, NULL);
}

void cell_destroy(Cell *cell)
{
    if (!cell)
        return;

    pthread_mutex_destroy(&cell->mutex);
}

int cell_try_occupy(Cell *cell, struct Vehicle *vehicle)
{
    return cell_try_occupy_with_symbol(cell, vehicle, 'C');
}

int cell_try_occupy_with_symbol(Cell *cell, struct Vehicle *vehicle, char symbol)
{
    int success = 0;

    pthread_mutex_lock(&cell->mutex);

    if (!cell->occupied)
    {
        cell->occupied = 1;
        cell->occupant_symbol = symbol ? symbol : 'C';
        cell->vehicle = vehicle;
        success = 1;
    }

    pthread_mutex_unlock(&cell->mutex);

    return success;
}

void cell_release(Cell *cell)
{
    pthread_mutex_lock(&cell->mutex);

    cell->occupied = 0;
    cell->occupant_symbol = ' ';
    cell->vehicle = NULL;

    pthread_mutex_unlock(&cell->mutex);
}
