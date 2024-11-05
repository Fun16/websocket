#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Network credentials
const char* ssid = "ALX The Piano 01";
const char* password = "Thepiano01";

// Initial states of LEDs
bool ledState1 = 0;
bool ledState2 = 0;
const int ledPin1 = 3; // GPIO 2 for LED 1
const int ledPin2 = 4; // GPIO 4 for LED 2

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html { font-family: Arial, Helvetica, sans-serif; text-align: center; }
    h1 { font-size: 1.8rem; color: white; }
    h2 { font-size: 1.5rem; font-weight: bold; color: #143642; }
    .topnav { overflow: hidden; background-color: #143642; }
    body { margin: 0; }
    .content { padding: 30px; max-width: 600px; margin: 0 auto; }
    .card { background-color: #F8F7F9; box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5); padding:10px 0; }
    .button { padding: 15px 50px; font-size: 24px; text-align: center; color: #fff; background-color: #0f8b8d; border: none; border-radius: 5px; }
    .state { font-size: 1.5rem; color:#8c8c8c; font-weight: bold; }
  </style>
</head>
<body>
  <div class="topnav"><h1>ESP WebSocket Server</h1></div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 3 (LED 1)</h2>
      <p class="state">State: <span id="state1">%STATE1%</span></p>
      <p><button id="button1" class="button">Toggle LED 1</button></p>
    </div>
    <div class="card">
      <h2>Output - GPIO 4 (LED 2)</h2>
      <p class="state">State: <span id="state2">%STATE2%</span></p>
      <p><button id="button2" class="button">Toggle LED 2</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;

  window.addEventListener('load', onLoad);
  
  function initWebSocket() {
    console.log('Opening WebSocket...');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
  }
  
  function onOpen(event) { console.log('Connection opened'); }
  function onClose(event) { console.log('Connection closed'); setTimeout(initWebSocket, 2000); }
  
  function onMessage(event) {
    const data = JSON.parse(event.data);
    document.getElementById('state1').innerHTML = data.led1 ? "ON" : "OFF";
    document.getElementById('state2').innerHTML = data.led2 ? "ON" : "OFF";
  }
  
  function onLoad(event) {
    initWebSocket();
    document.getElementById('button1').addEventListener('click', () => websocket.send('toggle1'));
    document.getElementById('button2').addEventListener('click', () => websocket.send('toggle2'));
  }
</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  String message = "{\"led1\": " + String(ledState1) + ", \"led2\": " + String(ledState2) + "}";
  ws.textAll(message);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle1") == 0) {
      ledState1 = !ledState1;
      notifyClients();
    } else if (strcmp((char*)data, "toggle2") == 0) {
      ledState2 = !ledState2;
      notifyClients();
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
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
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var) {
  if(var == "STATE1") {
    return ledState1 ? "ON" : "OFF";
  }
  else if(var == "STATE2") {
    return ledState2 ? "ON" : "OFF";
  }
  return String();
}

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  digitalWrite(ledPin1, LOW);
  digitalWrite(ledPin2, LOW);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println(WiFi.localIP());
  
  initWebSocket();
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin1, ledState1);
  digitalWrite(ledPin2, ledState2);
}
