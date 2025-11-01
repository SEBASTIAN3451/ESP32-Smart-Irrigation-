/*/*

 * ESP32 Smart Irrigation System v2.0 PRO * ESP32 Smart Irrigation System

 *  * 

 * Sistema profesional de riego inteligente con OTA, WiFiManager y observabilidad completa. * Sistema inteligente de riego automático para plantas con monitoreo

 *  * de humedad del suelo, control de bomba de agua y programación horaria.

 * Features v2.0: * 

 * - OTA (Over-The-Air updates) via ArduinoOTA * Características:

 * - WiFiManager para configuración sin hardcodear credenciales * - Monitoreo continuo de humedad del suelo (sensor capacitivo)

 * - Schedules persistentes (Preferences/NVS) * - Control automático y manual de bomba de agua

 * - Historial de riegos (últimos 50 eventos) con Chart.js * - Programación de horarios de riego

 * - Export CSV de eventos de riego * - Dashboard web con control en tiempo real

 * - SSE para actualizaciones en tiempo real * - Historial de riegos y estadísticas

 * - Estadísticas avanzadas (total riegos, tiempo acumulado) * - Modos: Automático, Manual, Programado

 * - mDNS (esp32-irrigation.local) * - Indicador LED RGB de estado

 * - NTP timestamps UTC * 

 * - Endpoints: /metrics, /healthz, /api/status, /api/history, /api/export * Hardware:

 * - Indicador LED RGB de estado * - ESP32 DevKit

 *  * - Sensor de humedad de suelo capacitivo

 * Modos de operación: * - Relé 5V (control de bomba)

 * - AUTO: Riega automáticamente cuando humedad < umbral * - LED RGB WS2812B (NeoPixel)

 * - MANUAL: Control manual via dashboard * - Bomba de agua 5V (o electroválvula)

 * - SCHEDULED: Riega en horarios programados * 

 *  * Conexiones:

 * Hardware: * - Sensor Humedad -> GPIO 34 (ADC1_CH6)

 * - ESP32 DevKit + Sensor humedad capacitivo (GPIO34) + Relé (GPIO25) + LED RGB WS2812B (GPIO5) * - Relé Bomba -> GPIO 25

 *  * - LED RGB -> GPIO 5

 * Autor: Sebastian Lara | github.com/SEBASTIAN3451 * - Alimentación: 5V para bomba y relé

 * Fecha: Octubre 2025 * 

 */ * Autor: Sebastian Lara

 * Fecha: Octubre 2025

#include <Arduino.h> * Versión: 1.0

#include <WiFi.h> */

#include <WiFiManager.h>

#include <ESPAsyncWebServer.h>#include <Arduino.h>

#include <ESPmDNS.h>#include <WiFi.h>

#include <ArduinoOTA.h>#include <ESPAsyncWebServer.h>

#include <time.h>#include <ArduinoJson.h>

#include <ArduinoJson.h>#include <Adafruit_NeoPixel.h>

#include <Adafruit_NeoPixel.h>#include <Preferences.h>

#include <Preferences.h>#include <ESPmDNS.h>

#include <time.h>

// ==================== CONFIG ====================

#define SOIL_PIN 34// ==================== CONFIGURACIÓN ====================

#define PUMP_PIN 25

#define LED_PIN 5// WiFi - CAMBIAR ESTOS VALORES

#define LED_COUNT 1const char* ssid = "TU_WIFI_SSID";

#define MOISTURE_DRY 2800const char* password = "TU_WIFI_PASSWORD";

#define MOISTURE_WET 1500

#define PUMP_TIME_MS 5000// Pines

#define CHECK_INTERVAL 10000#define SOIL_MOISTURE_PIN 34    // Sensor de humedad (ADC)

#define HISTORY_SIZE 50#define PUMP_RELAY_PIN 25       // Relé de la bomba

#define LED_RGB_PIN 5           // LED RGB indicador

// ==================== GLOBALS ====================#define LED_COUNT 1             // Número de LEDs

AsyncWebServer server(80);

AsyncEventSource events("/events");// Umbrales de humedad (0-4095, ADC 12-bit)

Adafruit_NeoPixel led(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);#define MOISTURE_DRY 2800       // Suelo seco (necesita riego)

Preferences prefs;#define MOISTURE_WET 1500       // Suelo húmedo (detener riego)



