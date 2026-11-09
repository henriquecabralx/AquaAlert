/* Modulo RELOGIO: data/hora para os logs.
 * O Uno nao tem RTC: o relogio parte da DATA DE COMPILACAO e avanca com
 * millis(). Pode ser ajustado em tempo de execucao (comando serial) e,
 * futuramente, trocado por um RTC DS3231 mantendo a mesma API. */

static DataHora agora_dh;
static uint32_t ultimo_tick_ms = 0;

static bool ano_bissexto(uint16_t a) {
  return ((a % 4 == 0) && (a % 100 != 0)) || (a % 400 == 0);
}

static uint8_t dias_no_mes(uint16_t ano, uint8_t mes) {
  static const uint8_t dias[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  if (mes == 2 && ano_bissexto(ano)) return 29;
  return dias[mes - 1];
}

static void relogio_avancar_segundo(void) {
  if (++agora_dh.segundo < 60) return;
  agora_dh.segundo = 0;
  if (++agora_dh.minuto < 60) return;
  agora_dh.minuto = 0;
  if (++agora_dh.hora < 24) return;
  agora_dh.hora = 0;
  if (++agora_dh.dia <= dias_no_mes(agora_dh.ano, agora_dh.mes)) return;
  agora_dh.dia = 1;
  if (++agora_dh.mes <= 12) return;
  agora_dh.mes = 1;
  agora_dh.ano++;
}

void relogio_iniciar(void) {
  /* __DATE__ = "Nov  9 2026"   __TIME__ = "23:35:12" */
  static const char meses[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  char abrev[4];
  memcpy(abrev, __DATE__, 3);
  abrev[3] = '\0';
  const char *pos = strstr(meses, abrev);

  agora_dh.mes     = pos ? (uint8_t)((pos - meses) / 3 + 1) : 1;
  agora_dh.dia     = (uint8_t)atoi(__DATE__ + 4);
  agora_dh.ano     = (uint16_t)atoi(__DATE__ + 7);
  agora_dh.hora    = (uint8_t)atoi(__TIME__);
  agora_dh.minuto  = (uint8_t)atoi(__TIME__ + 3);
  agora_dh.segundo = (uint8_t)atoi(__TIME__ + 6);
  ultimo_tick_ms   = millis();
}

void relogio_atualizar(uint32_t agora_ms) {
  while ((uint32_t)(agora_ms - ultimo_tick_ms) >= 1000UL) {
    ultimo_tick_ms += 1000UL;
    relogio_avancar_segundo();
  }
}

bool relogio_definir(int ano, int mes, int dia, int hora, int minuto, int segundo) {
  if (ano < 2000 || ano > 2099 || mes < 1 || mes > 12) return false;
  if (dia < 1 || dia > dias_no_mes((uint16_t)ano, (uint8_t)mes)) return false;
  if (hora < 0 || hora > 23 || minuto < 0 || minuto > 59 || segundo < 0 || segundo > 59) return false;

  agora_dh.ano = (uint16_t)ano;   agora_dh.mes = (uint8_t)mes;       agora_dh.dia = (uint8_t)dia;
  agora_dh.hora = (uint8_t)hora;  agora_dh.minuto = (uint8_t)minuto; agora_dh.segundo = (uint8_t)segundo;
  ultimo_tick_ms = millis();
  return true;
}

void relogio_formatar(char *destino, size_t tamanho) {
  snprintf(destino, tamanho, "%04u-%02u-%02u %02u:%02u:%02u",
           agora_dh.ano, agora_dh.mes, agora_dh.dia,
           agora_dh.hora, agora_dh.minuto, agora_dh.segundo);
}
