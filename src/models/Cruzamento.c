#include <stdlib.h>
#include "Cruzamento.h"

/* -----------------------------------------------------------------------
 * cruzamento_init
 * Inicializa todos os campos do cruzamento.
 * O sinal começa VERDE para a via horizontal e VERMELHO para a vertical.
 * Inicializa o mutex e as duas variáveis de condição POSIX.
 * ----------------------------------------------------------------------- */
void cruzamento_init(Cruzamento *c, int id, int linha, int coluna,
                     Via *via_h, Via *via_v)
{
    c->id = id;
    c->linha = linha;
    c->coluna = coluna;

    c->via_horizontal = via_h;
    c->via_vertical = via_v;

    /* Estado inicial dos semáforos */
    c->sinal_h = SINAL_VERDE;
    c->sinal_v = SINAL_VERMELHO;

    /* Ambulância: inativa por padrão */
    c->ambulancia_presente = 0;
    c->direcao_ambulancia = VIA_HORIZONTAL;

    /* Inicializa primitivas de sincronização */
    pthread_mutex_init(&c->mutex, NULL);
    pthread_cond_init(&c->cond_h, NULL);
    pthread_cond_init(&c->cond_v, NULL);
}

/* -----------------------------------------------------------------------
 * cruzamento_destroy
 * Destrói as primitivas de sincronização do cruzamento.
 * Chamar antes de liberar a memória do cruzamento ao encerrar a simulação.
 * ----------------------------------------------------------------------- */
void cruzamento_destroy(Cruzamento *c)
{
    pthread_cond_destroy(&c->cond_v);
    pthread_cond_destroy(&c->cond_h);
    pthread_mutex_destroy(&c->mutex);
}

/* -----------------------------------------------------------------------
 * cruzamento_alternar_sinal
 * Troca o estado dos sinais do cruzamento de forma segura:
 *   VERDE horizontal  → VERMELHO horizontal + VERDE vertical  (acorda cond_v)
 *   VERDE vertical    → VERMELHO vertical   + VERDE horizontal (acorda cond_h)
 *
 * Prioridade da ambulância: se a ambulância estiver presente e já estiver
 * com sinal VERDE na sua direção, a troca é adiada (retorna sem mudar).
 * Isso impede que o semáforo feche justamente quando ela está passando.
 *
 * Chamada pela thread do relógio global (Neto) a cada intervalo de tempo.
 * ----------------------------------------------------------------------- */
void cruzamento_alternar_sinal(Cruzamento *c)
{
    pthread_mutex_lock(&c->mutex);

    /*
     * Se a ambulância está presente, verifica se a sua via já está verde.
     * Em caso positivo, não alterna — ela tem prioridade.
     */
    if (c->ambulancia_presente)
    {
        if (c->direcao_ambulancia == VIA_HORIZONTAL && c->sinal_h == SINAL_VERDE)
        {
            pthread_mutex_unlock(&c->mutex);
            return;
        }
        if (c->direcao_ambulancia == VIA_VERTICAL && c->sinal_v == SINAL_VERDE)
        {
            pthread_mutex_unlock(&c->mutex);
            return;
        }
    }

    /* Alterna os sinais e acorda os carros da via que ficou VERDE */
    if (c->sinal_h == SINAL_VERDE)
    {
        c->sinal_h = SINAL_VERMELHO;
        c->sinal_v = SINAL_VERDE;
        pthread_cond_broadcast(&c->cond_v); /* acorda carros da via vertical */
    }
    else
    {
        c->sinal_h = SINAL_VERDE;
        c->sinal_v = SINAL_VERMELHO;
        pthread_cond_broadcast(&c->cond_h); /* acorda carros da via horizontal */
    }

    pthread_mutex_unlock(&c->mutex);
}

/* -----------------------------------------------------------------------
 * cruzamento_esperar_verde
 * Bloqueia a thread chamadora até que o sinal da via informada fique VERDE.
 *
 * IMPORTANTE: o mutex do cruzamento DEVE estar travado antes de chamar
 * esta função (padrão de uso de pthread_cond_wait). Ao retornar, o mutex
 * ainda estará travado — o caller deve destravar após usar os dados.
 *
 * O loop while() protege contra "spurious wakeups" (acordadas espúrias que
 * o POSIX permite acontecer sem motivo real).
 *
 * Uso típico pelo veículo (Diogo):
 *
 *   pthread_mutex_lock(&cruzamento->mutex);
 *   cruzamento_esperar_verde(cruzamento, VIA_HORIZONTAL);
 *   // sinal está VERDE aqui — avançar
 *   pthread_mutex_unlock(&cruzamento->mutex);
 * ----------------------------------------------------------------------- */
void cruzamento_esperar_verde(Cruzamento *c, DirecaoVia via)
{
    if (via == VIA_HORIZONTAL)
    {
        while (c->sinal_h != SINAL_VERDE)
        {
            pthread_cond_wait(&c->cond_h, &c->mutex);
        }
    }
    else
    {
        while (c->sinal_v != SINAL_VERDE)
        {
            pthread_cond_wait(&c->cond_v, &c->mutex);
        }
    }
}
