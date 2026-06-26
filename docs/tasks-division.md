# Simulador de Tráfego Urbano em C — Plano de Implementação por MVP

> **Equipe:** Alan · Diogo · Neto · Cícero
>
> **Janela revisada:** 26/06/2026 → 03/07/2026
>
> **Última atualização:** 26/06/2026
>
> **Objetivo da revisão:** reduzir dependências entre tarefas e chegar cedo a um executável visual, mesmo que simples.

---

## Diagnóstico do plano anterior

O plano anterior separava o trabalho por features finais:

- Alan ficava com mapa, exclusão mútua e contratos de movimento;
- Diogo ficava com veículos, rotas e movimento;
- Neto ficava com relógio, semáforos e sincronização;
- Cícero ficava com ambulância, visualização e relatório.

Essa divisão parece organizada, mas cria dependências fortes:

- Diogo só consegue validar veículos depois que a API de movimento e o mapa estiverem estáveis.
- Cícero só consegue implementar ambulância depois que veículo, semáforo e prioridade estiverem integrados.
- A visualização ficava tarde demais, então os models eram escritos sem feedback visual.
- O `main.c` executava apenas um teste isolado de relógio/semáforo, não a simulação.
- O build real em 26/06/2026 falhava em `Ambulance.c`, então a primeira etapa do MVP é preservar a compilação.

O novo plano troca "donos de features finais" por "incrementos pequenos e demonstráveis". Cada tarefa deve terminar com algo que compila, roda e mostra algum comportamento.

---

## Definição do MVP inicial

O primeiro MVP não é a entrega completa. Ele é a menor simulação útil para a equipe enxergar o sistema funcionando.

O MVP inicial está pronto quando:

1. `make` compila sem erros;
2. `./bin/traffic-simulator` executa sem travar;
3. o programa cria e destrói o mapa real;
4. o terminal mostra frames ASCII com tick, ruas, cruzamentos e pelo menos um veículo;
5. o veículo anda por uma via fixa usando células reais do mapa;
6. a ocupação exclusiva de célula é respeitada;
7. a simulação encerra sozinha depois de uma quantidade limitada de ticks;
8. todos os recursos usados no MVP são encerrados corretamente.

Fica fora do MVP inicial:

- ambulância com prioridade;
- rotas complexas;
- conversões aleatórias;
- 10 a 20 veículos;
- mão dupla com faixas independentes;
- política final de semáforos;
- relatório final.

Esses itens entram depois que existir uma base executável e observável.

---

## Estado real do código em 26/06/2026

| Área | Estado real | Consequência para o plano |
|---|---|---|
| Build | `make` deve compilar usando `src/models/Ambulance.c` alinhado com `road_has_intersection`, `vertical_road` e `horizontal_road` | Primeira tarefa é preservar a compilação |
| `main.c` | roda apenas relógio + semáforo isolado por 10 ticks | Trocar para uma demo integrada com mapa e renderização |
| Mapa | `CityMap`, `Cell`, `Road` e `Intersection` já existem | Usar isso como base do MVP |
| Destruição de células | `cell_destroy(&city_map->cells[row][column])` deve destruir apenas o mutex, pois a célula pertence ao vetor da linha | Corrigir antes de confiar em criação/destruição repetida |
| Cruzamentos | `CityMap` cria cruzamentos reais por ponteiro; `Road` guarda ponteiros para esses mesmos cruzamentos | Usar `CityMap.intersections` como fonte de verdade |
| Semáforos | Existe `TrafficLight`, mas `Intersection` já possui `green_direction` e condições próprias | Para a simulação real, usar `Intersection` como fonte de verdade |
| Veículos | `Vehicle` existe como struct inicial; ainda não há thread nem movimento comum | Implementar movimento mínimo antes de rotas |
| Ambulância | `src/models/Ambulance.c` concentra o movimento especial, mas a prioridade ainda fica para etapa futura | Deixar fora do MVP inicial ou manter comportamento mínimo compilável |
| Visualização | `Elements.h` tem símbolos, mas não existe renderizador integrado | Criar renderizador simples primeiro, antes de arte ASCII detalhada |

