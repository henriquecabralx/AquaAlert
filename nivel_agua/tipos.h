#ifndef TIPOS_H
#define TIPOS_H

#include <stdint.h>

/* Tipos compartilhados entre os modulos (contrato de dados). */

typedef enum {
  NIVEL_DESCONHECIDO,   /* boot, ainda sem leitura   */
  NIVEL_BAIXO,          /* vermelho                  */
  NIVEL_MEDIO,          /* amarelo                   */
  NIVEL_ALTO,           /* verde                     */
  NIVEL_ERRO_SENSOR     /* sensor sem resposta       */
} EstadoNivel;

typedef struct {
  float       distancia_cm;   /* sensor -> superficie da agua */
  float       percentual;     /* 0..100 da altura             */
  EstadoNivel estado;
} Leitura;

typedef struct {
  uint16_t liga_ms;           /* 0 = silencio */
  uint16_t desliga_ms;
} PadraoBip;

typedef struct {
  uint16_t ano;
  uint8_t  mes, dia, hora, minuto, segundo;
} DataHora;

#endif /* TIPOS_H */
