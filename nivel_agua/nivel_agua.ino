/* =====================================================================
 *  Monitor de nivel de agua - Arduino Uno + JSM-SR04M   (C, sem classes)
 *
 *  Esta aba so ORQUESTRA os modulos; a logica vive nas outras abas.
 *  Fluxo: sensor -> nivel -> (leds, buzzer, log)
 * ===================================================================== */
#include "modulos.h"

static Leitura  leitura_atual   = { 0.0f, 0.0f, NIVEL_DESCONHECIDO };
static uint32_t ultimo_debug_ms = 0;

/* Converte float positivo em decimos (para imprimir 1 casa decimal sem %f,
 * que o printf do AVR nao suporta). */
#define DECIMOS(v)  ((int)((v) * 10.0f + 0.5f))

static void registrar_mudanca_estado(EstadoNivel antes, const Leitura *l) {
  if (l->estado == NIVEL_ERRO_SENSOR) {
    LOG_ERROR("Sensor sem resposta (%d leituras). Verifique ligacao e zona cega (<%d mm)",
              MAX_LEITURAS_INVALIDAS, SENSOR_ALCANCE_MIN_MM);
  } else if (antes == NIVEL_ERRO_SENSOR) {
    LOG_INFO("Sensor voltou a responder. Nivel: %s (%d%%)", nivel_nome(l->estado), (int)(l->percentual + 0.5f));
  } else if (antes == NIVEL_DESCONHECIDO) {
    LOG_INFO("Primeira leitura: nivel %s (%d%%)", nivel_nome(l->estado), (int)(l->percentual + 0.5f));
  } else {
    LOG_INFO("Nivel mudou: %s -> %s (%d%%)", nivel_nome(antes), nivel_nome(l->estado), (int)(l->percentual + 0.5f));
  }
}

void setup() {
  log_iniciar();
  relogio_iniciar();
  sensor_iniciar();
  leds_iniciar();
  buzzer_iniciar();

  LOG_INFO("Monitor de nivel de agua iniciado");
  LOG_INFO("Reservatorio: altura=%d mm, offset do sensor=%d mm", ALTURA_RESERVATORIO_MM, OFFSET_SENSOR_MM);
  LOG_INFO("Faixas: BAIXO < %d%%, ALTO >= %d%%, histerese %d%%", NIVEL_BAIXO_PCT, NIVEL_ALTO_PCT, HISTERESE_PCT);
  LOG_INFO("Relogio iniciado pela data de compilacao. Para ajustar envie: T AAAA-MM-DD HH:MM:SS");
}

void loop() {
  uint32_t agora = millis();

  relogio_atualizar(agora);
  comandos_atualizar();

  /* 1) Medir e calcular (somente quando ha amostra nova) */
  if (sensor_atualizar(agora)) {
    EstadoNivel antes = leitura_atual.estado;
    leitura_atual = nivel_atualizar(sensor_distancia_cm());

    if (leitura_atual.estado != antes) {
      registrar_mudanca_estado(antes, &leitura_atual);
    }

    PadraoBip p = alarme_padrao_para(leitura_atual.estado);
    if (buzzer_definir_padrao(p.liga_ms, p.desliga_ms)) {
      if (p.liga_ms == 0) {
        LOG_INFO("Alarme sonoro desligado");
      } else {
        LOG_INFO("Alarme sonoro ligado (%u ms ligado / %u ms desligado)", p.liga_ms, p.desliga_ms);
      }
    }
  }

  /* 2) Saidas (nao bloqueantes) */
  leds_atualizar(leitura_atual.estado, agora);
  buzzer_atualizar(agora);

  /* 3) Log periodico de debug */
  if (agora - ultimo_debug_ms >= LOG_DEBUG_INTERVALO_MS) {
    ultimo_debug_ms = agora;
    int d10 = DECIMOS(leitura_atual.distancia_cm);
    LOG_DEBUG("distancia=%d.%d cm nivel=%d%% estado=%s amostras_validas=%d/%d",
              d10 / 10, d10 % 10, (int)(leitura_atual.percentual + 0.5f),
              nivel_nome(leitura_atual.estado), sensor_amostras_validas(), JANELA_AMOSTRAS);
  }
}
