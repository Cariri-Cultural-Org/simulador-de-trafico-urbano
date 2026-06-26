#ifndef ROAD_H
#define ROAD_H
#include "Cell.h"

typedef struct Intersection Intersection;

typedef enum
{
    ROAD_HORIZONTAL,
    ROAD_VERTICAL,
} RoadDirection;

typedef enum
{
    ROAD_ONE_WAY = 0,
    ROAD_TWO_WAY = 1
    /* Numa via de mão dupla, 'cells' guarda UMA faixa. A faixa oposta é
     * representada por outra instância de Road com a mesma posição mas sentido
     * invertido (o CityMap mantém o par). */
} RoadType;

typedef struct
{
    int id;
    RoadDirection direction;
    RoadType type;

    Cell **cells; // matriz de células
    Intersection **intersections; // vetor de cruzamentos
    int cell_count;
} Road;

Road *road_create(int id, RoadDirection direction, RoadType type, int cell_count);
void road_destroy(Road *road);
Cell *road_get_cell(const Road *road, int index);
Intersection *road_get_intersection(const Road *road, int index);
int road_has_intersection(const Road *road, int index);

#endif
