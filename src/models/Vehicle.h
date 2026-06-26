#ifndef VEHICLE_H
#define VEHICLE_H

#include <pthread.h>
#include "Road.h"

typedef struct Vehicle
{
    int id;
    int velocity;       /* ticks por movimento: 1 = rápido, 2 = médio, 4 = lento */
    Road *current_road;
    int road_cell_index; /* índice da célula atual em current_road->cells */
    int row;
    int column;

    pthread_t thread;
    int active;          /* 1 = rodando, 0 = finalizado */
    void *city_map;      /* ponteiro para o CityMap */
} Vehicle;

/* Inicializa um veículo com os parâmetros básicos */
void vehicle_init(Vehicle *vehicle, int id, int velocity, Road *road, int road_cell_index, void *city_map);

#endif /* VEHICLE_H */
