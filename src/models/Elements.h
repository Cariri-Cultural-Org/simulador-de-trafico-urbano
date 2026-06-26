/*
 * Elements.h
 * ─────────────────────────────────────────────────────────────────
 * Simulador de Tráfego Urbano — Sistemas Operacionais
 * Protótipo visual em ASCII: todos os elementos gráficos do projeto.
 *
 * Como usar:
 *   #include "Elements.h"
 *
 * Convenções:
 *   - Semáforo VERMELHO  → [R]   (Red   — via fechada)
 *   - Semáforo VERDE     → [G]   (Green — via aberta)
 *   - Carro comum        → /---\ com C no interior
 *   - Ambulância         → /###\ com A no interior
 *   - Setas de fluxo     → << (Oeste/Sul)  >> (Leste/Norte)
 * ─────────────────────────────────────────────────────────────────
 */

#ifndef ELEMENTS_ASCII_H
#define ELEMENTS_ASCII_H


/*
 * 1. Células Básicas Do Mapa
 */

#define CELL_ROAD_H         "═══"   /* Rua horizontal               */
#define CELL_ROAD_V         " │ "   /* Rua vertical                 */
#define CELL_INTERSECTION   "─┼─"   /* Interseção de vias           */
#define CELL_EMPTY          "   "   /* Espaço sem via               */
#define CELL_SIDEWALK       "▒▒▒"   /* Calçada / faixa de pedestre  */
#define CELL_BUILDING       "███"   /* Bloco de edifício            */
#define CELL_PARK           " T "   /* Área verde (T = árvore)      */
#define CELL_HOUSE          "_H_"   /* Zona residencial             */


/*
 * 2. Faixas De Tráfego
 */

/* Mão única — fluxo para Oeste (esquerda) */
#define WEST_LANE       " << One-way Lane (West) << "

/* Mão dupla — faixa sentido Leste */
#define EAST_NORTH_LANE " >> North Lane >> "   /* faixa de cima */
#define EAST_SOUTH_LANE " >> South Lane >> "   /* faixa de baixo */

/* Mão dupla — faixa sentido Norte (subindo) */
#define NORTH_LANE      " ^^ North Lane ^^ "

/* Mão dupla — faixa sentido Sul (descendo) */
#define SOUTH_LANE      " vv South Lane   vv "


/*
 * 3. Semáforos
 */

/*
 * Estado A: vertical ABERTA, horizontal FECHADA
 *
 *        │   │   │
 *        │ ^ │ v │
 *  ──────┘   └───┘──────
 *   << [R]         [R] <<
 *  ──────┐         ┌──────
 *   >> [R]         [R] >>
 *  ──────┘   ┌───┐──────
 *        │[G]│ v │
 *        │   │   │
 */
#define TRAFFIC_LIGHT_V_OPEN_H_CLOSED \
    "       |   |   |\n" \
    "       | ^ | v |\n" \
    " ------+   +---+------\n" \
    "  << [R]         [R] <<\n" \
    " ------+         +------\n" \
    "  >> [R]         [R] >>\n" \
    " ------+   +---+------\n" \
    "       |[G]| v |\n" \
    "       |   |   |"

/*
 * Estado B: horizontal ABERTA, vertical FECHADA
 *
 *        │   │   │
 *        │ ^ │ v │
 *  ──────┘   └───┘──────
 *   << [G]         [G] <<
 *  ──────┐         ┌──────
 *   >> [G]         [G] >>
 *  ──────┘   ┌───┐──────
 *        │[R]│ v │
 *        │   │   │
 */
#define TRAFFIC_LIGHT_H_OPEN_V_CLOSED \
    "       |   |   |\n" \
    "       | ^ | v |\n" \
    " ------+   +---+------\n" \
    "  << [G]         [G] <<\n" \
    " ------+         +------\n" \
    "  >> [G]         [G] >>\n" \
    " ------+   +---+------\n" \
    "       |[R]| v |\n" \
    "       |   |   |"


/*
 * 4. Veículos - Carros Comuns
 */

/*  Indo para Norte (subindo)
 *    /---\
 *    | C |
 *    \___/
 */
#define CAR_NORTH \
    " /---\\\n" \
    " | C |\n" \
    " \\___/"