---

## Contratos mínimos congelados

Para reduzir bloqueios, os contratos abaixo devem ser mantidos pequenos até o MVP inicial funcionar.

### Mapa (`CityMap`)

- `CityMap *city_map_create(void)` cria a malha real.
- `void city_map_destroy(CityMap *city_map)` destrói a malha sem invalid free ou vazamento óbvio.
- `Cell *city_map_get_cell(const CityMap *city_map, int row, int column)` devolve uma célula real.
- `Intersection *city_map_get_intersection(const CityMap *city_map, int row, int column)` é a forma oficial de descobrir se uma posição é cruzamento.

### Célula (`Cell`)

- `cell_try_occupy(c, v)` tenta ocupar sem bloquear indefinidamente.
- `cell_release(c)` libera a célula atual.
- No MVP, se a próxima célula estiver ocupada, o veículo fica parado e tenta de novo no próximo tick.
- Nenhuma thread deve dormir aguardando tick ou sinal segurando mutex de célula.

### Veículo (`Vehicle`)

- No MVP, um veículo anda somente dentro de uma `Road`.
- A posição é `current_road + road_cell_index`.
- Movimento mínimo: `index + 1`, com parada no fim ou retorno ao início da via.
- Velocidade, conversão, sentido real da mão dupla e rota complexa entram depois.

### Relógio (`GlobalClock`)

- O tick global continua sendo a unidade de tempo da simulação.
- Threads podem esperar com `wait_next_tick(current_tick)`.
- O encerramento precisa acordar qualquer thread bloqueada.

### Visualização

- O renderizador lê o `CityMap` e as `Cell`.
- Uma célula ocupada por veículo comum pode ser exibida como `C`.
- Uma célula ocupada por ambulância pode ser exibida como `A` depois do MVP.
- No MVP, o renderer pode usar caracteres simples: espaço, `-`, `|`, `+`, `C`.

---

## Estratégia de implementação

A ordem abaixo evita que uma pessoa dependa da feature completa da outra.

1. Recuperar build e ciclo de vida do mapa.
2. Mostrar mapa estático no terminal.
3. Fazer um veículo comum andar em uma via fixa.
4. Ligar o movimento ao tick global.
5. Colocar mais de um veículo disputando células.
6. Alternar sinais dos cruzamentos.
7. Fazer veículos respeitarem cruzamento.
8. Adicionar ambulância como especialização de veículo.
9. Refinar rotas, velocidade, mão dupla, logs e relatório.

Enquanto as etapas 1 a 4 não estiverem prontas, nenhuma feature avançada deve ser tratada como P0.

---

## Tabela simples de responsabilidades e dependências

Esta tabela parte do que já existe no repositório em 26/06/2026 e mostra a menor próxima entrega útil de cada integrante.

| Integrante | Parte já implementada que deve aproveitar | Próximo foco | Depende de quem | Desbloqueia quem |
|---|---|---|---|---|
| Alan | `CityMap`, `Cell`, `Road` e `Intersection` já existem parcialmente; células já têm mutex e ocupação/liberação | Fazer o projeto voltar a compilar, corrigir destruição de células/cruzamentos e iniciar/destruir o mapa real em `main.c` | Não depende de ninguém para começar | Diogo consegue mover veículo em mapa real; Cícero consegue renderizar estado real; Neto consegue alternar cruzamentos reais |
| Diogo | `Vehicle` já existe como struct inicial, mas sem thread nem movimento | Posicionar um veículo em uma `Road` existente e implementar movimento mínimo de uma célula por tick | Depende de Alan para build e mapa real compilando; depois depende de Neto para usar tick em thread | Cícero consegue mostrar `C` andando; Neto consegue validar espera por sinal com veículo real |
| Neto | `GlobalClock` já acorda threads por tick; `TrafficLight` existe como protótipo isolado; `Intersection` já tem `green_direction` | Garantir encerramento limpo do relógio e fazer os cruzamentos reais alternarem sinal | Depende de Alan para usar `CityMap.intersections` como fonte real | Diogo consegue fazer veículo esperar no vermelho; Cícero consegue mostrar sinais e depois prioridade |
| Cícero | `Elements.h` já tem símbolos ASCII; `Ambulance.c` existe como base compilável para movimento especial | Criar renderizador ASCII simples primeiro; deixar ambulância para depois do veículo comum funcionando | Para renderizar mapa vazio, depende só de Alan; para mostrar veículo, depende de Diogo; para sinais, depende de Neto | Todos ganham feedback visual cedo; depois a ambulância pode ser implementada sobre a mesma API do veículo |

