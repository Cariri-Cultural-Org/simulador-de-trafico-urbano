#include <stdio.h>
#include "vehicle_thread.h"
#include "models/GlobalClock.h"
#include "models/TrafficLight.h"

// Inclusões para gerenciar a thread inicial na main (dependente do SO)
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

int main()
{
    printf("--- Starting the Urban Traffic Simulator ---\n");

    // 1. Inicializa o relógio global
    init_global_clock();

    // 2. Inicializa um semáforo de teste
    TrafficLight traffic_light;
    // Semáforo ID 1 | 3 ticks no GREEN | 3 ticks no RED
    init_traffic_light(&traffic_light, 1, 3, 3);

    // 3. Cria e inicia a thread do relógio
    os_thread_t thread_id;

#ifdef _WIN32
    thread_id = CreateThread(NULL, 0, thread_global_clock, NULL, 0, NULL);
#else
    pthread_create(&thread_id, NULL, thread_global_clock, NULL);
#endif

    // 4. Exemplo de veículo com thread própria
    Position test_route[] = {{0, 1}, {0, 2}, {0, 3}};
    ThreadVehicle test_vehicle;
    thread_vehicle_init(&test_vehicle, 1, VEHICLE_TYPE_CAR,
                        (Position){0, 0}, DIRECTION_EAST,
                        SPEED_FAST, test_route, 3);

    if (thread_vehicle_start(&test_vehicle) != 0) {
        fprintf(stderr, "Failed to start the vehicle thread.\n");
    }

    // Loop principal da simulação para fins de teste
    // Vamos simular por 10 ticks e depois encerrar
    for (int i = 1; i <= 10; i++)
    {
        // Pausa a thread principal até o relógio "bater" o próximo tick
        wait_next_tick(global_tick);

        // Assim que o tick vira, atualizamos o semáforo
        update_traffic_light(&traffic_light);

        // Imprime o estado atual
        printf("[Tick: %02d] Traffic light %d is: %s\n",
               global_tick,
               traffic_light.id,
               traffic_light.state == GREEN ? "GREEN" : "RED");
    }

    printf("\nStopping the simulation...\n");

    // 4. Sinaliza para a thread do relógio parar
    simulation_running = false;

    // 5. Aguarda a thread do relógio finalizar com segurança
#ifdef _WIN32
    WaitForSingleObject(thread_id, INFINITE);
    CloseHandle(thread_id);
#else
    pthread_join(thread_id, NULL);
#endif

    // 6. Aguarda a thread do veículo finalizar
    thread_vehicle_join(&test_vehicle);

    // 7. Limpa os recursos da memória
    destroy_traffic_light(&traffic_light);
    destroy_global_clock();

    printf("Cleanup completed. Exiting the simulator.\n");
    return 0;
}
