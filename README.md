# 🌱 ESP32 Smart Irrigation System

![ESP32](https://img.shields.io/badge/ESP32-DevKit-blue)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange)
![License](https://img.shields.io/badge/License-MIT-green)
![IoT](https://img.shields.io/badge/IoT-Agriculture-brightgreen)

Sistema inteligente de riego automático para plantas con monitoreo de humedad del suelo, control de bomba de agua y tres modos de operación: Automático, Manual y Programado.

## 📸 Preview

![Dashboard](https://via.placeholder.com/800x400/667eea/ffffff?text=Smart+Irrigation+Dashboard)

## ✨ Características Principales

- 💧 **Riego Automático Inteligente**: Riega solo cuando el suelo lo necesita
- 📊 **Monitoreo en Tiempo Real**: Visualiza la humedad del suelo constantemente
- 🎮 **3 Modos de Operación**: Automático, Manual y Programado
- 📱 **Dashboard Web Responsive**: Control desde cualquier dispositivo
- 🔔 **Indicador LED RGB**: Estado visual del sistema
- 📈 **Estadísticas**: Historial de riegos y tiempo total
- ⏱️ **Programación Horaria**: Configura horarios automáticos (próximamente)
- 🛡️ **Protección**: Límite de tiempo máximo de riego (seguridad)

## 🚀 Funcionalidades

### Modo Automático ⚡
- Monitorea la humedad del suelo cada 10 segundos
- Activa riego cuando humedad < 30%
- Detiene riego cuando humedad > 70%
- Límite de seguridad: máximo 30 segundos de riego continuo

### Modo Manual 🎮
- Control total desde el dashboard
- Botones de inicio/detener riego
- Ideal para pruebas o riegos puntuales

### Modo Programado ⏰
- 3 horarios configurables
- Duración personalizable por horario
- Activación/desactivación individual

## 🛠️ Hardware Requerido

| Componente | Cantidad | Especificaciones | Precio aprox. |
|------------|----------|------------------|---------------|
| ESP32 DevKit | 1 | ESP32-WROOM-32 | $5-10 USD |
| Sensor de Humedad Capacitivo | 1 | V1.2 o superior | $2-5 USD |
| Módulo Relé 5V | 1 | 1 canal, optoacoplado | $1-3 USD |
| LED RGB WS2812B | 1 | NeoPixel compatible | $0.50 USD |
| Bomba de Agua 5V | 1 | Mini bomba sumergible | $3-5 USD |
| Fuente 5V 2A | 1 | Para bomba y ESP32 | $3-5 USD |
| Cables Jumper | varios | Macho-Hembra | $2 USD |

**Total estimado**: ~$20-30 USD

## 📐 Diagrama de Conexión

```
ESP32           Componente
-----           ----------
GPIO 34  <-->  Sensor Humedad (A0/OUT)
GPIO 25  <-->  Relé (IN)
GPIO 5   <-->  LED RGB (DIN)
3.3V     <-->  Sensor Humedad (VCC)
5V       <-->  Relé (VCC), LED (VCC)
GND      <-->  GND común

Relé            Bomba
----            -----
COM      <-->  VCC Bomba
NO       <-->  (+) Fuente 5V
GND      <-->  GND Bomba
```

### Esquema Visual

```
                    ┌──────────────┐
                    │   ESP32      │
                    │   DevKit     │
                    └──────┬───────┘
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
   ┌────▼────┐        ┌────▼────┐       ┌────▼────┐
   │ Sensor  │        │  Relé   │       │ LED RGB │
   │ Humedad │        │   5V    │       │ WS2812B │
   └─────────┘        └────┬────┘       └─────────┘
                           │
                      ┌────▼────┐
                      │  Bomba  │
                      │   5V    │
                      └─────────┘
```

## 🔧 Instalación

### 1. Requisitos Previos

- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO IDE](https://platformio.org/install/ide?install=vscode)
- Cable USB para ESP32
- Arduino IDE (opcional, para drivers)

### 2. Clonar el Repositorio

```bash
git clone https://github.com/TU_USUARIO/ESP32_Smart_Irrigation.git
cd ESP32_Smart_Irrigation
```

### 3. Configurar WiFi

Edita `src/main.cpp` líneas 45-46:

```cpp
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";
```

### 4. Compilar y Subir

1. Abre el proyecto en VS Code
2. PlatformIO: Build (✓)
3. PlatformIO: Upload (→)
4. PlatformIO: Serial Monitor (🔌)

### 5. Calibrar Sensor (Importante)

El sensor de humedad necesita calibración:

```cpp
// En src/main.cpp líneas 52-53:
#define MOISTURE_DRY 2800   // Valor cuando sensor está en aire (seco)
#define MOISTURE_WET 1500   // Valor cuando sensor está en agua (húmedo)
```

**Cómo calibrar:**
1. Abre Serial Monitor
2. Observa valor con sensor en aire → anota como MOISTURE_DRY
3. Observa valor con sensor en agua → anota como MOISTURE_WET
4. Actualiza valores en el código

## 💻 Uso

### Acceder al Dashboard

1. Conecta el ESP32 y abre Serial Monitor
2. Anota la IP mostrada (ej: `192.168.1.100`)
3. En tu navegador: `http://192.168.1.100`

### Controles del Dashboard

- **Barra de Humedad**: Muestra humedad actual (0-100%)
- **Indicador de Bomba**: Verde (apagada) / Azul pulsante (activa)
- **Modos**: Selecciona entre Automático, Manual o Programado
- **Botones Manual**: Iniciar/Detener riego
- **Estadísticas**: Riegos totales, tiempo acumulado, promedio

### API REST

#### `GET /api/status`

Obtiene estado completo del sistema

```json
{
  "moisture_percent": 45,
  "moisture_raw": 2100,
  "pump_active": false,
  "mode": "Automático",
  "watering_count": 12,
  "total_time": 360,
  "uptime": 86400
}
```

#### `POST /api/pump?action=on|off`

Control manual de la bomba

```bash
curl -X POST http://192.168.1.100/api/pump?action=on
```

#### `POST /api/mode?mode=auto|manual|scheduled`

Cambiar modo de operación

```bash
curl -X POST http://192.168.1.100/api/mode?mode=auto
```

## 🎨 Indicador LED RGB

| Color | Estado |
|-------|--------|
| 🟢 Verde | Sistema OK, WiFi conectado |
| 🔵 Azul | Bomba activa, regando |
| 🟡 Amarillo | Conectando a WiFi |
| 🔴 Rojo | Error de conexión |

## ⚙️ Configuración Avanzada

### Ajustar Umbrales de Humedad

En `src/main.cpp`:

```cpp
// En función autoIrrigation():
if (soilMoisture < 30 && !pumpActive) {  // Cambiar 30
    // Iniciar riego
}
else if (soilMoisture > 70 && pumpActive) {  // Cambiar 70
    // Detener riego
}
```

### Cambiar Intervalo de Monitoreo

```cpp
#define CHECK_INTERVAL 10000  // Milisegundos (10 segundos)
```

### Modificar Tiempo Máximo de Riego

```cpp
// En función autoIrrigation():
if (pumpActive && (millis() - pumpStartTime > 30000)) {  // 30 segundos
```

## 📊 Proyectos Futuros / Mejoras

- [ ] Implementar horarios programados funcionales
- [ ] Almacenar datos en SD card o SPIFFS
- [ ] Gráficas históricas de humedad
- [ ] Notificaciones push (Telegram/email)
- [ ] Sensor de nivel de agua del depósito
- [ ] Control de múltiples zonas de riego
- [ ] Integración con Home Assistant
- [ ] Predicción de riego con ML

## 🐛 Troubleshooting

### La bomba no se activa

- ✓ Verifica alimentación del relé (5V)
- ✓ Revisa conexión GPIO 25 → IN del relé
- ✓ Asegúrate que el relé es de 5V (no 3.3V)
- ✓ Prueba modo manual desde dashboard

### Lecturas erráticas del sensor

- ✓ Calibra el sensor correctamente
- ✓ Sensor capacitivo requiere contacto con tierra húmeda
- ✓ No uses sensor resistivo (se oxida rápido)
- ✓ Verifica conexión a GPIO 34

### LED RGB no funciona

- ✓ Verifica que es WS2812B (NeoPixel)
- ✓ Alimenta el LED con 5V (no 3.3V)
- ✓ GPIO 5 → DIN del LED
- ✓ Prueba reducir brillo en código

### WiFi no conecta

- ✓ Verifica SSID y contraseña
- ✓ Red debe ser 2.4GHz
- ✓ Verifica señal WiFi fuerte

## 📚 Librerías Utilizadas

- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) - Servidor web
- [ArduinoJson](https://arduinojson.org/) - Manejo de JSON
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel) - Control LED RGB
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) - TCP asíncrono

## 🔐 Seguridad

⚠️ **Importante**: Este proyecto es para uso educativo/doméstico.

- No exponer directamente a internet sin autenticación
- Usar contraseñas WiFi fuertes
- Considerar VPN para acceso remoto
- Supervisar primer uso para evitar inundaciones

## 🤝 Contribuciones

¡Contribuciones bienvenidas! Por favor:

1. Fork el proyecto
2. Crea tu Feature Branch (`git checkout -b feature/MejorRiego`)
3. Commit cambios (`git commit -m 'Agregar predicción de riego'`)
4. Push al Branch (`git push origin feature/MejorRiego`)
5. Abre un Pull Request

## 📝 Licencia

MIT License - Ver [LICENSE](LICENSE) para detalles.

## 👤 Autor

**Sebastian Lara**

- GitHub: [@TU_USUARIO](https://github.com/TU_USUARIO)
- LinkedIn: [Tu Perfil](https://linkedin.com/in/tu-perfil)

## 🌟 Proyectos Relacionados

- [ESP32 Temperature Monitor](https://github.com/TU_USUARIO/ESP32_Temperature_Monitor)
- [Smart Agriculture Platform](https://github.com/TU_USUARIO/otro-proyecto)

## 📖 Referencias

- [ESP32 ADC Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html)
- [Soil Moisture Sensor Guide](https://www.example.com)
- [IoT Best Practices](https://www.example.com)

---

⭐ Si este proyecto te ayudó, dale una estrella en GitHub!

**Hecho con ❤️ para un futuro más verde 🌱**
