/* Modulo SENSOR: driver do JSM-SR04M (modo trigger/echo, como o HC-SR04).
 * Nao bloqueante: colhe 1 amostra a cada INTERVALO_AMOSTRA_MS e devolve a
 * mediana da janela (rejeita ruido e ondulacao da agua). */

static float    amostras_cm[JANELA_AMOSTRAS];
static uint8_t  amostra_idx;
static uint32_t amostra_ultima_ms;

void sensor_iniciar(void) {
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIG, LOW);
  for (uint8_t i = 0; i < JANELA_AMOSTRAS; i++) amostras_cm[i] = -1.0f;
  amostra_idx = 0;
  amostra_ultima_ms = 0;
}

/* Uma medicao crua em cm, ou -1 se falhou. */
static float sensor_medir_uma_vez(void) {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(4);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(15);
  digitalWrite(PIN_TRIG, LOW);

  uint32_t pulso_us = pulseIn(PIN_ECHO, HIGH, ECHO_TIMEOUT_US);
  if (pulso_us == 0) return -1.0f;                 /* sem eco */

  float cm = pulso_us / US_POR_CM;
  if (cm > ALCANCE_MAX_CM) return -1.0f;           /* fora de alcance */
  return cm;
}

bool sensor_atualizar(uint32_t agora_ms) {
  if (agora_ms - amostra_ultima_ms < INTERVALO_AMOSTRA_MS) return false;
  amostra_ultima_ms = agora_ms;
  amostras_cm[amostra_idx] = sensor_medir_uma_vez();
  amostra_idx = (amostra_idx + 1) % JANELA_AMOSTRAS;
  return true;
}

uint8_t sensor_amostras_validas(void) {
  uint8_t n = 0;
  for (uint8_t i = 0; i < JANELA_AMOSTRAS; i++)
    if (amostras_cm[i] >= 0.0f) n++;
  return n;
}

float sensor_distancia_cm(void) {
  float   v[JANELA_AMOSTRAS];
  uint8_t n = 0;
  for (uint8_t i = 0; i < JANELA_AMOSTRAS; i++)
    if (amostras_cm[i] >= 0.0f) v[n++] = amostras_cm[i];

  if (n < MIN_AMOSTRAS_VALIDAS) return -1.0f;

  for (uint8_t i = 1; i < n; i++) {                /* ordenacao por insercao */
    float chave = v[i];
    int8_t j = i - 1;
    while (j >= 0 && v[j] > chave) { v[j + 1] = v[j]; j--; }
    v[j + 1] = chave;
  }
  return v[n / 2];
}
