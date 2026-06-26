#include "TrafficLight.h"

void init_traffic_light(TrafficLight *traffic_light, int id, int green_time, int red_time)
{
    traffic_light->id = id;
    traffic_light->state = RED; // Inicia fechado por padrão
    traffic_light->green_time = green_time;
    traffic_light->red_time = red_time;
    traffic_light->current_tick = 0;

#ifdef _WIN32
    InitializeCriticalSection(&traffic_light->mutex);
#else
    pthread_mutex_init(&traffic_light->mutex, NULL);
#endif
}

void update_traffic_light(TrafficLight *traffic_light)
{
#ifdef _WIN32
    EnterCriticalSection(&traffic_light->mutex);
#else
    pthread_mutex_lock(&traffic_light->mutex);
#endif

    traffic_light->current_tick++;

    // Alterna o semáforo com base no tempo (ticks)
    if (traffic_light->state == RED && traffic_light->current_tick >= traffic_light->red_time)
    {
        traffic_light->state = GREEN;
        traffic_light->current_tick = 0; // Reseta o contador
    }
    else if (traffic_light->state == GREEN && traffic_light->current_tick >= traffic_light->green_time)
    {
        traffic_light->state = RED;
        traffic_light->current_tick = 0; // Reseta o contador
    }

#ifdef _WIN32
    LeaveCriticalSection(&traffic_light->mutex);
#else
    pthread_mutex_unlock(&traffic_light->mutex);
#endif
}

void destroy_traffic_light(TrafficLight *traffic_light)
{
#ifdef _WIN32
    DeleteCriticalSection(&traffic_light->mutex);
#else
    pthread_mutex_destroy(&traffic_light->mutex);
#endif
}