Resumo das dependências críticas:

- Diogo não deve esperar ambulância, semáforo final ou rotas completas; ele só precisa de Alan entregar build + mapa real.
- Cícero não deve começar pela ambulância; ele pode entregar visualização do mapa assim que Alan estabilizar o mapa.
- Neto não precisa esperar veículos para estabilizar relógio e encerramento, mas precisa do mapa real para alternar cruzamentos integrados.
- Ambulância depende de três coisas prontas: movimento comum de Diogo, cruzamento/sinal de Neto e visualização de Cícero.

---

## Tarefas por incremento

### Incremento 0 — Build compilando e mapa visível

**Meta:** até o fim de 26/06/2026, `make` e `./bin/traffic-simulator` devem funcionar e mostrar um mapa estático ou quase estático.

| ID | Responsável | Tarefa | Não depende de | Critério de aceite |
|---|---|---|---|---|
| M0-A | Alan | Corrigir ciclo de vida de `Cell` e `Intersection` para o mapa criar/destruir sem invalid free | Veículos, semáforos, renderer | Rodar `city_map_create()` + `city_map_destroy()` no fluxo principal sem crash |
| M0-B | Alan | Recuperar compilação do projeto, adaptando ou neutralizando temporariamente `Ambulance.c` | Prioridade da ambulância | `make` termina com sucesso |
| M0-C | Alan | Alterar `main.c` para criar o `CityMap` real e destruir no final | Veículos prontos | `./bin/traffic-simulator` imprime pelo menos dimensões e quantidade de vias/cruzamentos |
| M0-D | Cícero | Criar renderizador ASCII simples do mapa | Ambulância, semáforo real | O terminal mostra 20 linhas por frame, com ruas e cruzamentos reconhecíveis |
| M0-E | Neto | Garantir que o relógio tenha encerramento limpo e acorde esperas no fim | Veículos reais | Uma thread de teste espera ticks e termina quando a simulação acaba |
| M0-F | Diogo | Criar função de posicionamento inicial de um `Vehicle` em uma `Road` existente | Renderer, semáforo | Uma célula real fica ocupada por um veículo e aparece como ocupada no debug |

**Demo esperada ao fim do incremento:**

```text
Tick 0
        |       |       |       |
--------+-------+-------+-------+-------
        |       |       |       |
...
```

Se o movimento ainda não estiver pronto, o mapa estático já vale como MVP técnico porque prova que o projeto compila, inicializa o estado compartilhado e tem uma saída observável.

### Incremento 1 — Um veículo se move

**Meta:** em 27/06/2026, um veículo comum deve andar por uma via fixa, primeiro sem semáforo.

| ID | Responsável | Tarefa | Não depende de | Critério de aceite |
|---|---|---|---|---|
| V1-A | Diogo | Implementar `vehicle_advance_one_step()` usando `road_get_cell`, `cell_try_occupy` e `cell_release` | Semáforo, ambulância, rotas | O veículo sai da célula N e ocupa N+1 sem duplicar posição |
| V1-B | Diogo | Se a próxima célula estiver ocupada, manter o veículo parado | Semáforo, prioridade | O veículo não sobrescreve outro ocupante |
| V1-C | Cícero | Exibir célula ocupada como `C` no renderer | Thread de veículo | A posição do veículo é visível no frame |
| V1-D | Neto | Fazer o loop principal esperar ticks para atualizar/renderizar | Vários veículos | Cada frame corresponde a um tick |
| V1-E | Alan | Revisar invariantes de ocupação de célula | Rotas completas | Não existe frame em que o mesmo veículo apareça em duas células |

