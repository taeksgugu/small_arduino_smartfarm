#include <AsyncTimer.h>

AsyncTimer t;

void setup() {
    Serial.begin(250000);
   
    t.setInterval([]() {
        Serial.println("Function A fired on " + String(millis() / 1000) + "s.");
    }, 500);
   
    t.setInterval([]() {
        Serial.println("Function B fired on " + String(millis() / 1000) + "s.");
    }, 1000);
   
    t.setTimeout([]() {
        Serial.println("Function C fired after 5 seconds.");
    }, 5000);
}

void loop() {
    t.handle();
}