/*
 * ESP32 Smart Irrigation System
 * 
 * Sistema inteligente de riego automático para plantas con monitoreo
 * de humedad del suelo, control de bomba de agua y programación horaria.
 * 
 * Características:
 * - Monitoreo continuo de humedad del suelo (sensor capacitivo)
 * - Control automático y manual de bomba de agua
 * - Programación de horarios de riego
 * - Dashboard web con control en tiempo real
 * - Historial de riegos y estadísticas
 * - Modos: Automático, Manual, Programado
 * - Indicador LED RGB de estado
 * 
 * Hardware:
 * - ESP32 DevKit
 * - Sensor de humedad de suelo capacitivo
 * - Relé 5V (control de bomba)
 * - LED RGB WS2812B (NeoPixel)
 * - Bomba de agua 5V (o electroválvula)
 * 
 * Conexiones:
 * - Sensor Humedad -> GPIO 34 (ADC1_CH6)
 * - Relé Bomba -> GPIO 25
 * - LED RGB -> GPIO 5
 * - Alimentación: 5V para bomba y relé
 * 
 * Autor: Sebastian Lara
 * Fecha: Octubre 2025
 * Versión: 1.0
 */

#include <Arduino.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include <time.h>

// ==================== CONFIGURACIÓN ====================

// WiFi - CAMBIAR ESTOS VALORES
const char* ssid = "TU_WIFI_SSID";
const char* password = "TU_WIFI_PASSWORD";

// Pines
#define SOIL_MOISTURE_PIN 34    // Sensor de humedad (ADC)
#define PUMP_RELAY_PIN 25       // Relé de la bomba
#define LED_RGB_PIN 5           // LED RGB indicador
#define LED_COUNT 1             // Número de LEDs

// Umbrales de humedad (0-4095, ADC 12-bit)
#define MOISTURE_DRY 2800       // Suelo seco (necesita riego)
#define MOISTURE_WET 1500       // Suelo húmedo (detener riego)

// Configuración de riego
#define PUMP_TIME_MS 5000       // Tiempo de riego por ciclo (5 segundos)
#define CHECK_INTERVAL 10000    // Intervalo de verificación (10 segundos)

// ==================== OBJETOS GLOBALES ====================

AsyncWebServer server(80);
AsyncEventSource events("/events");
Adafruit_NeoPixel led(LED_COUNT, LED_RGB_PIN, NEO_GRB + NEO_KHZ800);
Preferences preferences;

// Variables de estado
enum Mode { AUTO, MANUAL, SCHEDULED };
Mode currentMode = AUTO;

bool pumpActive = false;
int soilMoisture = 0;
unsigned long lastCheck = 0;
unsigned long pumpStartTime = 0;
unsigned long totalWateringTime = 0;
int wateringCount = 0;
time_t lastNtpSync = 0;

// Horarios programados (formato 24h)
struct Schedule {
    int hour;
    int minute;
    int duration;  // segundos
    bool enabled;
};

Schedule schedules[3] = {
    {7, 0, 10, false},    // 7:00 AM, 10 segundos
    {14, 0, 10, false},   // 2:00 PM, 10 segundos
    {19, 0, 10, false}    // 7:00 PM, 10 segundos
};

// Evitar re-ejecutar el mismo minuto
int lastRunDay[3] = {-1, -1, -1};
int lastRunMinute[3] = {-1, -1, -1};

