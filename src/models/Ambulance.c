#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Ambulance.h"
#include "GlobalClock.h"
#include "CityMap.h"
#include "Intersection.h"

#ifdef _WIN32
DWORD WINAPI thread_ambulance(LPVOID arg)
#else
void *thread_ambulance(void *arg)
#endif
{
    Vehicle *ambulance = (Vehicle *)arg;
    CityMap *city_map = (CityMap *)ambulance->city_map;

    // Tenta ocupar a célula inicial
    Cell *start_cell = road_get_cell(ambulance->current_road, ambulance->road_cell_index);
    if (start_cell != NULL)
    {
        while (!cell_try_occupy(start_cell, ambulance) && simulation_running && ambulance->active)
        {
            wait_next_tick(global_tick);
        }
        ambulance->row = start_cell->row;
        ambulance->column = start_cell->column;
    }

    int last_tick = global_tick;

    while (simulation_running && ambulance->active)
    {
        // Aguarda a sincronização do relógio global (sem busy-wait)
        wait_next_tick(last_tick);
        last_tick = global_tick;

        if (!simulation_running || !ambulance->active)
        {
            break;
        }

        // A ambulância move-se a cada tick (velocidade = 1)
        Road *road = ambulance->current_road;
        int index = ambulance->road_cell_index;

        // Determina a próxima célula
        int next_index = index + 1;
        if (next_index >= road->cell_count)
        {
            next_index = 0; // Wrap around
        }

        Cell *next_cell = road_get_cell(road, next_index);
        if (next_cell == NULL)
        {
            continue;
        }

        // Verifica se a próxima célula coincide com um cruzamento
        bool next_is_intersection = road_has_intersection(road, next_index);
        Intersection *intersection = NULL;

        if (next_is_intersection)
        {
            intersection = city_map_get_intersection(city_map, next_cell->row, next_cell->column);
            if (intersection != NULL)
            {
                // Respeita o semáforo do cruzamento: aguarda o sinal verde normalmente
                // NOTA: A prioridade da ambulância será integrada na próxima etapa (Semana 2)
                pthread_mutex_lock(&intersection->mutex);
                intersection_wait_green(intersection, road->direction);
                pthread_mutex_unlock(&intersection->mutex);
            }
        }

        // Tenta ocupar a próxima célula
        if (cell_try_occupy(next_cell, ambulance))
        {
            // Ocupou com sucesso! Libera a célula anterior
            Cell *current_cell = road_get_cell(road, index);
            if (current_cell != NULL)
            {
                cell_release(current_cell);
            }

            // Atualiza posição da ambulância
            ambulance->road_cell_index = next_index;
            ambulance->row = next_cell->row;
            ambulance->column = next_cell->column;

            // Se a nova célula ocupada é um cruzamento, decide se vai virar
            if (next_is_intersection && intersection != NULL)
            {
                // Decisão de curva com probabilidade (ex: 30%)
                if ((rand() % 100) < 30)
                {
                    Road *new_road = (road->direction == ROAD_HORIZONTAL)
                                         ? intersection->vertical_road
                                         : intersection->horizontal_road;
                    if (new_road != NULL)
                    {
                        int new_index = (new_road->direction == ROAD_HORIZONTAL)
                                            ? next_cell->column
                                            : next_cell->row;
                        if (new_index >= 0 && new_index < new_road->cell_count)
                        {
                            ambulance->current_road = new_road;
                            ambulance->road_cell_index = new_index;
                        }
                    }
                }
            }
        }
    }

    // Libera a célula atual ao encerrar
    Cell *current_cell = road_get_cell(ambulance->current_road, ambulance->road_cell_index);
    if (current_cell != NULL)
    {
        cell_release(current_cell);
    }

    return 0;
}
