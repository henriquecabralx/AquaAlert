#ifndef MODULOS_H
#define MODULOS_H

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "config.h"
#include "tipos.h"

/* API publica de cada modulo (cada um e uma aba .ino).
 * Variaveis internas de cada aba sao 'static': so as funcoes abaixo
 * sao visiveis para os outros modulos. */

/* --- sensor.ino --- */
void    sensor_iniciar(void);
bool    sensor_atualizar(uint32_t agora_ms);  /* true = nova amostra colhida */
float   sensor_distancia_cm(void);            /* mediana; < 0 se invalida    */
uint8_t sensor_amostras_validas(void);

/* --- nivel.ino --- */
Leitura     nivel_atualizar(float distancia_cm);   /* < 0 = leitura invalida */
const char *nivel_nome(EstadoNivel e);

/* --- leds.ino --- */
void leds_iniciar(void);
void leds_atualizar(EstadoNivel estado, uint32_t agora_ms);

/* --- buzzer.ino --- */
void buzzer_iniciar(void);
bool buzzer_definir_padrao(uint16_t liga_ms, uint16_t desliga_ms); /* true = mudou */
void buzzer_atualizar(uint32_t agora_ms);

/* --- alarme.ino --- */
PadraoBip alarme_padrao_para(EstadoNivel estado);

/* --- relogio.ino --- */
void relogio_iniciar(void);
void relogio_atualizar(uint32_t agora_ms);
bool relogio_definir(int ano, int mes, int dia, int hora, int minuto, int segundo);
void relogio_formatar(char *destino, size_t tamanho);  /* "AAAA-MM-DD HH:MM:SS" */

/* --- log.ino --- */
void log_iniciar(void);
void log_escrever(const char *tag, const char *formato_P, ...);

#define LOG_INFO(fmt, ...)   log_escrever("INFO",  PSTR(fmt), ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...)  log_escrever("ERROR", PSTR(fmt), ##__VA_ARGS__)
#if LOG_DEBUG_ATIVO
#define LOG_DEBUG(fmt, ...)  log_escrever("DEBUG", PSTR(fmt), ##__VA_ARGS__)
#else
#define LOG_DEBUG(fmt, ...)  ((void)0)
#endif

/* --- comandos.ino --- */
void comandos_atualizar(void);

#endif /* MODULOS_H */
