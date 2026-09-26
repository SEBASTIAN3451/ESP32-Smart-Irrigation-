# ESP32 Smart Irrigation System 💧🌱

> Sistema de riego inteligente basado en ESP32 con monitoreo de humedad del suelo, control de bomba y tres modos de operación.

![ESP32](https://img.shields.io/badge/ESP32-DevKit-E7352C?logo=espressif&logoColor=white)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-orange?logo=platformio)
![C++](https://img.shields.io/badge/C%2B%2B-Arduino-00599C?logo=cplusplus&logoColor=white)
![IoT](https://img.shields.io/badge/IoT-Smart%20Agriculture-2ea44f)

## 📌 Descripción

Este proyecto utiliza un **ESP32** para automatizar el riego de una planta o cultivo a partir de la humedad del suelo.

El sistema incorpora un servidor web para consultar el estado y controlar el riego, además de modos automático, manual y programado.

## ✨ Funcionalidades

- 💧 Riego automático según humedad.
- 🎮 Control manual de la bomba.
- ⏰ Hasta 3 horarios de riego programables.
- 💾 Persistencia de horarios mediante NVS.
- 📊 Dashboard web.
- ⚡ Actualización en tiempo real mediante SSE.
- 📈 Historial de eventos de riego.
- 📤 Exportación del historial a CSV.
- 🌐 Acceso mediante mDNS.
- 📡 API REST.
- 📊 Métricas Prometheus.
- 🔄 Actualización OTA.
- 🛡️ Tiempo máximo de riego para reducir el riesgo de inundación.

## 🔄 Modos de operación

### AUTO

El ESP32 supervisa la humedad y activa la bomba cuando el valor se encuentra por debajo del umbral configurado. El riego se detiene al alcanzar el nivel objetivo o el límite de seguridad.

### MANUAL

Permite encender y detener la bomba directamente desde el dashboard.

### SCHEDULED

Permite configurar horarios de riego. Los horarios se almacenan en memoria no volátil para conservar la configuración después de reiniciar el ESP32.

## 🏗️ Arquitectura

```text
┌─────────────────┐
│ Sensor humedad  │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│      ESP32      │
│                 │
│ Control + Wi-Fi │
│ Web Server      │
└───────┬─────────┘
        │
   ┌────┴─────┐
   ▼          ▼
Dashboard    Relé
Web          │
             ▼
          Bomba
```

## 🔌 Conexiones

| Componente | Pin |
|---|---|
| Sensor de humedad | GPIO 34 (ADC) |
| Relé | GPIO 25 |
| LED RGB WS2812B | GPIO 5 |
| ESP32 | Control principal |

> **Importante:** la bomba debe utilizar una alimentación adecuada y no debe alimentarse directamente desde un GPIO del ESP32.

## 🛠️ Hardware

- ESP32 DevKit
- Sensor capacitivo de humedad
- Módulo relé
- Bomba de agua o electroválvula
- LED RGB WS2812B (opcional)
- Fuente externa adecuada para la bomba
- Cables y protoboard

## 🚀 Instalación

### 1. Clonar

```bash
git clone https://github.com/SEBASTIAN3451/ESP32-Smart-Irrigation-.git
cd ESP32-Smart-Irrigation-
```

### 2. Abrir con PlatformIO

Abre el proyecto en VS Code con PlatformIO.

### 3. Configurar Wi-Fi

Configura las credenciales según la implementación actual del firmware.

Si utilizas el portal de configuración WiFiManager, completa la configuración desde el dispositivo cuando sea necesario.

### 4. Calibrar el sensor

Los valores del sensor dependen del hardware y del suelo. Realiza una calibración en condiciones secas y húmedas antes de definir los umbrales de riego.

### 5. Compilar y cargar

```bash
pio run
pio run -t upload
```

## 🌐 Dashboard

Después de conectar el ESP32 a la red, accede mediante la IP mostrada en el monitor serial o mediante el nombre mDNS configurado por el proyecto.

El dashboard permite:

- Consultar humedad.
- Ver el estado de la bomba.
- Cambiar el modo de operación.
- Activar o detener el riego.
- Configurar horarios.
- Consultar estadísticas.
- Exportar el historial.

## 📡 API

| Método | Endpoint | Descripción |
|---|---|---|
| GET | `/api/status` | Estado actual |
| GET | `/api/history` | Historial de riegos |
| GET | `/api/export` | Exportación CSV |
| GET/POST | `/api/schedules` | Consultar/guardar horarios |
| GET/POST | `/api/mode` | Cambiar modo |
| GET/POST | `/api/pump` | Control de bomba |
| GET | `/metrics` | Métricas Prometheus |
| GET | `/healthz` | Health check |
| GET | `/events` | Stream SSE |

> Los métodos exactos aceptados por algunos endpoints dependen de la versión del firmware presente en el repositorio.

## 📁 Estructura

```text
ESP32-Smart-Irrigation-/
├── src/
│   └── main.cpp
├── platformio.ini
└── README.md
```

## 🧰 Stack

- **ESP32**
- **C++ / Arduino**
- **PlatformIO**
- **Sensor capacitivo de humedad**
- **Wi-Fi**
- **ESPAsyncWebServer**
- **ArduinoJson**
- **Chart.js**
- **WiFiManager**
- **ArduinoOTA**
- **NVS**
- **Server-Sent Events**
- **mDNS**

## 🔐 Seguridad y buenas prácticas

- Utiliza una fuente independiente y adecuada para la bomba.
- No conectes cargas de potencia directamente a los GPIO.
- Configura límites máximos de tiempo de riego.
- No expongas el servidor directamente a Internet sin autenticación y medidas de seguridad.
- Prueba primero el sistema sin conectar una bomba de potencia.

## 👨‍💻 Autor

**Sebastian Lara**  
Ingeniería Electrónica · IoT · Software

- GitHub: [@SEBASTIAN3451](https://github.com/SEBASTIAN3451)

## 📄 Licencia

MIT License.
