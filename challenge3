#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// Wi-Fi Credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// PWM Settings
const int ledPin = 2;  // LED pin (can be changed as per your setup)
const int pwmChannel = 0;
const int pwmFreq = 5000;
const int pwmResolution = 8;

// Web Server and WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Variable to store the LED brightness
int brightness = 0;

// HTML for Web Page
const char* html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>LED Brightness Control</title>
  <script>
    var ws;
    function startWebSocket() {
      ws = new WebSocket(`ws://${location.hostname}/ws`);
      ws.onmessage = (event) => {
        document.getElementById("brightnessValue").innerText = event.data;
        document.getElementById("brightnessSlider").value = event.data;
      };
    }
    function updateBrightness(val) {
      ws.send(val);
    }
    window.onload = () => { startWebSocket(); };
  </script>
</head>
<body>
  <h1>LED Brightness Control</h1>
  <p>Adjust brightness:</p>
  <input type="range" id="brightnessSlider" min="0" max="255" oninput="updateBrightness(this.value)">
  <p>Brightness: <span id="brightnessValue">0</span></p>
</body>
</html>
)rawliteral";

// OTA Setup
void setupOTA() {
  // Start OTA
  MDNS.begin("esp32-led");  // Set MDNS hostname for easy access
  server.on("/update", HTTP_POST, [](AsyncWebServerRequest *request){
    // Handle response after OTA update
    request->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, [](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      Serial.printf("Update Start: %s\n", filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) Update.printError(Serial);
    }
    if (!Update.hasError()) {
      if (Update.write(data, len) != len) Update.printError(Serial);
    }
    if (final) {
      if (Update.end(true)) Serial.printf("Update Success: %uB\n", index + len);
      else Update.printError(Serial);
    }
  });
  MDNS.addService("http", "tcp", 80);
}

// WebSocket Events
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
               void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_DATA) {
    String message = "";
    for (size_t i = 0; i < len; i++) message += (char) data[i];
    brightness = message.toInt();  // Convert to integer
    ledcWrite(pwmChannel, brightness);  // Update PWM value
    ws.textAll(String(brightness));     // Send updated brightness to all clients
  }
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(1000);

  Serial.println("WiFi Connected");

  // Start OTA
  setupOTA();

  // Set up PWM for LED
  ledcSetup(pwmChannel, pwmFreq, pwmResolution);
  ledcAttachPin(ledPin, pwmChannel);

  // WebSocket Setup
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);

  // Serve Web Page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", html);
  });

  // Start Web Server
  server.begin();
}

void loop() {
  // OTA and WebSocket run asynchronously, so no code needed here for updates
}
