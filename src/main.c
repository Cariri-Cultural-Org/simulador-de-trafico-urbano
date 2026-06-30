#include <stdio.h>
#include <stdbool.h>
#include "SimulationOutput.h"
#include "models/Ambulance.h"
#include "models/GlobalClock.h"
#include "models/CityMap.h"
#include "models/Vehicle.h"

/* Inclusões de thread dependentes do SO */
#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef _WIN32
static DWORD WINAPI clock_test_thread(LPVOID arg)
#else
static void *clock_test_thread(void *arg)
#endif
{
    (void)arg;

    int observed_tick = global_tick;

    while (simulation_running)
    {
        wait_next_tick(observed_tick);

        if (!simulation_running)
            break;

        observed_tick = global_tick;
        simulation_output_log("[Test] aguardou tick %d e seguiu vivo\n", observed_tick);
    }

    simulation_output_log("[Test] encerrando junto com o relogio global\n");
    return 0;
}

int main(void)
{
    simulation_output_init();
    simulation_output_log("--- Starting the Urban Traffic Simulator ---\n");

    /* ── M0-C: Criar o CityMap real ─────────────────────────────────────── */
    CityMap *city_map = city_map_create();
    if (!city_map)
    {
        simulation_output_log("ERRO: Falha ao criar o CityMap.\n");
        simulation_output_destroy();
        return 1;
    }

    simulation_output_log("Mapa criado: %d linhas x %d colunas\n",
                          city_map->rows, city_map->columns);
    simulation_output_log("Vias: %d  |  Cruzamentos: %d\n",
                          city_map->road_count, city_map->intersection_count);

    /* ── Inicializa o relógio global ────────────────────────────────────── */
    init_global_clock();

    /* Cria a thread do relógio */
    os_thread_t clock_thread;
#ifdef _WIN32
    clock_thread = CreateThread(NULL, 0, thread_global_clock, NULL, 0, NULL);
#else
    pthread_create(&clock_thread, NULL, thread_global_clock, NULL);
#endif

    /* Thread de teste para validar espera e encerramento limpo */
    os_thread_t test_thread;
#ifdef _WIN32
    test_thread = CreateThread(NULL, 0, clock_test_thread, NULL, 0, NULL);
#else
    pthread_create(&test_thread, NULL, clock_test_thread, NULL);
#endif

    Vehicle ambulance;
    vehicle_init(&ambulance, 1, 1, city_map->roads[0], 0, city_map);
    vehicle_set_render_symbol(&ambulance, 'A');

    os_thread_t ambulance_thread;
#ifdef _WIN32
    ambulance_thread = CreateThread(NULL, 0, thread_ambulance, &ambulance, 0, NULL);
#else
    pthread_create(&ambulance_thread, NULL, thread_ambulance, &ambulance);
#endif

    /* ── Loop de simulação (5 ticks de demonstração) ────────────────────── */
    for (int i = 1; i <= 8; i++)
    {
        wait_next_tick(global_tick);

        if (!simulation_output_render_city_map(city_map, stdout, global_tick))
        {
            simulation_output_log("ERRO: Falha ao renderizar o mapa ASCII.\n");
            break;
        }
    }

    simulation_output_log("Encerrando simulacao...\n");

    /* ── Finaliza o relógio ─────────────────────────────────────────────── */
    stop_global_clock();

#ifdef _WIN32
    WaitForSingleObject(ambulance_thread, INFINITE);
    CloseHandle(ambulance_thread);
    WaitForSingleObject(test_thread, INFINITE);
    CloseHandle(test_thread);
    WaitForSingleObject(clock_thread, INFINITE);
    CloseHandle(clock_thread);
#else
    pthread_join(ambulance_thread, NULL);
    pthread_join(test_thread, NULL);
    pthread_join(clock_thread, NULL);
#endif

    destroy_global_clock();

    /* ── M0-A / M0-C: Destruir o CityMap sem invalid free ──────────────── */
    city_map_destroy(city_map);
    city_map = NULL;

    simulation_output_log("Cleanup concluido. Saindo do simulador.\n");
    simulation_output_destroy();
    return 0;
}