enum Mode { AUTO, MANUAL, SCHEDULED };// Configuración de riego

Mode mode = AUTO;#define PUMP_TIME_MS 5000       // Tiempo de riego por ciclo (5 segundos)

bool pumpActive = false;#define CHECK_INTERVAL 10000    // Intervalo de verificación (10 segundos)

int soilMoisture = 0;

unsigned long lastCheck = 0, pumpStartTime = 0;// ==================== OBJETOS GLOBALES ====================

unsigned long totalWateringTime = 0;

int wateringCount = 0;AsyncWebServer server(80);

AsyncEventSource events("/events");

struct Schedule { int hour; int minute; int duration; bool enabled; };Adafruit_NeoPixel led(LED_COUNT, LED_RGB_PIN, NEO_GRB + NEO_KHZ800);

Schedule schedules[3] = {{7,0,10,false},{14,0,10,false},{19,0,10,false}};Preferences preferences;

int lastRunDay[3]={-1,-1,-1}, lastRunMinute[3]={-1,-1,-1};

// Variables de estado

struct WaterEvent { time_t timestamp; int duration; int moisture; };enum Mode { AUTO, MANUAL, SCHEDULED };

WaterEvent history[HISTORY_SIZE];Mode currentMode = AUTO;

int historyIndex=0, historyCount=0;

bool pumpActive = false;

// ==================== HTML with Chart.js ====================int soilMoisture = 0;

const char index_html[] PROGMEM = R"rawliteral(unsigned long lastCheck = 0;

<!DOCTYPE html>unsigned long pumpStartTime = 0;

<html lang="es">unsigned long totalWateringTime = 0;

<head>int wateringCount = 0;

<meta charset="utf-8"/><meta name="viewport" content="width=device-width, initial-scale=1"/>time_t lastNtpSync = 0;

<title>💧 ESP32 Smart Irrigation PRO</title>

<script src="https://cdn.jsdelivr.net/npm/chart.js@4.4.0"></script>// Horarios programados (formato 24h)