/*  Indo para Sul (descendo)
 *    /___\
 *    | C |
 *    \---/
 */
#define CAR_SOUTH \
    " /___\\\n" \
    " | C |\n" \
    " \\---/"

/*  Indo para Leste (direita)
 *    +------+
 *    |C C C >
 *    +------+
 */
#define CAR_EAST \
    " +------+\n" \
    " |C C C >\n" \
    " +------+"

/*  Indo para Oeste (esquerda)
 *    +------+
 *    < C C C|
 *    +------+
 */
#define CAR_WEST \
    " +------+\n" \
    " < C C C|\n" \
    " +------+"


/*
 * 5. Veículos - Ambulância
 */

/*  Indo para Norte (subindo)
 *    /###\
 *    |[+]|
 *    | A |
 *    \___/
 */
#define AMBULANCE_NORTH \
    " /###\\\n" \
    " |[+]|\n" \
    " | A |\n" \
    " \\___/"

/*  Indo para Sul (descendo)
 *    /___\
 *    | A |
 *    |[+]|
 *    \###/
 */
#define AMBULANCE_SOUTH \
    " /___\\\n" \
    " | A |\n" \
    " |[+]|\n" \
    " \\###/"

/*  Indo para Leste (direita)
 *    +--------+
 *    |A  [+]  >
 *    +--------+
 */
#define AMBULANCE_EAST \
    " +--------+\n" \
    " |A  [+]  >\n" \
    " +--------+"

/*  Indo para Oeste (esquerda)
 *    +--------+
 *    <  [+]  A|
 *    +--------+
 */
#define AMBULANCE_WEST \
    " +--------+\n" \
    " <  [+]  A|\n" \
    " +--------+"


/*
 * 6. Cenário Urbano - Blocos Decorativos
 */

/*
 * Edifício Empresarial (bloco à esquerda do mapa)
 *
 *  +-------------------------------+
 *  | [Edificio Empresarial]        |
 *  | ### ### ### ### ###           |
 *  | # # # # # # # # # #          |
 *  | ### ### ### ### ###           |
 *  +-------------------------------+
 */
#define BUILDING \
    " +-------------------------------+\n" \
    " | [Business Building]           |\n" \
    " | ### ### ### ### ###           |\n" \
    " | # # # # # # # # # #          |\n" \
    " | ### ### ### ### ###           |\n" \
    " +-------------------------------+"

/*
 * Zona Residencial (bloco à direita do mapa)
 *
 *  +---------------+ +---------------+
 *  |   _/\_        | |   _/\_        |
 *  |  /____\       | |  /____\       |
 *  |  | H  |       | |  | H  |       |
 *  +---------------+ +---------------+
 */
#define RESIDENTIAL_ZONE \
    " +---------------+ +---------------+\n" \
    " |   _/\\_        | |   _/\\_        |\n" \
    " |  /____\\       | |  /____\\       |\n" \
    " |  | H  |       | |  | H  |       |\n" \
    " +---------------+ +---------------+"

/*
 * Parque Urbano (bloco inferior direito)
 *
 *  +---------------------------------+
 *  | [PARQUE URBANO]  ### Pista      |
 *  |  T   T   T       ### Caminhada  |
 *  |    T   T   T                    |
 *  +---------------------------------+
 */
#define URBAN_PARK \
    " +---------------------------------+\n" \
    " | [URBAN PARK]     ### Track      |\n" \
    " |  T   T   T       ### Walkway    |\n" \
    " |    T   T   T                    |\n" \
    " +---------------------------------+"

/*
 * Área Verde / Bosque (bloco inferior esquerdo)
 *
 *  +-------------------------------+
 *  |  T   T   T   T   T   T       |
 *  |    T   T   T   T   T   T     |
 *  +-------------------------------+
 */
#define WOODS \
    " +-------------------------------+\n" \
    " |  T   T   T   T   T   T       |\n" \
    " |    T   T   T   T   T   T     |\n" \
    " +-------------------------------+"


/*
 * 7. Frames De Animação - Exemplo De Ticks
 */

