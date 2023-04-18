#include <DHT.h>
#include <Vegemite.h>

DHT dht(A1, DHT22);
Vegemite v;

uint32_t StartTime;
uint32_t DataCaptureDelay;

uint32_t getStartTime() {
    return millis() + DataCaptureDelay;
}

void setup() {
    Serial.begin(250000);
   
    dht.begin();
   
    DataCaptureDelay = 2000;
    StartTime = getStartTime();
}

void loop() {
    if (millis() > StartTime) {
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();
       
        if (!isnan(humidity) && !isnan(temperature)) {
            v.send("temperature", temperature);
            v.send("humidity", humidity);
        }
       
        StartTime = getStartTime();
    }
}