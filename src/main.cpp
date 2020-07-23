#include <Arduino.h>

#define LED_ONBOARD_PIN   2
#define LED_PIN   25
#define BTN_PIN   16

struct Led {
    // state variables
    uint8_t pin;
    bool    on;

    // methods
    void update() {
        digitalWrite(pin, on ? HIGH : LOW);
    }
};

Led    onboard_led = { LED_ONBOARD_PIN, false };
Led    led         = { LED_PIN, false };

void setup() {
    pinMode(onboard_led.pin, OUTPUT);
    pinMode(led.pin,         OUTPUT);
}

void loop() {
    onboard_led.on = millis() % 2000 < 50;
    led.on = millis() % 1000 < 50;

    led.update();
    onboard_led.update();
}