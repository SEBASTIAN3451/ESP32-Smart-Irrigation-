# 💧 ESP32 Smart Irrigation System PRO v2.0# 🌱 ESP32 Smart Irrigation System



Sistema profesional de riego inteligente con capacidades avanzadas de configuración remota, persistencia de datos y observabilidad completa para agricultura de precisión.![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue)

![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange)

![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue)![License](https://img.shields.io/badge/License-MIT-green)

![PlatformIO](https://img.shields.io/badge/PlatformIO-Core-orange)![IoT](https://img.shields.io/badge/IoT-Agriculture-brightgreen)

![License](https://img.shields.io/badge/license-MIT-green)

Sistema inteligente de riego automático para plantas con monitoreo de humedad del suelo, control de bomba de agua y tres modos de operación: Automático, Manual y Programado.

## ✨ Features v2.0 PRO

## 📸 Preview

### 🚀 Productividad y DevOps

- **OTA (Over-The-Air Updates)**: Actualiza firmware remotamente vía ArduinoOTA![Dashboard](https://via.placeholder.com/800x400/667eea/ffffff?text=Smart+Irrigation+Dashboard)

- **WiFiManager**: Portal cautivo para configuración WiFi sin hardcodear credenciales

- **Persistent Schedules**: Horarios guardados en NVS (no se pierden al reiniciar)## ✨ Características Principales

- **mDNS**: Acceso vía `http://esp32-irrigation.local`

- 💧 **Riego Automático Inteligente**: Riega solo cuando el suelo lo necesita

### 🌱 Control de Riego Avanzado- 📊 **Monitoreo en Tiempo Real**: Visualiza la humedad del suelo constantemente

- **Modo AUTO**: Riego automático basado en umbrales de humedad- 🎮 **3 Modos de Operación**: Automático, Manual y Programado

- **Modo MANUAL**: Control directo desde dashboard web- 📱 **Dashboard Web Responsive**: Control desde cualquier dispositivo

- **Modo SCHEDULED**: Hasta 3 horarios programables con persistencia- 🔔 **Indicador LED RGB**: Estado visual del sistema

- **Estadísticas Completas**: Total de riegos, tiempo acumulado, histórico de eventos- 📈 **Estadísticas**: Historial de riegos y tiempo total

- ⏱️ **Programación Horaria**: 3 horarios diarios configurables (nuevo)

### 📊 Observabilidad y Datos- ⚡ **Tiempo Real (SSE)**: Actualizaciones instantáneas del estado de bomba y humedad

- **Dashboard Web Interactivo**: UI moderna con Chart.js para gráficos en tiempo real- 🌐 **mDNS**: Acceso por `http://esp32-irrig.local`

- **Historial de Eventos**: Últimos 50 riegos con timestamp, duración y humedad- 🛡️ **Protección**: Límite de tiempo máximo de riego (seguridad)

- **Server-Sent Events (SSE)**: Actualizaciones en tiempo real sin polling

- **Export CSV**: Descarga log completo de eventos de riego## 🚀 Funcionalidades

- **Prometheus Metrics**: Endpoint `/metrics` para monitoreo externo

- **Indicador LED RGB**: Estado visual (verde=OK, azul=regando, magenta=OTA)### Modo Automático ⚡

- Monitorea la humedad del suelo cada 10 segundos

### 🔧 Endpoints API REST- Activa riego cuando humedad < 30%

- `GET /` - Dashboard principal con gráficos Chart.js- Detiene riego cuando humedad > 70%

- `GET /api/status` - Estado actual (humedad, bomba, modo, estadísticas)- Límite de seguridad: máximo 30 segundos de riego continuo

- `GET /api/history` - Histórico de eventos de riego (JSON)

- `GET /api/export` - Export CSV de histórico### Modo Manual 🎮

- `GET /api/schedules` - Obtener horarios programados- Control total desde el dashboard

- `POST /api/schedules` - Guardar horarios (persistente en NVS)- Botones de inicio/detener riego

- `GET /api/mode?m=AUTO|MANUAL|SCHEDULED` - Cambiar modo operación- Ideal para pruebas o riegos puntuales

- `GET /api/pump?state=0|1` - Control manual bomba

- `GET /metrics` - Métricas Prometheus### Modo Programado ⏰

- `GET /healthz` - Health check- 3 horarios configurables

- `GET /events` - SSE stream- Duración personalizable por horario

- Activación/desactivación individual

## 🛠️ Hardware Requerido

## 🛠️ Hardware Requerido

| Componente | Descripción | GPIO |

|------------|-------------|------|| Componente | Cantidad | Especificaciones | Precio aprox. |

| ESP32 DevKit | Microcontrolador principal | - ||------------|----------|------------------|---------------|

| Sensor Humedad Suelo | Capacitivo (v1.2 o v2.0) | GPIO 34 (ADC) || ESP32 DevKit | 1 | ESP32-WROOM-32 | $5-10 USD |

| Relé 5V | Control de bomba/válvula | GPIO 25 || Sensor de Humedad Capacitivo | 1 | V1.2 o superior | $2-5 USD |

| LED RGB WS2812B | Indicador de estado (NeoPixel) | GPIO 5 || Módulo Relé 5V | 1 | 1 canal, optoacoplado | $1-3 USD |

| Bomba de agua 5V | O electroválvula | Relé NO/COM || LED RGB WS2812B | 1 | NeoPixel compatible | $0.50 USD |

| Bomba de Agua 5V | 1 | Mini bomba sumergible | $3-5 USD |

### 📐 Conexiones| Fuente 5V 2A | 1 | Para bomba y ESP32 | $3-5 USD |

| Cables Jumper | varios | Macho-Hembra | $2 USD |

```

Sensor Humedad:**Total estimado**: ~$20-30 USD

  VCC  -> 3.3V

  AOUT -> GPIO 34## 📐 Diagrama de Conexión

  GND  -> GND

```

Relé:ESP32           Componente

  VCC  -> 5V (VIN)-----           ----------

  IN   -> GPIO 25GPIO 34  <-->  Sensor Humedad (A0/OUT)

  GND  -> GNDGPIO 25  <-->  Relé (IN)

  NO   -> Bomba (+)GPIO 5   <-->  LED RGB (DIN)

  COM  -> 5V Externo3.3V     <-->  Sensor Humedad (VCC)

5V       <-->  Relé (VCC), LED (VCC)

LED RGB WS2812B:GND      <-->  GND común

  VCC  -> 5V (VIN)

  DIN  -> GPIO 5Relé            Bomba

  GND  -> GND----            -----

COM      <-->  VCC Bomba

Bomba:NO       <-->  (+) Fuente 5V

  (+)  -> Relé NOGND      <-->  GND Bomba

  (-)  -> 5V Externo GND```

```

### Esquema Visual

⚠️ **IMPORTANTE**: Usar fuente externa 5V para bomba (NO alimentar desde ESP32)

```

## 📦 Instalación                    ┌──────────────┐

                    │   ESP32      │

### 1. Clonar repositorio                    │   DevKit     │

```bash                    └──────┬───────┘

git clone https://github.com/SEBASTIAN3451/ESP32-Smart-Irrigation-.git                           │

cd ESP32-Smart-Irrigation-        ┌──────────────────┼──────────────────┐

```        │                  │                  │

   ┌────▼────┐        ┌────▼────┐       ┌────▼────┐

### 2. Instalar PlatformIO   │ Sensor  │        │  Relé   │       │ LED RGB │

```bash   │ Humedad │        │   5V    │       │ WS2812B │

pip install platformio   └─────────┘        └────┬────┘       └─────────┘

```                           │

                      ┌────▼────┐

### 3. Compilar y cargar                      │  Bomba  │

```bash                      │   5V    │

pio run -t upload                      └─────────┘

``````



### 4. Configuración WiFi (Primera vez)## 🔧 Instalación

1. Al encender, el ESP32 crea red `ESP32-Irrigation`

2. Conectar desde smartphone/PC### 1. Requisitos Previos

3. Se abre portal cautivo automáticamente

4. Seleccionar tu WiFi e ingresar contraseña- [Visual Studio Code](https://code.visualstudio.com/)

5. ¡Listo! Las credenciales se guardan permanentemente- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)

- Cable USB para ESP32

## 🚀 Uso- Arduino IDE (opcional, para drivers)



### Dashboard Web### 2. Clonar el Repositorio

1. Acceder a: `http://esp32-irrigation.local` (o IP del Serial Monitor)

2. Ver humedad de suelo en tiempo real```bash

3. Cambiar entre modos: AUTO / MANUAL / SCHEDULEDgit clone https://github.com/SEBASTIAN3451/ESP32-Smart-Irrigation-.git

4. En modo MANUAL: controlar bomba directamentecd ESP32-Smart-Irrigation-

5. Ver gráfico histórico de riegos (Chart.js)```

6. Exportar datos CSV

### 3. Configurar WiFi

### Configurar Horarios

```bashEdita `src/main.cpp` líneas 45-46:

# Ejemplo: Schedule #0 activado a las 7:00 AM, 15 segundos

curl -X POST "http://esp32-irrigation.local/api/schedules" \```cpp

  -d "id=0&h=7&m=0&d=15&e=1"const char* ssid = "TU_WIFI_SSID";

const char* password = "TU_WIFI_PASSWORD";

# Obtener horarios actuales```

curl http://esp32-irrigation.local/api/schedules

```### 4. Compilar y Subir



### Actualización OTA1. Abre el proyecto en VS Code

```bash2. PlatformIO: Build (✓)

# Desde PlatformIO3. PlatformIO: Upload (→)

pio run -t upload --upload-port esp32-irrigation.local4. PlatformIO: Serial Monitor (🔌)



# LED mostrará color magenta durante OTA### 5. Calibrar Sensor (Importante)

```

El sensor de humedad necesita calibración:

### Prometheus Integration

```yaml```cpp

scrape_configs:// En src/main.cpp líneas 52-53:

  - job_name: 'esp32-irrigation'#define MOISTURE_DRY 2800   // Valor cuando sensor está en aire (seco)

    static_configs:#define MOISTURE_WET 1500   // Valor cuando sensor está en agua (húmedo)

      - targets: ['esp32-irrigation.local:80']```

    metrics_path: '/metrics'

```**Cómo calibrar:**

1. Abre Serial Monitor

## 🎯 Modos de Operación2. Observa valor con sensor en aire → anota como MOISTURE_DRY

3. Observa valor con sensor en agua → anota como MOISTURE_WET

### 1. AUTO (Automático)4. Actualiza valores en el código

- Monitorea humedad cada 10 segundos

- Si `humedad_raw >= MOISTURE_DRY` (2800): activa bomba## 💻 Uso

- Riega hasta `humedad_raw <= MOISTURE_WET` (1500) o máx 5 segundos

- Registra evento en histórico### Acceder al Dashboard



### 2. MANUAL1. Conecta el ESP32 y abre Serial Monitor

- Control directo desde dashboard2. Anota la IP mostrada (ej: `192.168.1.100`)

- Presionar botón "💦 REGAR" / "⏹️ PARAR"3. En tu navegador: `http://192.168.1.100`

- Útil para pruebas o riego puntual

### Controles del Dashboard

### 3. SCHEDULED (Programado)

- Hasta 3 horarios independientes- **Barra de Humedad**: Muestra humedad actual (0-100%)

- Cada uno configurable: hora, minuto, duración (segundos)- **Indicador de Bomba**: Verde (apagada) / Azul pulsante (activa)

- Persistente en NVS (no se pierde al reiniciar)- **Modos**: Selecciona entre Automático, Manual o Programado

- Ejecuta riego exacto al horario programado- **Botones Manual**: Iniciar/Detener riego

- **Estadísticas**: Riegos totales, tiempo acumulado, promedio

## 📡 API Examples

### API REST

### Estado actual

```bash#### `GET /api/status`

curl http://esp32-irrigation.local/api/status

# {Obtiene estado completo del sistema

#   "moisture_raw": 2200,

#   "moisture_percent": 55,```json

#   "pump_active": false,{

#   "mode": "AUTO",  "moisture_percent": 45,

#   "watering_count": 12,  "moisture_raw": 2100,

#   "total_watering_time": 180  "pump_active": false,

# }  "mode": "Automático",

```  "watering_count": 12,

  "total_time": 360,

### Cambiar a modo AUTO  "uptime": 86400

```bash}

curl "http://esp32-irrigation.local/api/mode?m=AUTO"```

```

#### `POST /api/pump?action=on|off`

### Activar bomba (modo MANUAL)

```bashControl manual de la bomba

curl "http://esp32-irrigation.local/api/pump?state=1"

``````bash

curl -X POST http://192.168.1.100/api/pump?action=on

### Histórico de riegos```

```bash

curl http://esp32-irrigation.local/api/history#### `POST /api/mode?mode=auto|manual|scheduled`

# [

#   {"timestamp":1730000000,"duration":5,"moisture":2150},Cambiar modo de operación

#   {"timestamp":1730003600,"duration":4,"moisture":2300}

# ]```bash

```curl -X POST http://192.168.1.100/api/mode?mode=auto

```

### Export CSV

```bash#### `GET /api/schedule`

curl http://esp32-irrigation.local/api/export -o irrigation_log.csvObtiene los 3 horarios configurados

```

Respuesta:

### SSE Stream (JavaScript)```json

```javascript[

const es = new EventSource('http://esp32-irrigation.local/events');  {"index":0, "hour":7,  "minute":0,  "duration":10, "enabled":true},

es.addEventListener('update', (e) => {  {"index":1, "hour":14, "minute":0,  "duration":10, "enabled":false},

  const data = JSON.parse(e.data);  {"index":2, "hour":19, "minute":30, "duration":15, "enabled":true}

  console.log(`Humedad: ${data.moisture_percent}% | Bomba: ${data.pump_active}`);]

});```

```

#### `POST /api/schedule`

## 📊 ScreenshotsConfigura un horario. Parámetros (x-www-form-urlencoded): `index` (0..2), `hour` (0..23), `minute` (0..59), `duration` (segundos, >=1), `enabled` (true/false)



### Dashboard PrincipalEjemplo:

- Cards de humedad, estado bomba, modo activo, total riegos```bash

- Botones para cambiar modo (AUTO/MANUAL/SCHEDULED)curl -X POST \

- Control manual de bomba  -d "index=0" -d "hour=7" -d "minute=0" -d "duration=20" -d "enabled=true" \

- Gráfico Chart.js de histórico (barras de duración)  http://192.168.1.100/api/schedule

- Export CSV y acceso a Prometheus metrics```



### Indicadores LED RGB#### `GET /events`

| Color | Estado |Server‑Sent Events con evento "update" cuando cambia el estado.

|-------|--------|

| 🟢 Verde | Sistema OK / WiFi conectado |#### `GET /metrics`

| 🔵 Azul | Bomba activada (regando) |Métricas en formato Prometheus (`esp32_soil_moisture_percent`, `esp32_pump_active`).

| 🟣 Magenta | OTA en progreso |

| 🟠 Naranja | Inicializando |#### `GET /healthz`

Health check simple.

## 🔧 Configuración Avanzada

## 🎨 Indicador LED RGB

### Calibrar Sensor de Humedad

Editar `src/main.cpp`:| Color | Estado |

```cpp|-------|--------|

#define MOISTURE_DRY 2800   // Valor ADC suelo seco (ajustar según sensor)| 🟢 Verde | Sistema OK, WiFi conectado |

#define MOISTURE_WET 1500   // Valor ADC suelo húmedo| 🔵 Azul | Bomba activa, regando |

```| 🟡 Amarillo | Conectando a WiFi |

| 🔴 Rojo | Error de conexión |

**Cómo calibrar**:

1. Colocar sensor en suelo completamente seco → Leer valor en `/api/status` → Usar como `MOISTURE_DRY`## ⚙️ Configuración Avanzada

2. Regar hasta saturar → Leer valor → Usar como `MOISTURE_WET`

### Ajustar Umbrales de Humedad

### Ajustar Tiempos

```cppEn `src/main.cpp`:

#define PUMP_TIME_MS 5000     // Máximo tiempo de riego (ms)

#define CHECK_INTERVAL 10000  // Intervalo verificación (ms)```cpp

```// En función autoIrrigation():

if (soilMoisture < 30 && !pumpActive) {  // Cambiar 30

### Reset WiFi    // Iniciar riego

Mantener presionado botón BOOT al encender para entrar en modo configuración.}

else if (soilMoisture > 70 && pumpActive) {  // Cambiar 70

## 🧪 Troubleshooting    // Detener riego

}

| Problema | Solución |```

|----------|----------|

| Bomba no activa | Verificar relé (LED debe encender), fuente externa 5V |### Cambiar Intervalo de Monitoreo

| Humedad siempre 0% | Sensor invertido o desconectado (GPIO 34) |

| LED RGB no enciende | Verificar GPIO 5, nivel lógico 5V (usar level shifter si es necesario) |```cpp

| Schedules no persisten | Verificar partición NVS en platformio.ini |#define CHECK_INTERVAL 10000  // Milisegundos (10 segundos)

| OTA falla | Firewall bloquea puerto 3232, usar IP directa |```



## 📚 Stack Tecnológico### Modificar Tiempo Máximo de Riego



- **Framework**: Arduino (ESP32)```cpp

- **Build System**: PlatformIO// En función autoIrrigation():

- **Web Server**: ESPAsyncWebServer (async)if (pumpActive && (millis() - pumpStartTime > 30000)) {  // 30 segundos

- **JSON**: ArduinoJson v6```

- **Charts**: Chart.js 4.4

- **WiFi Config**: WiFiManager (tzapu)## 📊 Proyectos Futuros / Mejoras

- **OTA**: ArduinoOTA

- **Persistent Storage**: Preferences (NVS)- [ ] Implementar horarios programados funcionales

- **LED Control**: Adafruit NeoPixel- [ ] Almacenar datos en SD card o SPIFFS

- **Time Sync**: NTP (pool.ntp.org)- [ ] Gráficas históricas de humedad

- **mDNS**: ESP mDNS- [ ] Notificaciones push (Telegram/email)

- [ ] Sensor de nivel de agua del depósito

## 📁 Estructura del Proyecto- [ ] Control de múltiples zonas de riego

- [ ] Integración con Home Assistant

```- [ ] Predicción de riego con ML

ESP32_Smart_Irrigation/

├── src/## 🐛 Troubleshooting

│   ├── main.cpp              # Código principal v2.0 PRO

│   └── main_backup_v1.cpp    # Backup versión 1.0### La bomba no se activa

├── platformio.ini            # Configuración PlatformIO

└── README.md- ✓ Verifica alimentación del relé (5V)

```- ✓ Revisa conexión GPIO 25 → IN del relé

- ✓ Asegúrate que el relé es de 5V (no 3.3V)

## 🔄 Changelog- ✓ Prueba modo manual desde dashboard



### v2.0 PRO (Octubre 2025)### Lecturas erráticas del sensor

- ✅ OTA (ArduinoOTA) para updates remotos

- ✅ WiFiManager para config sin hardcode- ✓ Calibra el sensor correctamente

- ✅ Persistent schedules (NVS/Preferences)- ✓ Sensor capacitivo requiere contacto con tierra húmeda

- ✅ Historial 50 eventos con timestamps NTP- ✓ No uses sensor resistivo (se oxida rápido)

- ✅ Chart.js 4.4 embedded (gráfico barras duración)- ✓ Verifica conexión a GPIO 34

- ✅ Export CSV de log de riegos

- ✅ Prometheus metrics endpoint### LED RGB no funciona

- ✅ mDNS (esp32-irrigation.local)

- ✅ SSE para tiempo real- ✓ Verifica que es WS2812B (NeoPixel)

- ✅ Estadísticas completas (total riegos, tiempo acumulado)- ✓ Alimenta el LED con 5V (no 3.3V)

- ✅ API REST completa con todos los endpoints- ✓ GPIO 5 → DIN del LED

- ✓ Prueba reducir brillo en código

### v1.0 (Inicial)

- Monitoreo básico humedad### WiFi no conecta

- Modos AUTO/MANUAL/SCHEDULED

- Dashboard web simple- ✓ Verifica SSID y contraseña

- LED RGB indicador- ✓ Red debe ser 2.4GHz

- ✓ Verifica señal WiFi fuerte

## 🌱 Casos de Uso

## 📚 Librerías Utilizadas

- **Huertos Urbanos**: Riego automático durante vacaciones

- **Invernaderos**: Monitoreo continuo y horarios precisos- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Servidor web

- **IoT/Agriculture**: Base para sistemas de agricultura de precisión- [ArduinoJson](https://arduinojson.org/) - Manejo de JSON

- **Educación**: Proyecto didáctico ESP32 + sensores + actuadores- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) - Control LED RGB

- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) - TCP asíncrono

## 🤝 Contribuciones

## 🔐 Seguridad

¡Pull requests bienvenidos! Para cambios mayores, abrir issue primero.

⚠️ **Importante**: Este proyecto es para uso educativo/doméstico.

## 📄 Licencia

- No exponer directamente a internet sin autenticación

MIT License - Ver archivo LICENSE- Usar contraseñas WiFi fuertes

- Considerar VPN para acceso remoto

## 👨‍💻 Autor- Supervisar primer uso para evitar inundaciones



**Sebastian Lara**## 🤝 Contribuciones

- GitHub: [@SEBASTIAN3451](https://github.com/SEBASTIAN3451)

- Proyecto: [ESP32-Smart-Irrigation-](https://github.com/SEBASTIAN3451/ESP32-Smart-Irrigation-)¡Contribuciones bienvenidas! Por favor:



---1. Fork el proyecto

2. Crea tu Feature Branch (`git checkout -b feature/MejorRiego`)

⭐ Si este proyecto te fue útil, dale una estrella en GitHub!3. Commit cambios (`git commit -m 'Agregar predicción de riego'`)

4. Push al Branch (`git push origin feature/MejorRiego`)
5. Abre un Pull Request

## 📝 Licencia

MIT License - Ver [LICENSE](LICENSE) para detalles.

## 👤 Autor

**Sebastian Lara**

- GitHub: [@SEBASTIAN3451](https://github.com/SEBASTIAN3451)
- LinkedIn: [Tu Perfil](https://linkedin.com/in/tu-perfil)

## 🌟 Proyectos Relacionados

- [ESP32 Temperature Monitor](https://github.com/SEBASTIAN3451/ESP32-Temperature-Monitor-)
- [Smart Agriculture Platform](https://github.com/SEBASTIAN3451/otro-proyecto)

## 📖 Referencias

- [ESP32 ADC Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
- [Soil Moisture Sensor Guide](https://www.example.com)
- [IoT Best Practices](https://www.example.com)

---

⭐ Si este proyecto te ayudó, dale una estrella en GitHub!

**Hecho con ❤️ para un futuro más verde 🌱**
