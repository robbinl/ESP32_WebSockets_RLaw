#include <Arduino.h>

#define LED_ONBOARD_PIN   2
#define LED1_PIN   25

// LED
struct Led {
    uint8_t pin;
    bool    on;

    void update() {
        digitalWrite(pin, on ? HIGH : LOW);
    }
};

// Global Variables
Led    onboard_led = { LED_ONBOARD_PIN, false };
Led    led1        = { LED1_PIN, false };

void setup() {
    pinMode(onboard_led.pin,  OUTPUT);
    pinMode(led1.pin,         OUTPUT);
}

void loop() {
    onboard_led.on = millis() % 1000 < 500;
    onboard_led.update();
    led1.on = millis() % 2000 < 1000;
    led1.update();
}