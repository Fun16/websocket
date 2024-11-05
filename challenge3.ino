#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

// WiFi credentials
const char* ssid = "ALX The Piano 01";
const char* password = "Thepiano01";


// PWM Settings
const int ledPin = 4;  // LED pin (can be changed as per your setup)
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