**Demo esperada:**

```text
Tick 1
----C---+-------+-------+-------

Tick 2
-----C--+-------+-------+-------
```

### Incremento 2 — Veículo em thread e disputa simples

**Meta:** em 28/06/2026, veículos devem executar em threads e disputar células de forma segura.

| ID | Responsável | Tarefa | Não depende de | Critério de aceite |
|---|---|---|---|---|
| T2-A | Diogo | Transformar o loop do veículo em thread própria | Semáforo | Uma thread por veículo avança com base no tick |
| T2-B | Neto | Padronizar encerramento: flag global, broadcast final e `join` | Ambulância | Nenhuma thread fica presa ao finalizar |
| T2-C | Alan | Criar cenário com dois veículos na mesma via | Cruzamentos | O veículo de trás bloqueia ou espera sem ocupar a célula do da frente |
| T2-D | Cícero | Sincronizar impressão para não misturar logs e frames | Ambulância | Frames aparecem inteiros, sem linhas intercaladas |

**Demo esperada:**

```text
Tick 8
--------C-C-----+-------+-------
```

O objetivo não é realismo. O objetivo é provar concorrência, ocupação exclusiva e visualização.

### Incremento 3 — Cruzamentos (`Intersection`) e semáforos reais

**Meta:** em 29/06/2026, cruzamentos devem alternar direção liberada e veículos devem respeitar o sinal.

| ID | Responsável | Tarefa | Não depende de | Critério de aceite |
|---|---|---|---|---|
| S3-A | Neto | Usar `Intersection.green_direction` como fonte de verdade da simulação | `TrafficLight` isolado | Horizontal e vertical não ficam verdes ao mesmo tempo |
| S3-B | Neto | Alternar todos os cruzamentos a cada N ticks | Ambulância | O estado muda de forma visível no terminal ou logs |
| S3-C | Diogo | Antes de entrar em cruzamento, consultar `city_map_get_intersection()` | Ambulância | Veículo para no vermelho e anda no verde |
| S3-D | Alan | Garantir que espera por sinal não segura mutex de célula | Ambulância | Não há espera circular óbvia entre célula e cruzamento |
| S3-E | Cícero | Mostrar sinal horizontal/vertical no renderer com marcador simples | Arte final | É possível entender visualmente qual direção está liberada |

**Decisão importante:** `TrafficLight` pode continuar existindo como módulo didático, mas a simulação integrada deve usar o estado do `Intersection`. Ter duas fontes de verdade aumenta o risco de inconsistência.

### Incremento 4 — Ambulância mínima

**Meta:** em 30/06/2026, a ambulância deve existir como veículo especial visível, ainda com prioridade simples.

| ID | Responsável | Tarefa | Não depende de | Critério de aceite |
|---|---|---|---|---|
| A4-A | Cícero | Reimplementar ambulância sobre a mesma API do veículo comum | Rotas complexas | Ambulância compila, anda e aparece como `A` |
| A4-B | Neto | Criar protocolo simples de prioridade no cruzamento | Rotas complexas | Quando a ambulância chega, a direção dela é liberada sem abrir fluxos conflitantes |
| A4-C | Diogo | Garantir que a ambulância continue respeitando célula ocupada | Prioridade avançada | Ambulância não atravessa outro veículo |
| A4-D | Alan | Revisar liberação da prioridade e destruição de recursos | Relatório | O cruzamento volta ao ciclo normal depois da passagem |

Prioridade da ambulância não deve ser implementada antes de veículo comum + cruzamento estarem funcionando. Caso contrário, a equipe volta ao problema de escrever código abstrato sem conseguir executar.

