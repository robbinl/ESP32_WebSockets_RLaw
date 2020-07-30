#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h> // we need this lib in platformio.ini

#define LED_ONBOARD_PIN   2
#define LED1_PIN   25
#define BTN1_PIN   16
#define LED2_PIN   26
#define BTN2_PIN   17
#define HTTP_PORT 80 // Web Server Setup

const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds

// LED
struct Led {
    uint8_t pin;
    bool    on;

    void update() {
        digitalWrite(pin, on ? HIGH : LOW);
    }
};

// Button
struct Button {
    uint8_t  pin;
    bool     lastReading;
    uint32_t lastDebounceTime;
    uint16_t state;

    bool pressed()                { return state == 1; }
    bool released()               { return state == 0xffff; }
    bool held(uint16_t count = 0) { return state > 1 + count && state < 0xffff; }

    void read() {
        bool reading = digitalRead(pin);

        // if the logic level has changed since the last reading
        // reset lastDebounceTime to now
        if (reading != lastReading) {
            lastDebounceTime = millis();
        }

        // after out of the bouncing phase
        // the actual status of the button is determined
        if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
            // the pin is pulled up when not pressed
            bool pressed = reading == LOW;
            if (pressed) {
                     if (state  < 0xfffe) state++;
                else if (state == 0xfffe) state = 2;
            } else if (state) {
                state = state == 0xffff ? 0 : 0xffff;
            }
        }
        lastReading = reading;
    }
};

// Global Variables
Led    onboard_led = { LED_ONBOARD_PIN, false };
Led    led1        = { LED1_PIN, false };
Button button1      = { BTN1_PIN, HIGH, 0, 0 };
Led    led2        = { LED2_PIN, false };
Button button2      = { BTN2_PIN, HIGH, 0, 0 };

AsyncWebServer server(HTTP_PORT);
AsyncWebSocket ws("/ws");

// WiFi credentials
char WIFI_SSID[20];
char WIFI_PASS[20];

// SPIFFS
void readFileSSID(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    while (file.available()) {
        int l = file.readBytesUntil('\n', WIFI_SSID, sizeof(WIFI_SSID));
        WIFI_SSID[l] = '\0';
    }
    file.close();
}

void readFilePASS(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);
    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }
    while (file.available()) {
        int l = file.readBytesUntil('\0', WIFI_PASS, sizeof(WIFI_PASS));
        WIFI_PASS[l] = '\0';
    }
    file.close();
}

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
    while (1) {
        onboard_led.on = millis() % 200 < 50;
        onboard_led.update();
    }
  }
  else{
    Serial.println("SPIFFS volume mounted properly");
    readFileSSID(SPIFFS, "/ssid.txt");
    readFilePASS(SPIFFS, "/pass.txt");
  }
}

// Wifi Setup
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}

// Web Server Setup
String processor(const String& var)
{
  if(var == "STATE")
    return (led1.on ? "on" : "off");
  return String();
}

void onRootRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html", false, processor);
}

void initWebServer() {
    server.on("/", onRootRequest);
    server.serveStatic("/", SPIFFS, "/");
    server.begin();
}

// Web Socket Setup
void onEvent(AsyncWebSocket       *server,  //
             AsyncWebSocketClient *client,  //
             AwsEventType          type,    // the signature of this function is defined
             void                 *arg,     // by the `AwsEventHandler` interface
             uint8_t              *data,    //
             size_t                len) {   //

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}
void initWebSocket() {
    ws.onEvent(onEvent);
    server.addHandler(&ws);
}

// Data Exchange
void notifyClients() {
    ws.textAll(led1.on ? "on" : "off");
}
void setup() {
    pinMode(onboard_led.pin,  OUTPUT);
    pinMode(led1.pin,         OUTPUT);
    pinMode(button1.pin,      INPUT);
    pinMode(led2.pin,         OUTPUT);
    pinMode(button2.pin,      INPUT);

    Serial.begin(9600); delay(500);
    initSPIFFS();
    initWiFi();
    initWebSocket();
    initWebServer();
}

void loop() {
    ws.cleanupClients();
    button1.read();
    if (button1.pressed()) {
        led1.on = !led1.on;
        notifyClients();
    }
    button2.read();
         if (button2.held())     led2.on = true;
    else if (button2.released()) led2.on = false;
    led1.update();
    led2.update();
    onboard_led.on = millis() % 2000 < 1000;
    onboard_led.update();
}