<style>struct Schedule {

*{margin:0;padding:0;box-sizing:border-box}    int hour;

body{font-family:'Segoe UI',sans-serif;background:linear-gradient(135deg,#11998e,#38ef7d);min-height:100vh;padding:20px}    int minute;

.container{max-width:1100px;margin:0 auto;background:#fff;border-radius:20px;padding:30px;box-shadow:0 20px 60px rgba(0,0,0,0.3)}    int duration;  // segundos

h1{text-align:center;color:#333;margin-bottom:30px}    bool enabled;

.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(180px,1fr));gap:15px;margin-bottom:25px}};

.card{border-radius:15px;padding:20px;color:#fff;text-align:center}

.card h3{font-size:13px;opacity:0.9;margin-bottom:6px}Schedule schedules[3] = {

.card .value{font-size:32px;font-weight:bold}    {7, 0, 10, false},    // 7:00 AM, 10 segundos

.controls{display:flex;gap:10px;margin:20px 0;flex-wrap:wrap;justify-content:center}    {14, 0, 10, false},   // 2:00 PM, 10 segundos

.btn{background:#11998e;color:#fff;border:none;padding:12px 24px;border-radius:8px;cursor:pointer;font-size:14px}    {19, 0, 10, false}    // 7:00 PM, 10 segundos

.btn:hover{background:#0d7a6e}};

.btn.active{background:#f5576c}

.btn.danger{background:#e74c3c}// Evitar re-ejecutar el mismo minuto

#chartContainer{margin:20px 0}int lastRunDay[3] = {-1, -1, -1};

.status{text-align:center;padding:12px;background:#f0f0f0;border-radius:8px;margin-top:15px}int lastRunMinute[3] = {-1, -1, -1};

.dot{display:inline-block;width:10px;height:10px;border-radius:50%;background:#4caf50;animation:pulse 2s infinite;margin-right:6px}

@keyframes pulse{0%,100%{opacity:1}50%{opacity:0.5}}// ==================== PÁGINA WEB ====================

table{width:100%;border-collapse:collapse;margin:15px 0}

th,td{padding:8px;text-align:left;border-bottom:1px solid #ddd}const char index_html[] PROGMEM = R"rawliteral(

th{background:#f5f5f5;font-weight:600}<!DOCTYPE html>

</style><html lang="es">

</head><head>

<body>    <meta charset="UTF-8">

<div class="container">    <meta name="viewport" content="width=device-width, initial-scale=1.0">

<h1>💧 Sistema de Riego Inteligente PRO</h1>    <title>🌱 Smart Irrigation</title>

<div class="cards">    <style>

<div class="card" style="background:linear-gradient(135deg,#4facfe,#00f2fe)">        * { margin: 0; padding: 0; box-sizing: border-box; }

<h3>Humedad Suelo</h3><div class="value"><span id="moisture">--</span>%</div></div>        

<div class="card" style="background:linear-gradient(135deg,#43e97b,#38f9d7)">        body {

<h3>Bomba</h3><div class="value" id="pumpStatus">OFF</div></div>            font-family: 'Segoe UI', sans-serif;

<div class="card" style="background:linear-gradient(135deg,#fa709a,#fee140)">            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);

<h3>Modo</h3><div class="value" id="modeDisplay">AUTO</div></div>            min-height: 100vh;

<div class="card" style="background:linear-gradient(135deg,#a8edea,#fed6e3)">            padding: 20px;

<h3>Total Riegos</h3><div class="value"><span id="waterCount">0</span></div></div>        }

</div>        

<div class="controls">        .container {

<button class="btn" onclick="setMode('AUTO')">🤖 AUTO</button>            max-width: 900px;

<button class="btn" onclick="setMode('MANUAL')">✋ MANUAL</button>            margin: 0 auto;

<button class="btn" onclick="setMode('SCHEDULED')">⏰ PROGRAMADO</button>            background: white;

<button class="btn danger" id="pumpBtn" onclick="togglePump()">💦 REGAR</button>            border-radius: 20px;

<button class="btn" onclick="exportCSV()">📥 CSV</button>            padding: 30px;

<button class="btn" onclick="location.href='/metrics'">📊 Metrics</button>            box-shadow: 0 20px 60px rgba(0,0,0,0.3);

</div>        }

<h3>📈 Historial de Riegos</h3>        

<div id="chartContainer"><canvas id="chart"></canvas></div>        h1 {

<div class="status"><span class="dot"></span><strong id="status">Conectado</strong><div style="font-size:12px;color:#666;margin-top:4px" id="lastUpdate">Cargando...</div></div>            color: #333;

</div>            text-align: center;

<script>            margin-bottom: 30px;

const ctx=document.getElementById('chart').getContext('2d');            font-size: 32px;

let chart=new Chart(ctx,{type:'bar',data:{labels:[],datasets:[{label:'Duración (s)',data:[],backgroundColor:'#11998e'}]},options:{responsive:true}});        }

function apply(d){        

document.getElementById('moisture').textContent=d.moisture_percent;        .status-card {

document.getElementById('pumpStatus').textContent=d.pump_active?'ON':'OFF';            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);

document.getElementById('modeDisplay').textContent=d.mode;            border-radius: 15px;

document.getElementById('waterCount').textContent=d.watering_count;            padding: 30px;

document.getElementById('pumpBtn').textContent=d.pump_active?'⏹️ PARAR':'💦 REGAR';            color: white;

document.getElementById('lastUpdate').textContent='Última: '+new Date().toLocaleTimeString();            margin-bottom: 30px;

}        }

async function setMode(m){await fetch('/api/mode?m='+m);poll();}        

async function togglePump(){await fetch('/api/pump?state='+(document.getElementById('pumpStatus').textContent==='OFF'?'1':'0'));poll();}        .moisture-display {

async function loadHistory(){            display: flex;

const r=await fetch('/api/history');const h=await r.json();            align-items: center;

chart.data.labels=h.map(x=>new Date(x.timestamp*1000).toLocaleString());            justify-content: space-between;

chart.data.datasets[0].data=h.map(x=>x.duration);            margin-bottom: 20px;

chart.update();        }

}        

async function poll(){try{const r=await fetch('/api/status');apply(await r.json());loadHistory()}catch(_){}}        .moisture-value {

try{const es=new EventSource('/events');es.addEventListener('update',e=>{try{apply(JSON.parse(e.data));loadHistory()}catch(_){}});}catch(_){}            font-size: 48px;

setInterval(poll,5000);poll();loadHistory();            font-weight: bold;

function exportCSV(){window.open('/api/export','_blank')}        }

</script>        

</body>        .moisture-bar {

</html>            width: 100%;

)rawliteral";            height: 30px;

            background: rgba(255,255,255,0.3);

// ==================== FUNCTIONS ====================            border-radius: 15px;

void setLED(uint32_t c){led.setPixelColor(0,c);led.show();}            overflow: hidden;

            position: relative;

void addWaterEvent(int dur, int moist){        }

  history[historyIndex].timestamp=time(nullptr);        

  history[historyIndex].duration=dur;        .moisture-fill {

  history[historyIndex].moisture=moist;            height: 100%;

  historyIndex=(historyIndex+1)%HISTORY_SIZE;            background: white;

  if(historyCount<HISTORY_SIZE)historyCount++;            transition: width 0.5s ease;

}            border-radius: 15px;

        }

void setPump(bool on){        

  pumpActive=on;        .controls {

  digitalWrite(PUMP_PIN,on?HIGH:LOW);            display: grid;

  if(on){pumpStartTime=millis();setLED(led.Color(0,0,255));}            grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));

  else{setLED(led.Color(0,255,0));}            gap: 20px;

}            margin-bottom: 30px;

        }

void saveSchedules(){        

  prefs.begin("irrigation",false);        .control-card {

  for(int i=0;i<3;i++){            background: #f5f5f5;

    prefs.putInt((String("h")+i).c_str(),schedules[i].hour);            border-radius: 15px;

    prefs.putInt((String("m")+i).c_str(),schedules[i].minute);            padding: 20px;

    prefs.putInt((String("d")+i).c_str(),schedules[i].duration);            text-align: center;

    prefs.putBool((String("e")+i).c_str(),schedules[i].enabled);        }

  }        

  prefs.end();        .control-card h3 {

}            color: #333;

            margin-bottom: 15px;

void loadSchedules(){            font-size: 18px;

  prefs.begin("irrigation",true);        }

  for(int i=0;i<3;i++){        

    schedules[i].hour=prefs.getInt((String("h")+i).c_str(),schedules[i].hour);        .btn {

    schedules[i].minute=prefs.getInt((String("m")+i).c_str(),schedules[i].minute);            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);

    schedules[i].duration=prefs.getInt((String("d")+i).c_str(),schedules[i].duration);            color: white;

    schedules[i].enabled=prefs.getBool((String("e")+i).c_str(),false);            border: none;

  }            padding: 15px 30px;

  prefs.end();            border-radius: 10px;

}            font-size: 16px;

            cursor: pointer;

void setupWiFiManager(){            transition: transform 0.2s;

  WiFi.mode(WIFI_STA);            width: 100%;

  WiFiManager wm;            margin-top: 10px;

  wm.setConfigPortalTimeout(180);        }

  if(!wm.autoConnect("ESP32-Irrigation")){        

    Serial.println("WiFi failed, restarting...");        .btn:hover {

    delay(3000);ESP.restart();            transform: scale(1.05);

  }        }

  Serial.println("✓ WiFi: "+WiFi.localIP().toString());        

  setLED(led.Color(0,255,0));        .btn:active {

  if(MDNS.begin("esp32-irrigation")){            transform: scale(0.95);

    MDNS.addService("http","tcp",80);        }

    Serial.println("✓ mDNS: http://esp32-irrigation.local");        

  }        .btn-danger {

  configTime(0,0,"pool.ntp.org");            background: linear-gradient(135deg, #f093fb 0%, #f5576c 100%);

}        }

        

void setupOTA(){        .btn-success {

  ArduinoOTA.setHostname("esp32-irrigation");            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);

  ArduinoOTA.onStart([](){ Serial.println("OTA Start");setLED(led.Color(255,0,255)); });        }

  ArduinoOTA.onEnd([](){ Serial.println("\nOTA End"); });        

  ArduinoOTA.onProgress([](unsigned int p,unsigned int t){Serial.printf("OTA: %u%%\r",(p/(t/100)));});        .pump-status {

  ArduinoOTA.onError([](ota_error_t e){Serial.printf("OTA Error[%u]: ",e);});            display: inline-block;

  ArduinoOTA.begin();            width: 15px;

  Serial.println("✓ OTA ready");            height: 15px;

}            border-radius: 50%;

            margin-right: 8px;

void setupWebServer(){        }

  server.on("/",[](AsyncWebServerRequest*req){req->send_P(200,"text/html",index_html);});        

        .pump-on { background: #4CAF50; animation: pulse 1s infinite; }

  server.on("/api/status",[](AsyncWebServerRequest*req){        .pump-off { background: #ccc; }

    int raw=analogRead(SOIL_PIN);        

    int pct=map(constrain(raw,MOISTURE_WET,MOISTURE_DRY),MOISTURE_DRY,MOISTURE_WET,0,100);        @keyframes pulse {

    StaticJsonDocument<400>doc;            0%, 100% { opacity: 1; }

    doc["moisture_raw"]=raw;doc["moisture_percent"]=pct;doc["pump_active"]=pumpActive;            50% { opacity: 0.5; }

    doc["mode"]=(mode==AUTO?"AUTO":(mode==MANUAL?"MANUAL":"SCHEDULED"));        }

    doc["watering_count"]=wateringCount;doc["total_watering_time"]=totalWateringTime;        

    String out;serializeJson(doc,out);req->send(200,"application/json",out);        .mode-selector {

  });            display: flex;

            gap: 10px;

  server.on("/api/mode",[](AsyncWebServerRequest*req){            margin-bottom: 20px;

    if(req->hasParam("m")){        }

      String m=req->getParam("m")->value();        

      if(m=="AUTO")mode=AUTO;else if(m=="MANUAL")mode=MANUAL;else if(m=="SCHEDULED")mode=SCHEDULED;        .mode-btn {

      if(mode!=MANUAL&&pumpActive)setPump(false);            flex: 1;

    }            padding: 15px;

    req->send(200,"text/plain","OK");            border: 2px solid #667eea;

  });            background: white;

            color: #667eea;

  server.on("/api/pump",[](AsyncWebServerRequest*req){            border-radius: 10px;

    if(req->hasParam("state")&&mode==MANUAL){            cursor: pointer;

      bool s=req->getParam("state")->value()=="1";            transition: all 0.3s;

      setPump(s);        }

    }        

    req->send(200,"text/plain","OK");        .mode-btn.active {

  });            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);

            color: white;

  server.on("/api/history",[](AsyncWebServerRequest*req){        }

    DynamicJsonDocument doc(4096);        

    JsonArray arr=doc.to<JsonArray>();        .stats {

    for(int i=0;i<historyCount;i++){            display: grid;

      int idx=(historyIndex+HISTORY_SIZE-historyCount+i)%HISTORY_SIZE;            grid-template-columns: repeat(3, 1fr);

      JsonObject o=arr.createNestedObject();            gap: 15px;

      o["timestamp"]=history[idx].timestamp;o["duration"]=history[idx].duration;o["moisture"]=history[idx].moisture;            margin-top: 30px;

    }        }

    String out;serializeJson(doc,out);req->send(200,"application/json",out);        

  });        .stat-box {

            background: #f5f5f5;

  server.on("/api/export",[](AsyncWebServerRequest*req){            padding: 20px;

    String csv="timestamp,duration_seconds,moisture_raw\n";            border-radius: 10px;

    for(int i=0;i<historyCount;i++){            text-align: center;

      int idx=(historyIndex+HISTORY_SIZE-historyCount+i)%HISTORY_SIZE;        }

      csv+=String(history[idx].timestamp)+","+String(history[idx].duration)+","+String(history[idx].moisture)+"\n";        

    }        .stat-value {

    req->send(200,"text/csv",csv);            font-size: 32px;

  });            font-weight: bold;

            color: #667eea;

  server.on("/api/schedules",[](AsyncWebServerRequest*req){        }

    if(req->method()==HTTP_POST){        

      if(req->hasParam("id",true)&&req->hasParam("h",true)&&req->hasParam("m",true)&&req->hasParam("d",true)&&req->hasParam("e",true)){        .stat-label {

        int id=req->getParam("id",true)->value().toInt();            color: #666;

        if(id>=0&&id<3){            font-size: 14px;

          schedules[id].hour=req->getParam("h",true)->value().toInt();            margin-top: 5px;

          schedules[id].minute=req->getParam("m",true)->value().toInt();        }

          schedules[id].duration=req->getParam("d",true)->value().toInt();        

          schedules[id].enabled=req->getParam("e",true)->value()=="1";        @media (max-width: 768px) {

          saveSchedules();            .controls { grid-template-columns: 1fr; }

        }            .stats { grid-template-columns: 1fr; }

      }        }

      req->send(200,"text/plain","OK");    </style>

    }else{</head>

      DynamicJsonDocument doc(512);<body>

      JsonArray arr=doc.to<JsonArray>();    <div class="container">

      for(int i=0;i<3;i++){        <h1>🌱 Sistema de Riego Inteligente</h1>

        JsonObject o=arr.createNestedObject();        

        o["id"]=i;o["hour"]=schedules[i].hour;o["minute"]=schedules[i].minute;        <div class="status-card">

        o["duration"]=schedules[i].duration;o["enabled"]=schedules[i].enabled;            <div class="moisture-display">

      }                <div>

      String out;serializeJson(doc,out);req->send(200,"application/json",out);                    <div style="font-size: 14px; opacity: 0.9;">Humedad del Suelo</div>

    }                    <div class="moisture-value"><span id="moisture">--</span>%</div>

  });                </div>

                <div style="text-align: right;">

  server.on("/metrics",[](AsyncWebServerRequest*req){                    <span class="pump-status" id="pumpStatus"></span>

    int raw=analogRead(SOIL_PIN);                    <span id="pumpText">Bomba Apagada</span>

    int pct=map(constrain(raw,MOISTURE_WET,MOISTURE_DRY),MOISTURE_DRY,MOISTURE_WET,0,100);                </div>

    String m="esp32_soil_moisture_percent ";m+=String(pct);m+="\nesp32_pump_active ";m+=String(pumpActive?1:0);m+="\n";            </div>

    req->send(200,"text/plain; version=0.0.4",m);            <div class="moisture-bar">

  });                <div class="moisture-fill" id="moistureBar" style="width: 0%"></div>

            </div>

  server.on("/healthz",[](AsyncWebServerRequest*req){req->send(200,"text/plain","ok");});        </div>

        

  events.onConnect([](AsyncEventSourceClient*c){        <div class="mode-selector">

    int raw=analogRead(SOIL_PIN);            <button class="mode-btn active" onclick="setMode('auto')">

    int pct=map(constrain(raw,MOISTURE_WET,MOISTURE_DRY),MOISTURE_DRY,MOISTURE_WET,0,100);                ⚡ Automático

    StaticJsonDocument<300>doc;            </button>

    doc["moisture_percent"]=pct;doc["pump_active"]=pumpActive;            <button class="mode-btn" onclick="setMode('manual')">

    doc["mode"]=(mode==AUTO?"AUTO":(mode==MANUAL?"MANUAL":"SCHEDULED"));                🎮 Manual

    doc["watering_count"]=wateringCount;            </button>

    String p;serializeJson(doc,p);c->send(p.c_str(),"update",millis());            <button class="mode-btn" onclick="setMode('scheduled')">

  });                ⏰ Programado

  server.addHandler(&events);            </button>

  server.begin();        </div>

  Serial.println("✓ Server :80\n");        

}        <div class="controls">

            <div class="control-card">

void checkAutoMode(){                <h3>Control Manual</h3>

  if(mode!=AUTO)return;                <button class="btn btn-success" onclick="startPump()">

  soilMoisture=analogRead(SOIL_PIN);                    💧 Iniciar Riego

  if(!pumpActive&&soilMoisture>=MOISTURE_DRY){                </button>

    Serial.printf("🌱 Suelo seco (%d), activando riego\n",soilMoisture);                <button class="btn btn-danger" onclick="stopPump()">

    setPump(true);                    ⛔ Detener Riego

  }else if(pumpActive){                </button>

    if(soilMoisture<=MOISTURE_WET||millis()-pumpStartTime>=PUMP_TIME_MS){            </div>

      int dur=(millis()-pumpStartTime)/1000;            

      totalWateringTime+=dur;wateringCount++;            <div class="control-card">

      addWaterEvent(dur,soilMoisture);                <h3>Estado del Sistema</h3>

      Serial.printf("💧 Riego completado (%ds)\n",dur);                <div style="text-align: left; margin-top: 15px;">

      setPump(false);                    <p>📊 Modo: <strong id="modeText">Automático</strong></p>

      StaticJsonDocument<300>doc;                    <p>💧 Estado: <strong id="statusText">Monitoreando</strong></p>

      doc["moisture_percent"]=map(constrain(soilMoisture,MOISTURE_WET,MOISTURE_DRY),MOISTURE_DRY,MOISTURE_WET,0,100);                    <p>⏱️ Tiempo activo: <strong id="uptime">0s</strong></p>

      doc["pump_active"]=false;doc["mode"]="AUTO";doc["watering_count"]=wateringCount;                </div>

      String p;serializeJson(doc,p);events.send(p.c_str(),"update",millis());            </div>

    }        </div>

  }        

}        <div class="stats">

            <div class="stat-box">

void checkScheduled(){                <div class="stat-value" id="wateringCount">0</div>

  if(mode!=SCHEDULED)return;                <div class="stat-label">Riegos Totales</div>

  time_t now=time(nullptr);if(now<100000)return;            </div>

  struct tm*t=localtime(&now);            <div class="stat-box">

  for(int i=0;i<3;i++){                <div class="stat-value" id="totalTime">0s</div>

    if(!schedules[i].enabled)continue;                <div class="stat-label">Tiempo Total</div>

    if(t->tm_hour==schedules[i].hour&&t->tm_min==schedules[i].minute){            </div>

      if(lastRunDay[i]==t->tm_mday&&lastRunMinute[i]==t->tm_min)continue;            <div class="stat-box">

      lastRunDay[i]=t->tm_mday;lastRunMinute[i]=t->tm_min;                <div class="stat-value" id="avgMoisture">0%</div>

      Serial.printf("⏰ Schedule %d activado\n",i);                <div class="stat-label">Humedad Promedio</div>

      setPump(true);            </div>

      delay(schedules[i].duration*1000);        </div>

      totalWateringTime+=schedules[i].duration;wateringCount++;    </div>

      addWaterEvent(schedules[i].duration,analogRead(SOIL_PIN));    

      setPump(false);    <script>

      StaticJsonDocument<300>doc;        let modeButtons = document.querySelectorAll('.mode-btn');

      doc["moisture_percent"]=map(constrain(analogRead(SOIL_PIN),MOISTURE_WET,MOISTURE_DRY),MOISTURE_DRY,MOISTURE_WET,0,100);        

      doc["pump_active"]=false;doc["mode"]="SCHEDULED";doc["watering_count"]=wateringCount;        function apply(data){

      String p;serializeJson(doc,p);events.send(p.c_str(),"update",millis());            document.getElementById('moisture').textContent = data.moisture_percent;

    }            document.getElementById('moistureBar').style.width = data.moisture_percent + '%';

  }            const pumpStatus = document.getElementById('pumpStatus');

}            const pumpText = document.getElementById('pumpText');

            if (data.pump_active) { pumpStatus.className = 'pump-status pump-on'; pumpText.textContent = '💧 Bomba Activa'; }

// ==================== SETUP & LOOP ====================            else { pumpStatus.className = 'pump-status pump-off'; pumpText.textContent = 'Bomba Apagada'; }

void setup(){            document.getElementById('modeText').textContent = data.mode;

  Serial.begin(115200);delay(1000);            document.getElementById('wateringCount').textContent = data.watering_count;

  pinMode(PUMP_PIN,OUTPUT);digitalWrite(PUMP_PIN,LOW);            document.getElementById('totalTime').textContent = data.total_time + 's';

  led.begin();setLED(led.Color(255,128,0));            document.getElementById('avgMoisture').textContent = data.moisture_percent + '%';

  Serial.println("\n💧 ESP32 Smart Irrigation PRO v2.0");        }

  loadSchedules();

  setupWiFiManager();        // SSE

  setupOTA();        try {

  setupWebServer();            const es = new EventSource('/events');

  Serial.printf("📱 http://%s | http://esp32-irrigation.local\n",WiFi.localIP().toString().c_str());            es.addEventListener('update', ev => { try { apply(JSON.parse(ev.data)); } catch(e){} });

}        } catch(e) {}



void loop(){        async function updateData(){

  ArduinoOTA.handle();            try { const r = await fetch('/api/status'); apply(await r.json()); } catch(e){}

  unsigned long now=millis();        }

  if(now-lastCheck>=CHECK_INTERVAL){        

    lastCheck=now;        function setMode(mode) {

    checkAutoMode();            fetch('/api/mode?mode=' + mode, { method: 'POST' })

    checkScheduled();                .then(() => {

  }                    modeButtons.forEach(btn => btn.classList.remove('active'));

  delay(100);                    event.target.classList.add('active');

}                    updateData();

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
