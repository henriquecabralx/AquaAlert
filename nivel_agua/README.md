# Monitor de nível de água — Arduino Uno + JSM-SR04M (C, sem classes)

## Como abrir
Pasta `nivel_agua/` → abra `nivel_agua.ino` na Arduino IDE. Cada `.ino` vira uma aba
(a IDE junta todas num único programa; a principal vem primeiro). Placa: Arduino Uno,
Monitor Serial em **9600 baud** com **"Nova linha"** no final.

## Estrutura

```
nivel_agua/
├── nivel_agua.ino   setup()/loop(): só orquestra os módulos
├── config.h         TODAS as constantes (reservatório, limites, pinos, bips, log)
├── tipos.h          struct/enum compartilhados (Leitura, EstadoNivel, PadraoBip, DataHora)
├── modulos.h        API pública (protótipos) de cada módulo + macros LOG_*
├── sensor.ino       driver JSM-SR04M: mediana de 5 amostras, não bloqueante
├── nivel.ino        distância → % → faixa (BAIXO/MÉDIO/ALTO), histerese, tolerância a falha
├── leds.ino         LEDs vermelho / amarelo / verde
├── buzzer.ino       bip intermitente com millis() (sem delay)
├── alarme.ino       regra: qual estado toca qual bip
├── relogio.ino      data/hora dos logs (compilação + millis; ajustável)
├── log.ino          [INFO]/[DEBUG]/[ERROR] AAAA-MM-DD HH:MM:SS - mensagem
└── comandos.ino     comandos pelo Monitor Serial (ajuste do relógio)
```

Convenção: cada módulo usa prefixo (`sensor_`, `nivel_`, `leds_`...) e guarda seu estado em
variáveis `static`; os outros só enxergam o que está em `modulos.h`.

## Logs

```
[INFO] 2026-11-09 23:49:04 - Monitor de nivel de agua iniciado
[INFO] 2026-11-09 23:49:07 - Nivel mudou: MEDIO -> BAIXO (10%)
[INFO] 2026-11-09 23:49:07 - Alarme sonoro ligado (150 ms ligado / 150 ms desligado)
[DEBUG] 2026-11-09 23:49:08 - distancia=55.0 cm nivel=10% estado=BAIXO amostras_validas=5/5
[ERROR] 2026-11-09 23:49:20 - Sensor sem resposta (30 leituras). Verifique ligacao e zona cega (<250 mm)
```
- `INFO`: início, mudanças de nível, alarme ligado/desligado, recuperação do sensor.
- `DEBUG`: leitura a cada 1 s (desligue com `LOG_DEBUG_ATIVO 0` em `config.h`).
- `ERROR`: sensor sem resposta, comando/data inválida.
- Uso no código: `LOG_INFO("valor=%d", x);`. O `printf` do AVR **não suporta `%f`**:
  para decimais use inteiros (veja a macro `DECIMOS` em `nivel_agua.ino`).

### Data e hora
O Uno não tem relógio. O `relogio.ino` começa na **data de compilação** e avança com `millis()`.
Limitações: a cada reset/queda de energia volta à hora da compilação, e o oscilador do Uno
desvia alguns segundos por dia. Para ajustar na hora, envie no Monitor Serial:

```
T 2026-11-09 23:45:00
```
Com um RTC DS3231 (I2C, A4/A5) basta trocar o interior de `relogio.ino`; a API não muda.

## Geometria (ponto crítico)
O JSM-SR04M **não mede abaixo de 25 cm**. O sensor fica acima da caixa:

```
   [sensor]  ─┐
              │  OFFSET_SENSOR_MM  (>= 250; padrão 280)
   ~~~~~~~~~ ─┤  nível máximo (100%)
              │  ALTURA_RESERVATORIO_MM (300)
   ───────── ─┘  fundo (0%)
```
`% = ((offset + altura) − distância medida) / altura × 100`.
Para outro reservatório, mude só os dois `#define` em `config.h`. Se o offset for menor
que 250 mm o sketch **não compila** (`#error`).

## Ligações
| Componente     | Pino | Obs.                                              |
|----------------|------|---------------------------------------------------|
| JSM-SR04M TRIG | 9    | VCC 5 V                                           |
| JSM-SR04M ECHO | 8    |                                                   |
| LED vermelho   | 4    | resistor 220 Ω                                    |
| LED amarelo    | 5    | resistor 220 Ω                                    |
| LED verde      | 6    | resistor 220 Ω                                    |
| Buzzer         | 7    | ativo direto; se >20 mA use transistor            |

Reservados: 2/3 (Bluetooth/Wi-Fi), 10–13 (microSD), A4/A5 (display/RTC).

## Módulos futuros
Cada um é uma nova aba `.ino` + protótipos em `modulos.h` + uma chamada no `loop()`:

| Módulo          | Aba sugerida   | Chamada no loop                       |
|-----------------|----------------|---------------------------------------|
| Display I2C     | `display.ino`  | `display_mostrar(&leitura_atual)`     |
| microSD (log)   | `sdcard.ino`   | `sd_registrar(&leitura_atual)`        |
| Wi-Fi/Bluetooth | `telemetria.ino` | `telemetria_publicar(&leitura_atual)` |

O `log_escrever()` pode ganhar uma segunda saída (SD/Wi-Fi) sem mudar nenhuma chamada `LOG_*`.
Cuidado com a RAM do Uno (2 KB): evite `String`, mantenha textos de log em `LOG_*` (ficam em flash).
