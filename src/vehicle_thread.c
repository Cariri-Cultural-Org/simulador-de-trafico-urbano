#include "vehicle_thread.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>   /* sleep — usado apenas nos stubs temporários */

/* ═══════════════════════════════════════════════════════════
 *  STUBS DE INTEGRAÇÃO
 *  Remova estes blocos conforme Alan e Neto entregarem
 *  as implementações reais.
 * ═══════════════════════════════════════════════════════════ */

int cell_is_free(Position position) {
    /* [STUB - Alan] Sempre retorna livre por enquanto */
    (void)position;
    return 1;
}

void cell_occupy(Position position, int vehicle_id) {
    /* [STUB - Alan] Sem-op até o mapa existir */
    (void)position;
    (void)vehicle_id;
}

void cell_release_position(Position position) {
    /* [STUB - Alan] Sem-op até o mapa existir */
    (void)position;
}

void wait_tick(void) {
    /* [STUB - Neto] Substitua por pthread_cond_wait no relógio global */
    sleep(1);
}

int signal_is_green(Position intersection, Direction direction) {
    /* [STUB - Neto] Sempre verde por enquanto */
    (void)intersection;
    (void)direction;
    return 1;
}

void wait_green_signal(Position intersection, Direction direction) {
    /* [STUB - Neto] Substitua por pthread_cond_wait no semáforo de trânsito */
    while (!signal_is_green(intersection, direction)) {
        sleep(1); /* temporário — será removido com a integração do Neto */
    }
}

/* ═══════════════════════════════════════════════════════════
 *  FUNÇÕES INTERNAS
 * ═══════════════════════════════════════════════════════════ */

static const char *direction_to_string(Direction direction) {
    switch (direction) {
        case DIRECTION_NORTH: return "NORTH";
        case DIRECTION_SOUTH: return "SOUTH";
        case DIRECTION_EAST: return "EAST";
        case DIRECTION_WEST: return "WEST";
        default: return "?";
    }
}

static const char *vehicle_type_to_string(VehicleType type) {
    return type == VEHICLE_TYPE_AMBULANCE ? "AMBULANCE" : "CAR";
}

/* Tenta mover o veículo para o próximo passo da rota.
 * Retorna 1 se moveu, 0 se bloqueado (célula ocupada ou sinal). */
static int try_move(ThreadVehicle *vehicle) {
    if (vehicle->route_index >= vehicle->route_size) {
        /* Rota concluída */
        return 0;
    }

    Position destination = vehicle->route[vehicle->route_index];

    /* ── 1. Verificar sinal de trânsito ───────────────────── */
    if (!signal_is_green(destination, vehicle->direction)) {
        vehicle->state = VEHICLE_STATE_WAITING_SIGNAL;
        printf("[%s #%d] red signal at (%d,%d) - blocking...\n",
               vehicle_type_to_string(vehicle->type), vehicle->id,
               destination.row, destination.column);

        wait_green_signal(destination, vehicle->direction);

        printf("[%s #%d] green signal - resuming.\n",
               vehicle_type_to_string(vehicle->type), vehicle->id);
        vehicle->state = VEHICLE_STATE_MOVING;
    }

    /* ── 2. Verificar se a célula de destino está livre ──── */
    if (!cell_is_free(destination)) {
        vehicle->state = VEHICLE_STATE_WAITING_CELL;
        printf("[%s #%d] cell (%d,%d) occupied - waiting...\n",
               vehicle_type_to_string(vehicle->type), vehicle->id,
               destination.row, destination.column);

        /* Espera bloqueante: tenta novamente no próximo tick.
         * A implementação real usará pthread_cond_wait (Alan/Neto). */
        return 0;
    }

    /* ── 3. Mover: libera célula atual, ocupa a próxima ──── */
    cell_release_position(vehicle->position);
    cell_occupy(destination, vehicle->id);

    printf("[%s #%d] (%d,%d) -> (%d,%d) [%s]\n",
           vehicle_type_to_string(vehicle->type), vehicle->id,
           vehicle->position.row, vehicle->position.column,
           destination.row, destination.column,
           direction_to_string(vehicle->direction));

    vehicle->position = destination;
    vehicle->route_index++;
    vehicle->state = VEHICLE_STATE_MOVING;
    return 1;
}