/*
 * TICK 01 — Carro comum desce pelo eixo vertical.
 *            Ambulância aguarda abaixo, no eixo vertical.
 *            Semáforo horizontal: [R] | Semáforo vertical: [G]
 *
 *      |   |     |   |
 *      |   |/---\|   |
 *      |   || C ||   |
 *   ---+   |\___/+   +---
 *   << [R]       [R] <<
 *   ---+         +---
 *   >> [R]       [R] >>
 *   ---+   |     |   +---
 *      |   |     |   |
 *      |/###\    |   |
 *      || A |    |   |
 */
#define TICK_01 \
    "      |   |     |   |\n" \
    "      |   |/---\\|   |\n" \
    "      |   || C ||   |\n" \
    "   ---+   |\\___/|   +---\n" \
    "   << [R]         [R] <<\n" \
    "   ---+           +---\n" \
    "   >> [R]         [R] >>\n" \
    "   ---+   |     |   +---\n" \
    "      |   |     |   |\n" \
    "      |/###\\    |   |\n" \
    "      || A |    |   |"

/*
 * TICK 02 — Carro comum entra no cruzamento.
 *            Ambulância avança para o cruzamento.
 *            Semáforo horizontal: [R] | Semáforo vertical: [G]
 *
 *      |   |     |   |
 *      |   |     |   |
 *      |   |     |   |
 *   ---+   |/---\|   +---
 *   << [R] || C ||[R] <<
 *   ---+   |\___/|   +---
 *   >> [R]       [R] >>
 *   ---+   |/###\|   +---
 *      |   || A ||   |
 *      |   || + ||   |
 */
#define TICK_02 \
    "      |   |     |   |\n" \
    "      |   |     |   |\n" \
    "      |   |     |   |\n" \
    "   ---+   |/---\\|   +---\n" \
    "   << [R] || C ||[R] <<\n" \
    "   ---+   |\\___/|   +---\n" \
    "   >> [R]         [R] >>\n" \
    "   ---+   |/###\\|   +---\n" \
    "      |   || A ||   |\n" \
    "      |   || + ||   |"

/*
 * TICK 03 — Carro comum sai do cruzamento (sul).
 *            Ambulância ocupa o cruzamento com prioridade.
 *            Carro horizontal entra pela faixa leste.
 *            Semáforo: ambulância força verde vertical.
 *
 *      |   |     |   |
 *      |   |     |   |
 *      |   |/---\|   |
 *   ---+   || C ||   +---
 *  +------+|\___/|
 *  |C C C >| [R] |  [G] <<
 *  +------+|/###\|
 *   >> [G] || A || [R] >>
 *   ---+   || + ||   +---
 *      |   |\___/|   |
 */
#define TICK_03 \
    "      |   |     |   |\n" \
    "      |   |     |   |\n" \
    "      |   |/---\\|   |\n" \
    "   ---+   || C ||   +---\n" \
    "  +------+|\\___/|\n" \
    "  |C C C >|  [R]|      [G] <<\n" \
    "  +------+|/###\\|\n" \
    "   >> [G] || A ||  [R] >>\n" \
    "   ---+   || + ||   +---\n" \
    "      |   |\\___/|   |"


/*
 * 8. Legenda
 */

#define LEGEND \
    "+----------------------------------------------------------+\n" \
    "| Symbol   | Meaning            | Logical Function         |\n" \
    "+----------+--------------------+--------------------------+\n" \
    "| ### / _H_| Facades / Houses   | Physical barrier.        |\n" \
    "| ###      | Safety Lane        | Pedestrian crossing.     |\n" \
    "| ^ v < >  | Flow Arrows        | Mandatory direction.     |\n" \
    "| [R]      | Red Traffic Light  | Vehicles stop and sleep. |\n" \
    "| [G]      | Green Traffic Light| Free flow.               |\n" \
    "| /---\\    | Common Car   (C)   | Respects lights.         |\n" \
    "| /###\\    | Ambulance    (A)   | Signal priority.         |\n" \
    "+----------------------------------------------------------+"


/*
 * 9. Identificadores De Veículos Na Grade
 */

#define ID_CAR          'C'
#define ID_AMBULANCE    'A'
#define ID_EMPTY        ' '
#define ID_WALL         '#'
#define ID_ROAD         '.'
#define ID_INTERSECTION '+'
#define ID_GREEN_LIGHT  'G'
#define ID_RED_LIGHT    'R'


#endif /* ELEMENTS_ASCII_H */
