#include <stdlib.h>
#include "Road.h"
#include "Intersection.h"

Road *road_create(int id, RoadDirection direction, RoadType type, int cell_count)
{
    Road *road = malloc(sizeof(Road));
    if (!road)
        return NULL;

    road->id = id;
    road->direction = direction;
    road->type = type;
    road->cell_count = cell_count;

    // inicializa vertor sem lixo de memória
    road->cells = calloc(cell_count, sizeof(Cell *));
    if (!road->cells)
    {
        free(road);
        return NULL;
    }

    road->intersections = calloc(cell_count, sizeof(Intersection *));
    if (!road->intersections)
    {
        free(road->cells); // também libera as células antes de destruir a via
        free(road);
        return NULL;
    }

    return road;
}

void road_destroy(Road *road)
{
    if (!road)
        return;

    free(road->intersections);
    free(road->cells);
    free(road);
}

Cell *road_get_cell(const Road *road, int index)
{
    if (!road || index < 0 || index >= road->cell_count)
        return NULL;
    return road->cells[index];
}

/*
- Retorna o `Intersection` se o indice na via coincide com um cruzamento, NULL caso contrário.
- Usado pela thread do veículo para saber se deve verificar o semáforo antes de avançar.
*/
Intersection *road_get_intersection(const Road *road, int index)
{
    if (!road || index < 0 || index >= road->cell_count)
        return NULL;

    return road->intersections[index];
}

int road_has_intersection(const Road *road, int index)
{
    return road_get_intersection(road, index) != NULL;
}
