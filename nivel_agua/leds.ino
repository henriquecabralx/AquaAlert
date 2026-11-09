/* Modulo LEDS: vermelho (baixo), amarelo (medio), verde (alto).
 * Em erro de sensor os tres piscam juntos. */

void leds_iniciar(void) {
  pinMode(PIN_LED_VERMELHO, OUTPUT);
  pinMode(PIN_LED_AMARELO,  OUTPUT);
  pinMode(PIN_LED_VERDE,    OUTPUT);
  digitalWrite(PIN_LED_VERMELHO, LOW);
  digitalWrite(PIN_LED_AMARELO,  LOW);
  digitalWrite(PIN_LED_VERDE,    LOW);
}

void leds_atualizar(EstadoNivel estado, uint32_t agora_ms) {
  bool vm = false, am = false, vd = false;

  switch (estado) {
    case NIVEL_BAIXO: vm = true; break;
    case NIVEL_MEDIO: am = true; break;
    case NIVEL_ALTO:  vd = true; break;
    case NIVEL_ERRO_SENSOR: {
      bool ligado = ((agora_ms / LED_ERRO_PISCA_MS) % 2) == 0;
      vm = am = vd = ligado;
      break;
    }
    default: break;                    /* DESCONHECIDO: tudo apagado */
  }

  digitalWrite(PIN_LED_VERMELHO, vm);
  digitalWrite(PIN_LED_AMARELO,  am);
  digitalWrite(PIN_LED_VERDE,    vd);
}
