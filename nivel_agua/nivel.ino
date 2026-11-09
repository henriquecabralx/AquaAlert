/* Modulo NIVEL: distancia (cm) -> percentual -> faixa (BAIXO/MEDIO/ALTO),
 * com histerese e tolerancia a falhas. Nao conhece pinos nem hardware. */

static EstadoNivel estado_atual     = NIVEL_DESCONHECIDO;
static uint8_t     invalidas_seguidas = 0;
static float       ultima_distancia = 0.0f;
static float       ultimo_percentual = 0.0f;

const char *nivel_nome(EstadoNivel e) {
  switch (e) {
    case NIVEL_BAIXO:       return "BAIXO";
    case NIVEL_MEDIO:       return "MEDIO";
    case NIVEL_ALTO:        return "ALTO";
    case NIVEL_ERRO_SENSOR: return "ERRO_SENSOR";
    default:                return "INICIANDO";
  }
}

Leitura nivel_atualizar(float distancia_cm) {
  Leitura r;

  /* Leitura invalida: mantem o ultimo estado ate estourar a tolerancia. */
  if (distancia_cm < 0.0f) {
    if (invalidas_seguidas < 255) invalidas_seguidas++;
    if (invalidas_seguidas >= MAX_LEITURAS_INVALIDAS) estado_atual = NIVEL_ERRO_SENSOR;
    r.distancia_cm = ultima_distancia;
    r.percentual   = ultimo_percentual;
    r.estado       = estado_atual;
    return r;
  }
  invalidas_seguidas = 0;

  /* distancia sensor->fundo = offset + altura
   * altura da agua          = (offset + altura) - distancia medida */
  float agua_cm = (OFFSET_SENSOR_CM + ALTURA_RESERVATORIO_CM) - distancia_cm;
  float pct = (agua_cm / ALTURA_RESERVATORIO_CM) * 100.0f;
  if (pct < 0.0f)   pct = 0.0f;
  if (pct > 100.0f) pct = 100.0f;      /* inclui agua dentro da zona cega */

  ultima_distancia  = distancia_cm;
  ultimo_percentual = pct;

  /* percentual -> faixa, com histerese */
  switch (estado_atual) {
    case NIVEL_BAIXO:
      if (pct > NIVEL_BAIXO_PCT + HISTERESE_PCT) estado_atual = NIVEL_MEDIO;
      break;
    case NIVEL_ALTO:
      if (pct < NIVEL_ALTO_PCT - HISTERESE_PCT) estado_atual = NIVEL_MEDIO;
      break;
    default: /* DESCONHECIDO, MEDIO ou ERRO_SENSOR: classificacao direta */
      if (pct < NIVEL_BAIXO_PCT)       estado_atual = NIVEL_BAIXO;
      else if (pct >= NIVEL_ALTO_PCT)  estado_atual = NIVEL_ALTO;
      else                             estado_atual = NIVEL_MEDIO;
      break;
  }

  r.distancia_cm = ultima_distancia;
  r.percentual   = ultimo_percentual;
  r.estado       = estado_atual;
  return r;
}
