#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#ifdef _WIN32
#include <windows.h>
typedef CRITICAL_SECTION os_mutex_t;
#else
#include <pthread.h>
typedef pthread_mutex_t os_mutex_t;
#endif

typedef enum
{
    RED,
    GREEN
} TrafficLightState;

typedef struct
{
    int id;
    TrafficLightState state;
    int green_time;
    int red_time;
    int current_tick;
    os_mutex_t mutex;
} TrafficLight;

// Inicializa o semáforo de trânsito
void init_traffic_light(TrafficLight *traffic_light, int id, int green_time, int red_time);

// Atualiza o estado do semáforo a cada tick
void update_traffic_light(TrafficLight *traffic_light);

// Limpa recursos do semáforo
void destroy_traffic_light(TrafficLight *traffic_light);

#endif // TRAFFIC_LIGHT_H
