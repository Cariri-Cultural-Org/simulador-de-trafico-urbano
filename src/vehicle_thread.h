#ifndef VEHICLE_THREAD_H
#define VEHICLE_THREAD_H

#include <pthread.h>

/* ─── Constantes ─────────────────────────────────────────── */
#define MAX_VEHICLES 20
#define MIN_VEHICLES 10
#define MAX_ROUTE 256

/* ─── Enums ──────────────────────────────────────────────── */
typedef enum {
    DIRECTION_NORTH,
    DIRECTION_SOUTH,
    DIRECTION_EAST,
    DIRECTION_WEST
} Direction;

typedef enum {
    SPEED_FAST = 1,   /* move a cada 1 tick  */
    SPEED_MEDIUM = 2, /* move a cada 2 ticks */
    SPEED_SLOW = 4    /* move a cada 4 ticks */
} Speed;

typedef enum {
    VEHICLE_TYPE_CAR,
    VEHICLE_TYPE_AMBULANCE
} VehicleType;

typedef enum {
    VEHICLE_STATE_MOVING,
    VEHICLE_STATE_WAITING_CELL,   /* bloqueado esperando célula livre   */
    VEHICLE_STATE_WAITING_SIGNAL, /* bloqueado no sinal vermelho        */
    VEHICLE_STATE_FINISHED
} VehicleState;

/* ─── Posição ────────────────────────────────────────────── */
typedef struct {
    int row;
    int column;
} Position;

/* ─── Estrutura principal ────────────────────────────────── */
typedef struct {
    int id;
    VehicleType type;
    Position position;
    Direction direction;
    Speed speed;
    VehicleState state;

    /* rota: sequência de posições que o veículo deve percorrer */
    Position route[MAX_ROUTE];
    int route_size;
    int route_index;       /* próximo passo da rota */

    /* controle de tick: veículo só move quando tick_contador == velocidade */
    int tick_counter;

    pthread_t thread;
} ThreadVehicle;

/* ─── API pública ────────────────────────────────────────── */

/* Inicializa um veículo com os parâmetros fornecidos */
void thread_vehicle_init(ThreadVehicle *vehicle, int id, VehicleType type,
                         Position initial_position, Direction direction,
                         Speed speed, Position *route, int route_size);

/* Cria a pthread do veículo e começa a simulação */
int thread_vehicle_start(ThreadVehicle *vehicle);

/* Aguarda a thread do veículo terminar */
void thread_vehicle_join(ThreadVehicle *vehicle);

/* Cria e inicia entre MIN_VEHICLES e MAX_VEHICLES veículos */
int thread_vehicles_create_all(ThreadVehicle vehicles[], int quantity);

/* Aguarda todos os veículos finalizarem */
void thread_vehicles_join_all(ThreadVehicle vehicles[], int quantity);

/* ─── Stubs de integração (serão substituídos depois) ───── */

/* [STUB - Alan] Retorna 1 se a célula está livre, 0 caso contrário */
int cell_is_free(Position position);

/* [STUB - Alan] Ocupa a célula para o veículo de id fornecido */
void cell_occupy(Position position, int vehicle_id);

/* [STUB - Alan] Libera a célula */
void cell_release_position(Position position);

/* [STUB - Neto] Bloqueia a thread até o próximo tick do relógio global */
void wait_tick(void);

/* [STUB - Neto] Retorna 1 se o sinal está verde para a direção dada */
int signal_is_green(Position intersection, Direction direction);

/* [STUB - Neto] Bloqueia a thread até o sinal ficar verde */
void wait_green_signal(Position intersection, Direction direction);

#endif /* VEHICLE_THREAD_H */