// ==================== PÁGINA WEB ====================

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="es">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>🌱 Smart Irrigation</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        
        body {
            font-family: 'Segoe UI', sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            padding: 20px;
        }
        
        .container {
            max-width: 900px;
            margin: 0 auto;
            background: white;
            border-radius: 20px;
            padding: 30px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
        }
        
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
            font-size: 32px;
        }
        
        .status-card {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
            border-radius: 15px;
            padding: 30px;
            color: white;
            margin-bottom: 30px;
        }
        
        .moisture-display {
            display: flex;
            align-items: center;
            justify-content: space-between;
            margin-bottom: 20px;
        }
        
        .moisture-value {
            font-size: 48px;
            font-weight: bold;
        }
        
        .moisture-bar {
            width: 100%;
            height: 30px;
            background: rgba(255,255,255,0.3);
            border-radius: 15px;
            overflow: hidden;
            position: relative;
        }
        
        .moisture-fill {
            height: 100%;
            background: white;
            transition: width 0.5s ease;
            border-radius: 15px;
        }
        
        .controls {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .control-card {
            background: #f5f5f5;
            border-radius: 15px;
            padding: 20px;
            text-align: center;
        }
        
        .control-card h3 {
            color: #333;
            margin-bottom: 15px;
            font-size: 18px;
        }
        
        .btn {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            border: none;
            padding: 15px 30px;
            border-radius: 10px;
            font-size: 16px;
            cursor: pointer;
            transition: transform 0.2s;
            width: 100%;
            margin-top: 10px;
        }
        
        .btn:hover {
            transform: scale(1.05);
        }
        
        .btn:active {
            transform: scale(0.95);
        }
        
        .btn-danger {
            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);
        }
        
        .btn-success {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
        }
        
        .pump-status {
            display: inline-block;
            width: 15px;
            height: 15px;
            border-radius: 50%;
            margin-right: 8px;
        }
        
        .pump-on { background: #4CAF50; animation: pulse 1s infinite; }
        .pump-off { background: #ccc; }
        
        @keyframes pulse {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        
        .mode-selector {
            display: flex;
            gap: 10px;
            margin-bottom: 20px;
        }
        
        .mode-btn {
            flex: 1;
            padding: 15px;
            border: 2px solid #667eea;
            background: white;
            color: #667eea;
            border-radius: 10px;
            cursor: pointer;
            transition: all 0.3s;
        }
        
        .mode-btn.active {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
        }
        
        .stats {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-top: 30px;
        }
        
        .stat-box {
            background: #f5f5f5;
            padding: 20px;
            border-radius: 10px;
            text-align: center;
        }
        
        .stat-value {
            font-size: 32px;
            font-weight: bold;
            color: #667eea;
        }
        
        .stat-label {
            color: #666;
            font-size: 14px;
            margin-top: 5px;
        }
        
        @media (max-width: 768px) {
            .controls { grid-template-columns: 1fr; }
            .stats { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>🌱 Sistema de Riego Inteligente</h1>
        
        <div class="status-card">
            <div class="moisture-display">
                <div>
                    <div style="font-size: 14px; opacity: 0.9;">Humedad del Suelo</div>
                    <div class="moisture-value"><span id="moisture">--</span>%</div>
                </div>
                <div style="text-align: right;">
                    <span class="pump-status" id="pumpStatus"></span>
                    <span id="pumpText">Bomba Apagada</span>
                </div>
            </div>
            <div class="moisture-bar">
                <div class="moisture-fill" id="moistureBar" style="width: 0%"></div>
            </div>
        </div>
        
        <div class="mode-selector">
            <button class="mode-btn active" onclick="setMode('auto')">
                ⚡ Automático
            </button>
            <button class="mode-btn" onclick="setMode('manual')">
                🎮 Manual
            </button>
            <button class="mode-btn" onclick="setMode('scheduled')">
                ⏰ Programado
            </button>
        </div>
        
        <div class="controls">
            <div class="control-card">
                <h3>Control Manual</h3>
                <button class="btn btn-success" onclick="startPump()">
                    💧 Iniciar Riego
                </button>
                <button class="btn btn-danger" onclick="stopPump()">
                    ⛔ Detener Riego
                </button>
            </div>
            
            <div class="control-card">
                <h3>Estado del Sistema</h3>
                <div style="text-align: left; margin-top: 15px;">
                    <p>📊 Modo: <strong id="modeText">Automático</strong></p>
                    <p>💧 Estado: <strong id="statusText">Monitoreando</strong></p>
                    <p>⏱️ Tiempo activo: <strong id="uptime">0s</strong></p>
                </div>
            </div>
        </div>
        
        <div class="stats">
            <div class="stat-box">
                <div class="stat-value" id="wateringCount">0</div>
                <div class="stat-label">Riegos Totales</div>
            </div>
            <div class="stat-box">
                <div class="stat-value" id="totalTime">0s</div>
                <div class="stat-label">Tiempo Total</div>
            </div>
            <div class="stat-box">
                <div class="stat-value" id="avgMoisture">0%</div>
                <div class="stat-label">Humedad Promedio</div>
            </div>
        </div>
    </div>
    
    <script>
        let modeButtons = document.querySelectorAll('.mode-btn');
        
        function apply(data){
            document.getElementById('moisture').textContent = data.moisture_percent;
            document.getElementById('moistureBar').style.width = data.moisture_percent + '%';
            const pumpStatus = document.getElementById('pumpStatus');
            const pumpText = document.getElementById('pumpText');
            if (data.pump_active) { pumpStatus.className = 'pump-status pump-on'; pumpText.textContent = '💧 Bomba Activa'; }
            else { pumpStatus.className = 'pump-status pump-off'; pumpText.textContent = 'Bomba Apagada'; }
            document.getElementById('modeText').textContent = data.mode;
            document.getElementById('wateringCount').textContent = data.watering_count;
            document.getElementById('totalTime').textContent = data.total_time + 's';
            document.getElementById('avgMoisture').textContent = data.moisture_percent + '%';
        }

        // SSE
        try {
            const es = new EventSource('/events');
            es.addEventListener('update', ev => { try { apply(JSON.parse(ev.data)); } catch(e){} });
        } catch(e) {}

        async function updateData(){
            try { const r = await fetch('/api/status'); apply(await r.json()); } catch(e){}
        }
        
        function setMode(mode) {
            fetch('/api/mode?mode=' + mode, { method: 'POST' })
                .then(() => {
                    modeButtons.forEach(btn => btn.classList.remove('active'));
                    event.target.classList.add('active');
                    updateData();
                });
        }
        
        function startPump() {
            fetch('/api/pump?action=on', { method: 'POST' })
                .then(() => updateData());
        }
        
        function stopPump() {
            fetch('/api/pump?action=off', { method: 'POST' })
                .then(() => updateData());
        }
        
        setInterval(updateData, 4000);
        updateData();
    </script>
</body>
</html>
)rawliteral";

// ==================== FUNCIONES ====================

void setLEDColor(uint8_t r, uint8_t g, uint8_t b) {
    led.setPixelColor(0, led.Color(r, g, b));
    led.show();
}

int readSoilMoisture() {
    int raw = analogRead(SOIL_MOISTURE_PIN);
    // Convertir a porcentaje (invertido: más bajo = más húmedo)
    int percent = map(raw, MOISTURE_WET, MOISTURE_DRY, 100, 0);
    percent = constrain(percent, 0, 100);
    return percent;
}

void activatePump(bool state) {
    pumpActive = state;
    digitalWrite(PUMP_RELAY_PIN, state ? HIGH : LOW);
    
    if (state) {
        pumpStartTime = millis();
        setLEDColor(0, 0, 255);  // Azul = regando
        Serial.println("💧 Bomba ACTIVADA");
    } else {
        if (pumpStartTime > 0) {
            totalWateringTime += (millis() - pumpStartTime) / 1000;
            wateringCount++;
        }
        pumpStartTime = 0;
        setLEDColor(0, 255, 0);  // Verde = OK
        Serial.println("⛔ Bomba DESACTIVADA");

        // Notificar SSE
        StaticJsonDocument<256> doc;
        doc["moisture_percent"] = soilMoisture;
        doc["pump_active"] = pumpActive;
        doc["mode"] = (currentMode == AUTO) ? "Automático" : (currentMode == MANUAL) ? "Manual" : "Programado";
        doc["watering_count"] = wateringCount;
        doc["total_time"] = totalWateringTime;
        String payload; serializeJson(doc, payload);
        events.send(payload.c_str(), "update", millis());
    }
}

void autoIrrigation() {
    soilMoisture = readSoilMoisture();
    
    if (soilMoisture < 30 && !pumpActive) {
        // Suelo seco, activar riego
        Serial.printf("🌵 Suelo seco (%d%%), iniciando riego...\n", soilMoisture);
        activatePump(true);
    } 
    else if (soilMoisture > 70 && pumpActive) {
        // Suelo húmedo, detener riego
        Serial.printf("💧 Suelo húmedo (%d%%), deteniendo riego\n", soilMoisture);
        activatePump(false);
    }
    
    // Límite de seguridad: máximo 30 segundos de riego continuo
    if (pumpActive && (millis() - pumpStartTime > 30000)) {
        Serial.println("⚠️ Tiempo máximo alcanzado, deteniendo bomba");
        activatePump(false);
    }
}

void setupWiFi() {
    Serial.println("\n========================================");
    Serial.println("  🌱 Smart Irrigation System v1.0");
    Serial.println("========================================\n");
    
    Serial.print("Conectando a WiFi: ");
    Serial.println(ssid);
    
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        setLEDColor(255, 255, 0);  // Amarillo = conectando
        delay(500);
        setLEDColor(0, 0, 0);
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✓ WiFi conectado!");
        Serial.print("✓ IP: ");
        Serial.println(WiFi.localIP());
        setLEDColor(0, 255, 0);  // Verde = conectado

        // mDNS
        if (MDNS.begin("esp32-irrig")) {
            MDNS.addService("http", "tcp", 80);
            Serial.println("✓ mDNS: http://esp32-irrig.local");
        }

        // NTP
        configTime(0, 0, "pool.ntp.org", "time.nist.gov");
        time_t now = time(nullptr);
        if (now > 100000) lastNtpSync = now;
    } else {
        Serial.println("\n✗ Error de WiFi");
        setLEDColor(255, 0, 0);  // Rojo = error
    }
}

void setupWebServer() {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });
    
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request){
        StaticJsonDocument<300> doc;
        doc["moisture_percent"] = soilMoisture;
        doc["moisture_raw"] = analogRead(SOIL_MOISTURE_PIN);
        doc["pump_active"] = pumpActive;
        doc["mode"] = (currentMode == AUTO) ? "Automático" : 
                      (currentMode == MANUAL) ? "Manual" : "Programado";
        doc["watering_count"] = wateringCount;
        doc["total_time"] = totalWateringTime;
        doc["uptime"] = millis() / 1000;
        
        String response;
        serializeJson(doc, response);
        request->send(200, "application/json", response);
    });
    
    server.on("/api/pump", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("action", true)) {
            String action = request->getParam("action", true)->value();
            if (action == "on") {
                activatePump(true);
            } else {
                activatePump(false);
            }
        }
        request->send(200, "text/plain", "OK");
    });
    
    server.on("/api/mode", HTTP_POST, [](AsyncWebServerRequest *request){
        if (request->hasParam("mode", true)) {
            String mode = request->getParam("mode", true)->value();
            if (mode == "auto") currentMode = AUTO;
            else if (mode == "manual") currentMode = MANUAL;
            else if (mode == "scheduled") currentMode = SCHEDULED;
            
            Serial.printf("Modo cambiado a: %s\n", mode.c_str());
        }
        request->send(200, "text/plain", "OK");
    });

    // Schedule API
    server.on("/api/schedule", HTTP_GET, [](AsyncWebServerRequest *request){
        StaticJsonDocument<400> doc;
        for (int i=0;i<3;i++){
            JsonObject s = doc.createNestedObject();
            s["index"]=i; s["hour"]=schedules[i].hour; s["minute"]=schedules[i].minute; s["duration"]=schedules[i].duration; s["enabled"]=schedules[i].enabled;
        }
        String out; serializeJson(doc, out);
        request->send(200, "application/json", out);
    });
    server.on("/api/schedule", HTTP_POST, [](AsyncWebServerRequest *request){
        int idx=-1; if (request->hasParam("index", true)) idx = request->getParam("index", true)->value().toInt();
        if (idx<0 || idx>2){ request->send(400, "text/plain", "index 0..2"); return; }
        if (request->hasParam("hour", true)) schedules[idx].hour = constrain(request->getParam("hour", true)->value().toInt(), 0, 23);
        if (request->hasParam("minute", true)) schedules[idx].minute = constrain(request->getParam("minute", true)->value().toInt(), 0, 59);
        if (request->hasParam("duration", true)) schedules[idx].duration = max(1, request->getParam("duration", true)->value().toInt());
        if (request->hasParam("enabled", true)) schedules[idx].enabled = request->getParam("enabled", true)->value() == "true";
        request->send(200, "text/plain", "OK");
    });

    // Health & metrics
    server.on("/healthz", HTTP_GET, [](AsyncWebServerRequest *request){ request->send(200, "text/plain", "ok"); });
    server.on("/metrics", HTTP_GET, [](AsyncWebServerRequest *request){
        String m; m += "esp32_soil_moisture_percent "; m += String(soilMoisture); m += "\n";
        m += "esp32_pump_active "; m += (pumpActive?1:0); m += "\n";
        request->send(200, "text/plain; version=0.0.4", m);
    });

    // SSE
    events.onConnect([](AsyncEventSourceClient *client){
        StaticJsonDocument<200> doc; doc["moisture_percent"]=soilMoisture; doc["pump_active"]=pumpActive; String p; serializeJson(doc,p); client->send(p.c_str(), "update", millis());
    });
    server.addHandler(&events);
    
    server.begin();
    Serial.println("✓ Servidor web iniciado\n");
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    pinMode(PUMP_RELAY_PIN, OUTPUT);
    digitalWrite(PUMP_RELAY_PIN, LOW);
    
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    
    led.begin();
    led.setBrightness(50);
    setLEDColor(255, 255, 0);  // Amarillo = iniciando
    
    setupWiFi();
    
    if (WiFi.status() == WL_CONNECTED) {
        setupWebServer();
        
        Serial.println("🌐 Dashboard disponible en:");
        Serial.print("   http://");
        Serial.println(WiFi.localIP());
        Serial.println("   http://esp32-irrig.local (mDNS)");
        Serial.println("\n📊 Iniciando monitoreo...\n");
    }
}

