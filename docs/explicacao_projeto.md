# 📖 Explicação do Projeto: Simulador de Tráfego Urbano

> **Disciplina:** Sistemas Operacionais  
> **Linguagem:** C (compatível com Linux e Windows)  
> **Tema Central:** Programação Concorrente com Threads e Sincronização

---

## Visão Geral

O projeto consiste em um **simulador concorrente de tráfego urbano** escrito em **C**. Ele modela um ambiente de trânsito onde diferentes entidades (relógio, semáforos, veículos e uma ambulância) interagem simultaneamente de forma segura, utilizando conceitos fundamentais de Sistemas Operacionais como **Threads**, **Mutexes** e **Variáveis de Condição**.

A visualização ocorre por meio de uma interface **ASCII no terminal**, atualizada a cada tick do relógio global.

---

## Entidades do Simulador

Cada entidade roda como uma **Thread** independente:

| Entidade | Responsabilidade | Thread? |
|---|---|---|
| 🕐 **Relógio Global** | Emite "ticks" (unidades de tempo) que coordenam toda a simulação | Sim |
| 🚦 **Semáforos** | Alternam entre Verde e Vermelho com base no relógio. Cada cruzamento possui sinais para a via horizontal e vertical | Sim (1 por cruzamento) |
| 🚗 **Veículos** | Percorrem as vias, respeitam semáforos, verificam se a célula à frente está livre antes de avançar | Sim (10–20 simultâneos) |
| 🚑 **Ambulância** | Veículo especial com prioridade: ao se aproximar de um cruzamento, solicita que o sinal da sua via fique verde | Sim |

---

## Estrutura do Código-Fonte

O código é modularizado em arquivos dentro de `src/models/`:

```
src/
├── main.c                  # Ponto de entrada: inicializa relógio, semáforos e o loop da simulação
└── models/
    ├── Celula.h / .c       # Unidade mínima do mapa (posição na grade). Tem mutex próprio
    ├── Via.h / .c           # Sequência de células formando uma rua (horizontal/vertical, mão única/dupla)
    ├── Cruzamento.h / .c   # Interseção de duas vias, com semáforo, mutex e variáveis de condição
    ├── Mapa.h / .c          # Container central: matriz 20x40 de células, vias e cruzamentos
    ├── Relogio_global.h/.c  # Thread do relógio global, variável de condição para acordar threads
    ├── Semaforo.h / .c      # Lógica de transição Verde↔Vermelho por tick
    └── Elementos.h          # Definições visuais ASCII (carros, ambulância, semáforos, cenários)
```

---

## Mecanismos de Sincronização Utilizados

| Mecanismo | Onde é usado | Por quê? |
|---|---|---|
| **Mutex por Célula** (`Celula.mutex`) | Cada célula da grade possui um mutex | Garante que dois veículos não ocupem a mesma posição simultaneamente (impenetrabilidade) |
| **Mutex por Cruzamento** (`Cruzamento.mutex`) | Protege os campos do cruzamento (estado do sinal, flag da ambulância) | Evita condição de corrida ao ler/alterar sinais |
| **Variável de Condição** (`clock_cond`) | Relógio global acorda as threads a cada tick | Impede busy-wait: threads dormem até o próximo tick |
| **Variáveis de Condição** (`cond_h`, `cond_v`) | Cruzamentos: carros da via horizontal/vertical dormem no sinal vermelho | Zero consumo de CPU enquanto espera o semáforo abrir |
| **Mutex do Semáforo** (`SemaforoTransito.mutex`) | Protege a leitura/escrita do estado do semáforo | Evita leitura inconsistente durante uma troca de sinal |

---

## Compatibilidade Multiplataforma

O projeto utiliza compilação condicional (`#ifdef _WIN32`) para funcionar tanto em **Linux** (via `pthreads`) quanto em **Windows** (via `windows.h`). As abstrações de tipos estão em `Relogio_global.h`:

| Recurso | Linux (`pthreads`) | Windows |
|---|---|---|
| Mutex | `pthread_mutex_t` | `CRITICAL_SECTION` |
| Thread | `pthread_t` | `HANDLE` |
| Variável de Condição | `pthread_cond_t` | `HANDLE` (Event) |

---

## Como a Simulação Funciona (Fluxo)

```
1. main() inicializa o Relógio Global e os Semáforos
2. A thread do Relógio começa a emitir ticks (1 tick = 1 segundo de simulação)
3. A cada tick:
   a. main() acorda via variável de condição
   b. Semáforos são atualizados (contam ticks e alternam Verde↔Vermelho)
   c. Veículos (threads) acordam, verificam semáforos e tentam avançar
   d. A célula destino é travada (mutex lock) → se livre, o veículo ocupa; se não, espera
   e. O estado é impresso no terminal (visualização ASCII)
4. Ao final, sinaliza simulation_running = false e todas as threads encerram com segurança
```

---

## Conceitos de SO Aplicados no Projeto

Para uma referência rápida dos conceitos teóricos (Threads, Mutex, Condição de Corrida, Deadlock, etc.), consulte o documento complementar:
📚 [conceitos_so.md](file:///home/alan/Documentos/simulador-de-trafico-urbano/docs/conceitos_so.md)

Para entender o mecanismo de Memória Virtual (abordado na disciplina):
📚 [memoria_virtual.md](file:///home/alan/Documentos/simulador-de-trafico-urbano/docs/memoria_virtual.md)
