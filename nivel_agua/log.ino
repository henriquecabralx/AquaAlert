/* Modulo LOG: imprime no Monitor Serial no formato
 *   [INFO]  AAAA-MM-DD HH:MM:SS - mensagem
 *   [DEBUG] AAAA-MM-DD HH:MM:SS - mensagem
 *   [ERROR] AAAA-MM-DD HH:MM:SS - mensagem
 * Uso:  LOG_INFO("valor=%d", x);   (formato fica em flash, poupa RAM)
 * Obs.: printf do AVR nao suporta %f; use inteiros (ver DECIMOS). */

void log_iniciar(void) {
  Serial.begin(SERIAL_BAUD);
}

void log_escrever(const char *tag, const char *formato_P, ...) {
  char data_hora[20];
  char msg[LOG_TAM_MENSAGEM];

  relogio_formatar(data_hora, sizeof(data_hora));

  va_list args;
  va_start(args, formato_P);
  vsnprintf_P(msg, sizeof(msg), formato_P, args);
  va_end(args);

  Serial.print('[');
  Serial.print(tag);
  Serial.print(F("] "));
  Serial.print(data_hora);
  Serial.print(F(" - "));
  Serial.println(msg);
}
