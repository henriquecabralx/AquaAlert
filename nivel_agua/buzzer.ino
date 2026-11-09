/* Modulo BUZZER: bip intermitente NAO bloqueante (sem delay()).
 * Padrao (0, 0) = silencio. */

static uint16_t bip_liga_ms     = 0;
static uint16_t bip_desliga_ms  = 0;
static bool     bip_soando      = false;
static bool     bip_reiniciar   = false;
static uint32_t bip_fase_inicio = 0;

static void buzzer_saida(bool ligado) {
  bip_soando = ligado;
#if BUZZER_PASSIVO
  if (ligado) tone(PIN_BUZZER, BUZZER_FREQ_HZ); else noTone(PIN_BUZZER);
#else
  digitalWrite(PIN_BUZZER, ligado ? HIGH : LOW);
#endif
}

void buzzer_iniciar(void) {
  pinMode(PIN_BUZZER, OUTPUT);
  buzzer_saida(false);
}

bool buzzer_definir_padrao(uint16_t liga_ms, uint16_t desliga_ms) {
  if (liga_ms == bip_liga_ms && desliga_ms == bip_desliga_ms) return false;
  bip_liga_ms    = liga_ms;
  bip_desliga_ms = desliga_ms;
  bip_reiniciar  = true;
  return true;
}

void buzzer_atualizar(uint32_t agora_ms) {
  if (bip_liga_ms == 0) {                         /* silencio */
    if (bip_soando) buzzer_saida(false);
    bip_reiniciar = false;
    return;
  }
  if (bip_reiniciar) {                            /* novo padrao: comeca ligando */
    bip_reiniciar = false;
    bip_fase_inicio = agora_ms;
    buzzer_saida(true);
    return;
  }
  uint16_t duracao = bip_soando ? bip_liga_ms : bip_desliga_ms;
  if (agora_ms - bip_fase_inicio >= duracao) {
    bip_fase_inicio = agora_ms;
    buzzer_saida(!bip_soando);
  }
}
