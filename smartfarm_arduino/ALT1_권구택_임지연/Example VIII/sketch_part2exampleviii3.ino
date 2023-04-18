#include <DHT.h>
#include <AsyncTimer.h>
#include <Vegemite.h>

DHT dht(A1, DHT22);
AsyncTimer t;
Vegemite v;

void setup() {
    Serial.begin(250000);
   
    dht.begin();
   
    t.setInterval([] {
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();
       
        if (!isnan(humidity) && !isnan(temperature)) {
            v.send("temperature", temperature);
            v.send("humidity", humidity);
        }
    }, 2000);
}

void loop() {
    t.handle();
}