### Incremento 5 — Entrega completa

**Meta:** entre 01/07/2026 e 03/07/2026, evoluir o MVP para os critérios da disciplina.

| ID | Responsável | Tarefa | Critério de aceite |
|---|---|---|---|
| F5-A | Diogo | Criar 10 a 20 veículos com rotas reproduzíveis | Simulação roda por tempo prolongado sem sair do mapa |
| F5-B | Diogo | Implementar velocidades 1, 2 e 4 ticks | Diferença de velocidade é visível |
| F5-C | Alan | Refinar mão única, mão dupla e limites de via | Veículos não trafegam em sentido inválido |
| F5-D | Neto | Fortalecer espera bloqueante e broadcasts de encerramento | Encerramento não deixa threads presas |
| F5-E | Cícero | Melhorar visualização e separar logs de frames | Saída fica legível para demonstração |
| F5-F | Todos | Testes de concorrência e execução prolongada | Sem deadlock em cenários repetidos |
| F5-G | Cícero | Relatório final baseado no comportamento real | Documento descreve o que foi entregue, não só o planejado |
| F5-H | Alan | Atualizar README com arquitetura final e limitações | README bate com o código executável |

---

## Nova divisão de responsabilidades

### Alan — Núcleo executável e invariantes do mapa

Alan não deve ser gargalo da API de movimento. A responsabilidade principal passa a ser garantir que o estado compartilhado seja confiável.

Tarefas principais:

- manter `CityMap`, `Cell`, `Road` e `Intersection` compilando juntos;
- corrigir criação/destruição de recursos;
- garantir ocupação exclusiva de célula;
- revisar a ordem de locks;
- manter `main.c` sempre executável;
- validar que nenhuma mudança quebra o MVP.

Entregas visíveis:

- mapa inicializado e destruído no executável;
- cenário mínimo com células ocupadas;
- checklist de invariantes de concorrência.

### Diogo — Movimento mínimo evolutivo

Diogo não deve esperar uma API sofisticada de rotas. Ele pode começar com o contrato atual de `Road` e `Cell`.

Tarefas principais:

- posicionar veículo em uma via existente;
- mover de índice em índice;
- não sobrescrever célula ocupada;
- transformar o movimento em thread;
- depois adicionar velocidades, sentido e rotas.

Entregas visíveis:

- um `C` andando no mapa;
- dois veículos disputando uma via;
- logs mínimos de bloqueio/movimento quando necessário.

### Neto — Tempo, encerramento e sinais

Neto pode evoluir relógio e cruzamentos sem depender de veículos completos.

Tarefas principais:

- manter o tick global como fonte de tempo;
- garantir encerramento limpo;
- alternar cruzamentos;
- acordar threads bloqueadas em sinal;
- impedir estados conflitantes de semáforo.

Entregas visíveis:

- ticks avançando no frame;
- cruzamentos alternando estado;
- threads encerrando sem travar.

### Cícero — Observabilidade primeiro, ambulância depois

Cícero não deve começar pela ambulância avançada. A primeira contribuição precisa reduzir a abstração para todo mundo.

Tarefas principais:

- criar renderização ASCII simples;
- mostrar veículos e sinais;
- sincronizar saída no terminal;
- depois implementar ambulância sobre a API comum;
- documentar o comportamento real.

Entregas visíveis:

- frames legíveis desde o começo;
- ambulância aparecendo como `A` quando a base estiver pronta;
- relatório coerente com a execução.

---

## Regras para manter tarefas desconexas

1. Toda tarefa P0 deve ter uma forma de execução local.
2. Nenhum model novo conta como concluído se não for chamado por `main.c`, por uma demo ou por um teste simples.
3. Branches não devem depender de código não mergeado de outra pessoa.
4. Quando uma tarefa precisar de outra feature, criar um stub pequeno ou usar o contrato mínimo existente.
5. O executável principal deve continuar compilando durante todo o projeto.
6. Features avançadas entram atrás do MVP, não na frente dele.
7. A visualização deve evoluir junto com o comportamento, não no fim.
8. Antes de mudar uma struct compartilhada, listar quais arquivos serão afetados.
9. Se uma mudança quebrar `make`, ela deve ser corrigida antes de qualquer nova feature.
10. O relatório deve acompanhar as decisões reais tomadas durante a implementação.

