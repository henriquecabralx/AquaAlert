/* Modulo COMANDOS: leitura de comandos pelo Monitor Serial (nao bloqueante).
 * Configure o Monitor Serial com "Nova linha" no final.
 *   T AAAA-MM-DD HH:MM:SS   -> ajusta o relogio (ex.: T 2026-11-09 23:45:00) */

static char    linha[32];
static uint8_t linha_len = 0;

static void comandos_executar(const char *cmd) {
  int a, mo, d, h, mi, s;

  if ((cmd[0] == 'T' || cmd[0] == 't') &&
      sscanf(cmd + 1, "%d-%d-%d %d:%d:%d", &a, &mo, &d, &h, &mi, &s) == 6) {
    if (relogio_definir(a, mo, d, h, mi, s)) {
      LOG_INFO("Relogio ajustado manualmente");
    } else {
      LOG_ERROR("Data/hora invalida. Use: T AAAA-MM-DD HH:MM:SS");
    }
    return;
  }
  LOG_ERROR("Comando desconhecido: %s", cmd);
}

void comandos_atualizar(void) {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\r') continue;
    if (c == '\n') {
      linha[linha_len] = '\0';
      if (linha_len > 0) comandos_executar(linha);
      linha_len = 0;
    } else if (linha_len < sizeof(linha) - 1) {
      linha[linha_len++] = c;
    }
  }
}
