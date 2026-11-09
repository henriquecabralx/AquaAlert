#ifndef CONFIG_H
#define CONFIG_H

/* =====================================================================
 *  CONFIGURACAO CENTRAL
 *  Tudo que muda de um reservatorio/montagem para outro fica aqui.
 *  Medidas em milimetros (inteiros) para permitir checagens em #if.
 * ===================================================================== */

/* ---------- Reservatorio ---------- */
/* Altura util: do fundo ate o nivel MAXIMO de agua (100%). */
#define ALTURA_RESERVATORIO_MM   300

/* Distancia da face do sensor ate o nivel MAXIMO de agua (100%).
 * O JSM-SR04M nao mede abaixo de 250 mm: deve ser >= 250.
 * Use folga (ex.: 280) para a agua cheia nao cair na zona cega. */
#define OFFSET_SENSOR_MM         280

/* ---------- Sensor JSM-SR04M ---------- */
#define SENSOR_ALCANCE_MIN_MM    250
#define SENSOR_ALCANCE_MAX_MM    4500
#define US_POR_CM                58.3f      /* ida+volta, som a ~343 m/s (20 C) */
#define ECHO_TIMEOUT_US          30000UL
#define INTERVALO_AMOSTRA_MS     100        /* 1 amostra a cada 100 ms          */
#define JANELA_AMOSTRAS          5          /* mediana das ultimas N amostras   */
#define MIN_AMOSTRAS_VALIDAS     3
#define MAX_LEITURAS_INVALIDAS   30         /* ~3 s sem leitura -> erro sensor  */

/* ---------- Faixas de nivel (% da altura) ---------- */
#define NIVEL_BAIXO_PCT          15         /* abaixo: BAIXO (vermelho)         */
#define NIVEL_ALTO_PCT           85         /* acima:  ALTO  (verde)            */
#define HISTERESE_PCT            3

/* ---------- Alarme sonoro (1 = ativo, 0 = inativo) ---------- */
#define ALARME_NIVEL_BAIXO       1
#define ALARME_NIVEL_ALTO        1
#define ALARME_ERRO_SENSOR       1

/* Padroes de bip intermitente (ms ligado / ms desligado) */
#define BIP_BAIXO_LIGA_MS        150
#define BIP_BAIXO_DESLIGA_MS     150
#define BIP_ALTO_LIGA_MS         500
#define BIP_ALTO_DESLIGA_MS      1000
#define BIP_ERRO_LIGA_MS         80
#define BIP_ERRO_DESLIGA_MS      920

#define BUZZER_PASSIVO           0          /* 1 = buzzer passivo (usa tone())  */
#define BUZZER_FREQ_HZ           2000

#define LED_ERRO_PISCA_MS        500

/* ---------- Log ---------- */
#define SERIAL_BAUD              9600
#define LOG_DEBUG_ATIVO          1          /* 0 = remove as mensagens [DEBUG]  */
#define LOG_DEBUG_INTERVALO_MS   1000
#define LOG_TAM_MENSAGEM         112

/* ---------- Pinos (Arduino Uno) ---------- */
#define PIN_ECHO                 8
#define PIN_TRIG                 9
#define PIN_LED_VERMELHO         4
#define PIN_LED_AMARELO          5
#define PIN_LED_VERDE            6
#define PIN_BUZZER               7
/* Reservados p/ expansoes futuras (nao usar):
 *   0/1   Serial USB (logs)
 *   2/3   SoftwareSerial p/ Bluetooth (HC-05) ou Wi-Fi (ESP-01)
 *   10-13 SPI -> microSD (CS = 10)
 *   A4/A5 I2C -> display LCD/OLED ou RTC DS3231                       */

/* ---------- Derivados e verificacoes em tempo de compilacao ---------- */
#define ALTURA_RESERVATORIO_CM   (ALTURA_RESERVATORIO_MM / 10.0f)
#define OFFSET_SENSOR_CM         (OFFSET_SENSOR_MM / 10.0f)
#define ALCANCE_MAX_CM           (SENSOR_ALCANCE_MAX_MM / 10.0f)

#if OFFSET_SENSOR_MM < SENSOR_ALCANCE_MIN_MM
#error "OFFSET_SENSOR_MM deve ser >= 250 (zona cega do JSM-SR04M)"
#endif
#if (OFFSET_SENSOR_MM + ALTURA_RESERVATORIO_MM) > SENSOR_ALCANCE_MAX_MM
#error "Reservatorio fora do alcance maximo do sensor"
#endif
#if NIVEL_BAIXO_PCT >= NIVEL_ALTO_PCT
#error "NIVEL_BAIXO_PCT deve ser menor que NIVEL_ALTO_PCT"
#endif

#endif /* CONFIG_H */