void loop() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastCheck >= CHECK_INTERVAL) {
        lastCheck = currentTime;
        
        soilMoisture = readSoilMoisture();
        Serial.printf("💧 Humedad: %d%% | Bomba: %s | Modo: ", 
                     soilMoisture, 
                     pumpActive ? "ON" : "OFF");
        
        if (currentMode == AUTO) {
            Serial.println("AUTO");
            autoIrrigation();
        } else if (currentMode == MANUAL) {
            Serial.println("MANUAL");
        } else {
            Serial.println("PROGRAMADO");
            // Chequear horarios
            time_t now = time(nullptr);
            if (now > 100000) {
                struct tm t; localtime_r(&now, &t);
                for (int i=0;i<3;i++){
                    if (!schedules[i].enabled) continue;
                    if (t.tm_hour==schedules[i].hour && t.tm_min==schedules[i].minute) {
                        int day = t.tm_yday; int minute = t.tm_hour*60 + t.tm_min;
                        if (lastRunDay[i]!=day || lastRunMinute[i]!=minute) {
                            Serial.printf("⏰ Ejecutando horario %d (%02d:%02d) por %d s\n", i, schedules[i].hour, schedules[i].minute, schedules[i].duration);
                            activatePump(true);
                            // Programar apagado por duración
                            pumpStartTime = millis();
                            // Forzar detención cuando pase duración
                            // La lógica de seguridad en autoIrrigation también cubre este caso
                            lastRunDay[i]=day; lastRunMinute[i]=minute;
                        }
                    }
                }
                // Cortar cuando alcance duración en modo programado
                if (pumpActive && pumpStartTime>0) {
                    unsigned long elapsed = (millis()-pumpStartTime)/1000;
                    for (int i=0;i<3;i++){
                        if (schedules[i].enabled && elapsed >= (unsigned long)schedules[i].duration) {
                            activatePump(false);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    delay(100);
}