---

## Cronograma revisado

| Data | Marco verificável |
|---|---|
| **26/06/2026** | `make` volta a compilar; `./bin/traffic-simulator` cria mapa e mostra frame ASCII básico |
| **27/06/2026** | Um veículo comum se move por uma via fixa e aparece no mapa |
| **28/06/2026** | Dois ou mais veículos rodam em threads e disputam células sem ocupação dupla |
| **29/06/2026** | Cruzamentos alternam sinal e veículos respeitam vermelho/verde |
| **30/06/2026** | Ambulância mínima anda, aparece no mapa e solicita prioridade simples |
| **01/07/2026** | 10 a 20 veículos, velocidades e cenários de concorrência em execução prolongada |
| **02/07/2026** | Congelamento funcional; apenas correções críticas, visualização e documentação |
| **03/07/2026** | Entrega do código, README, relatório e divisão final coerente com commits |

---

## Checklist de validação por tarefa

Antes de marcar qualquer tarefa como concluída:

```bash
make
./bin/traffic-simulator
```

A tarefa só pode ser marcada como pronta se:

- compila sem erro;
- roda sem crash;
- altera algo visível ou verificável;
- não deixa threads presas;
- não introduz espera ocupada;
- não exige branch não mergeada de outro integrante;
- tem critério de aceite demonstrável.

---

## Plano mínimo de testes

| Cenário | Resultado esperado | Quando entra |
|---|---|---|
| Criar e destruir mapa | Sem crash, sem invalid free evidente | Incremento 0 |
| Renderizar mapa vazio | Frames mostram vias e cruzamentos | Incremento 0 |
| Posicionar um veículo | Uma célula aparece ocupada | Incremento 0 |
| Mover um veículo | `C` muda de posição a cada tick configurado | Incremento 1 |
| Próxima célula ocupada | Veículo permanece parado sem sobrescrever ocupante | Incremento 1 |
| Dois veículos em threads | Ambos avançam sem ocupar a mesma célula | Incremento 2 |
| Encerramento durante espera | Todas as threads acordam e recebem `join` | Incremento 2 |
| Cruzamento vermelho | Veículo espera antes de entrar | Incremento 3 |
| Alternância de sinal | Apenas uma direção fica liberada por vez | Incremento 3 |
| Ambulância com prioridade | Prioridade altera o sinal sem violar ocupação exclusiva | Incremento 4 |
| Execução prolongada | Sem deadlock em várias execuções | Incremento 5 |

---

## Critérios finais da entrega

| Critério | Responsável principal | Observação |
|---|---|---|
| Simulação executável | Todos | Mantida desde o MVP |
| Visualização ASCII | Cícero | Começa simples e evolui |
| Exclusão mútua por célula | Alan | Base de segurança do mapa |
| Veículos em threads | Diogo | Começa com uma via fixa |
| Relógio e sinais | Neto | Fonte única de tick e cruzamentos |
| Ambulância | Cícero + Neto | Entra depois da base comum |
| Ausência de deadlock | Alan + Neto | Validada por cenários repetidos |
| README e relatório | Alan + Cícero | Devem descrever o comportamento real |

---

## Resumo da nova abordagem

O projeto deve parar de avançar por models isolados e passar a avançar por demonstrações pequenas.

Primeiro, o simulador precisa compilar e mostrar o mapa. Depois, um veículo anda. Depois, vários veículos disputam células. Depois, sinais entram no caminho. Só então a ambulância recebe prioridade.

Essa ordem reduz bloqueios entre integrantes e dá feedback imediato sobre se as structs, locks e regras estão corretas.
