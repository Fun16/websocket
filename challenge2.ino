#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "ALX The Piano 01";
const char* password = "Thepiano01";

// GPIO Pins for LEDs
const int ledPin1 = 3;  // LED 1 on GPIO 3
const int ledPin2 = 4;  // LED 2 on GPIO 4

// Web server and WebSocket setup
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// PWM properties
int ledBrightness1 = 0; // Brightness for LED 1
int ledBrightness2 = 0; // Brightness for LED 2
const int pwmFreq = 5000;
const int pwmChannel1 = 0;
const int pwmChannel2 = 1;
const int pwmResolution = 8;  // 8-bit resolution, values from 0-255

// Store IP address globally
String ipAddress;

// HTML page with sliders
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP PWM LED Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html { font-family: Arial, Helvetica, sans-serif; text-align: center; }
    .slider { width: 300px; }
    .state { font-size: 1.5rem; color:#8c8c8c; font-weight: bold; }
  </style>
</head>
<body>
  <h2>LED Brightness Control</h2>
  <p>IP Address: <span id="ipAddress">%IP_ADDRESS%</span></p>
  <p>LED 1 Brightness: <span id="brightness1">%BRIGHTNESS1%</span></p>
  <input type="range" id="slider1" min="0" max="255" class="slider" value="%BRIGHTNESS1%" onchange="updateBrightness(1, this.value)">
  
  <p>LED 2 Brightness: <span id="brightness2">%BRIGHTNESS2%</span></p>
  <input type="range" id="slider2" min="0" max="255" class="slider" value="%BRIGHTNESS2%" onchange="updateBrightness(2, this.value)">

<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  function initWebSocket() {
    websocket = new WebSocket(gateway);
    websocket.onopen = function(event) { console.log('WebSocket opened'); };
    websocket.onclose = function(event) { console.log('WebSocket closed'); setTimeout(initWebSocket, 2000); };
    websocket.onmessage = function(event) {
      var data = JSON.parse(event.data);
      document.getElementById('brightness1').innerText = data.brightness1;
      document.getElementById('slider1').value = data.brightness1;
      document.getElementById('brightness2').innerText = data.brightness2;
      document.getElementById('slider2').value = data.brightness2;
    };
  }
  function updateBrightness(led, brightness) {
    websocket.send(JSON.stringify({ led: led, brightness: brightness }));
  }
  window.addEventListener('load', initWebSocket);
</script>
</body>
</html>
)rawliteral";

// WebSocket notification to clients
void notifyClients() {
  String json = "{\"brightness1\":" + String(ledBrightness1) + ",\"brightness2\":" + String(ledBrightness2) + "}";
  ws.textAll(json);
}

// WebSocket message handler for brightness adjustment
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    DynamicJsonDocument doc(1024);
    deserializeJson(doc, (char*)data);
    int led = doc["led"];
    int brightness = doc["brightness"];
    
    if (led == 1) {
      ledBrightness1 = brightness;
      ledcWrite(pwmChannel1, ledBrightness1);
    } else if (led == 2) {
      ledBrightness2 = brightness;
      ledcWrite(pwmChannel2, ledBrightness2);
    }
    notifyClients();
  }
}

// WebSocket events
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected\n", client->id());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    default:
      break;
  }
}

// Initialize WebSocket server
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Placeholder to process HTML variables
String processor(const String& var) {
  if (var == "IP_ADDRESS") return ipAddress;  // Provide IP address
  if (var == "BRIGHTNESS1") return String(ledBrightness1);
  if (var == "BRIGHTNESS2") return String(ledBrightness2);
  return String();
}

void setup() {
  Serial.begin(115200);

  // Wi-Fi connection
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  
  // Store IP address
  ipAddress = WiFi.localIP().toString();
  Serial.print("IP address: ");
  Serial.println(ipAddress);

  // PWM setup
  ledcSetup(pwmChannel1, pwmFreq, pwmResolution);
  ledcAttachPin(ledPin1, pwmChannel1);
  ledcWrite(pwmChannel1, ledBrightness1);

  ledcSetup(pwmChannel2, pwmFreq, pwmResolution);
  ledcAttachPin(ledPin2, pwmChannel2);
  ledcWrite(pwmChannel2, ledBrightness2);

  // WebSocket initialization
  initWebSocket();

  // HTML route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
}
