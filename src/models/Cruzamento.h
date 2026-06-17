
#ifndef CRUZAMENTO_H
#define CRUZAMENTO_H

#include <pthread.h>
#include "Via.h"

typedef enum
{
    SINAL_VERMELHO = 0,
    SINAL_VERDE = 1
} EstadoSinal;

typedef struct
{
    int id;     /* identificador único do cruzamento           */
    int linha;  /* posição na matriz do mapa (eixo Y)          */
    int coluna; /* posição na matriz do mapa (eixo X)          */

    /* Referências às vias que formam este cruzamento */
    Via *via_horizontal; /* rua que passa horizontalmente               */
    Via *via_vertical;   /* rua que passa verticalmente                 */

    /* ------- Semáforo de trânsito ------- */
    EstadoSinal sinal_h; /* sinal atual para a via horizontal           */
    EstadoSinal sinal_v; /* sinal atual para a via vertical             */

    /*
     * Mutex que protege todos os campos deste cruzamento.
     * Deve ser travado antes de ler ou escrever qualquer campo abaixo.
     */
    pthread_mutex_t mutex;

    /*
     * Variáveis de condição: carros bloqueiam aqui quando o sinal
     * da sua via está VERMELHO. Não consomem CPU enquanto esperam.
     *
     *   pthread_cond_wait(&cruzamento->cond_h, &cruzamento->mutex)
     *   → carro da via horizontal dorme até sinal_h == SINAL_VERDE
     *
     *   pthread_cond_wait(&cruzamento->cond_v, &cruzamento->mutex)
     *   → carro da via vertical dorme até sinal_v == SINAL_VERDE
     */
    pthread_cond_t cond_h; /* condição para carros da via horizontal      */
    pthread_cond_t cond_v; /* condição para carros da via vertical        */

    /* ------- Suporte à ambulância (Cícero usa estes campos) ------- */

    /*
     * Flag que a thread da ambulância liga quando está se aproximando.
     * O módulo de semáforos (Neto) verifica esta flag antes de trocar
     * o sinal, garantindo que a direção da ambulância fique VERDE.
     */
    int ambulancia_presente;       /* 0 = não  |  1 = ambulância a caminho      */
    DirecaoVia direcao_ambulancia; /* direção que a ambulância precisa livre */

} Cruzamento;

/* ---------- Funções auxiliares (implementadas em Cruzamento.c) ---------- */

/* Inicializa o cruzamento: zera campos, inicializa mutex e cond vars. */
void cruzamento_init(Cruzamento *c, int id, int linha, int coluna,
                     Via *via_h, Via *via_v);

/* Destrói mutex e variáveis de condição (chamar ao encerrar a simulação). */
void cruzamento_destroy(Cruzamento *c);

/*
 * Alterna o sinal do cruzamento de forma segura.
 * Quando sinal_h = VERDE → sinal_v = VERMELHO, e vice-versa.
 * Após a troca, acorda (broadcast) os carros da via que ficou VERDE.
 * Respeita a prioridade da ambulância: não troca o sinal se isso
 * bloquearia a via em que a ambulância está.
 */
void cruzamento_alternar_sinal(Cruzamento *c);

/*
 * Bloqueia o carro chamador até que o sinal da via informada fique VERDE.
 * Usa pthread_cond_wait internamente — zero busy-wait.
 * DEVE ser chamada com o mutex do cruzamento já travado.
 */
void cruzamento_esperar_verde(Cruzamento *c, DirecaoVia via);

#endif /* CRUZAMENTO_H */