/* ─── Função principal da thread ────────────────────────── */
static void *vehicle_thread_run(void *arg) {
    ThreadVehicle *vehicle = (ThreadVehicle *)arg;

    printf("[%s #%d] started at (%d,%d) speed=%d\n",
           vehicle_type_to_string(vehicle->type), vehicle->id,
           vehicle->position.row, vehicle->position.column,
           (int)vehicle->speed);

    /* Ocupa a célula inicial */
    cell_occupy(vehicle->position, vehicle->id);

    while (vehicle->route_index < vehicle->route_size) {
        /* ── Aguarda o tick do relógio global ─────────────── */
        wait_tick();

        /* ── Controle de velocidade ───────────────────────── *
         * Só tenta mover quando tick_contador atingir o valor
         * da velocidade (1, 2 ou 4 ticks).                   */
        vehicle->tick_counter++;
        if (vehicle->tick_counter < (int)vehicle->speed) {
            continue;  /* ainda não é hora de mover */
        }
        vehicle->tick_counter = 0;

        /* ── Tenta mover ──────────────────────────────────── */
        try_move(vehicle);
    }

    /* Libera a célula final ao terminar */
    cell_release_position(vehicle->position);

    vehicle->state = VEHICLE_STATE_FINISHED;
    printf("[%s #%d] route completed. Finishing thread.\n",
           vehicle_type_to_string(vehicle->type), vehicle->id);

    return NULL;
}

/* ═══════════════════════════════════════════════════════════
 *  API PÚBLICA
 * ═══════════════════════════════════════════════════════════ */

void thread_vehicle_init(ThreadVehicle *vehicle, int id, VehicleType type,
                         Position initial_position, Direction direction,
                         Speed speed, Position *route, int route_size) {
    memset(vehicle, 0, sizeof(ThreadVehicle));

    vehicle->id = id;
    vehicle->type = type;
    vehicle->position = initial_position;
    vehicle->direction = direction;
    vehicle->speed = speed;
    vehicle->state = VEHICLE_STATE_MOVING;
    vehicle->tick_counter = 0;
    vehicle->route_index = 0;
    vehicle->route_size = route_size > MAX_ROUTE ? MAX_ROUTE : route_size;

    memcpy(vehicle->route, route, vehicle->route_size * sizeof(Position));
}

int thread_vehicle_start(ThreadVehicle *vehicle) {
    int result = pthread_create(&vehicle->thread, NULL, vehicle_thread_run, vehicle);
    if (result != 0) {
        fprintf(stderr, "[ERROR] pthread_create failed for vehicle #%d (code %d)\n",
                vehicle->id, result);
        return -1;
    }
    return 0;
}

void thread_vehicle_join(ThreadVehicle *vehicle) {
    pthread_join(vehicle->thread, NULL);
}

int thread_vehicles_create_all(ThreadVehicle vehicles[], int quantity) {
    if (quantity < MIN_VEHICLES || quantity > MAX_VEHICLES) {
        fprintf(stderr, "[ERROR] invalid quantity: %d (expected %d-%d)\n",
                quantity, MIN_VEHICLES, MAX_VEHICLES);
        return -1;
    }

    int failures = 0;
    for (int i = 0; i < quantity; i++) {
        if (thread_vehicle_start(&vehicles[i]) != 0) {
            failures++;
        }
    }
    return failures == 0 ? 0 : -1;
}

void thread_vehicles_join_all(ThreadVehicle vehicles[], int quantity) {
    for (int i = 0; i < quantity; i++) {
        thread_vehicle_join(&vehicles[i]);
    }
}
