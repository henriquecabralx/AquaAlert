/* Modulo ALARME: regra de negocio "qual estado toca qual bip".
 * Fica separada do driver do buzzer para mudar a politica sem mexer no
 * hardware (ex.: no futuro, enviar alerta por Wi-Fi no mesmo ponto). */

PadraoBip alarme_padrao_para(EstadoNivel estado) {
  PadraoBip p = { 0, 0 };                         /* silencio */

  switch (estado) {
    case NIVEL_BAIXO:
      if (ALARME_NIVEL_BAIXO) { p.liga_ms = BIP_BAIXO_LIGA_MS; p.desliga_ms = BIP_BAIXO_DESLIGA_MS; }
      break;
    case NIVEL_ALTO:
      if (ALARME_NIVEL_ALTO)  { p.liga_ms = BIP_ALTO_LIGA_MS;  p.desliga_ms = BIP_ALTO_DESLIGA_MS; }
      break;
    case NIVEL_ERRO_SENSOR:
      if (ALARME_ERRO_SENSOR) { p.liga_ms = BIP_ERRO_LIGA_MS;  p.desliga_ms = BIP_ERRO_DESLIGA_MS; }
      break;
    default: break;
  }
  return p;
}
