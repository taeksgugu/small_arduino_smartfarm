#include <DHT.h>
#include <AsyncTimer.h>
#include <Vegemite.h>

auto SOILMOIST_PIN = A6;
auto DHT22_PIN = A1;
auto PUMP_PIN = 16;
auto LAMP_PIN = 17;

DHT dht(DHT22_PIN, DHT22);
AsyncTimer t;
Vegemite v;

bool currentPumpWorking = false;

void setup() {
    Serial.begin(250000);
    dht.begin();
   
    pinMode(SOILMOIST_PIN, INPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(LAMP_PIN, OUTPUT);
   
    v.requestSubscription("config-light");
    v.requestSubscription("pump-water");
   
    // 1번 코드 블럭
    t.setInterval([]() {
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();
      if (!isnan(humidity) && !isnan(temperature)) {
          v.send("temperature", temperature);
          v.send("humidity", humidity);
      }
      
      int soilMoist = map(analogRead(SOILMOIST_PIN), 0, 1023, 100, 0);
      v.send("soilmoist", soilMoist);
    }, 1000);
   
    //2번 코드 블럭
    t.setInterval([]() {
      int pumpWater = int(v.recv("pump-water"));
      int lightConf = int(v.recv("config-light"));
      
      if (pumpWater == 1 && !currentPumpWorking) {
          currentPumpWorking = true;
          v.send("pump-water", 0);
          digitalWrite(PUMP_PIN, HIGH);
          t.setTimeout([]() {
              digitalWrite(PUMP_PIN, LOW);
              currentPumpWorking = false;
          }, 5000);
      }
      
      digitalWrite(LAMP_PIN, lightConf == 1 ? HIGH : LOW);
    }, 500);
}

void loop() {
    v.subscribe();
    t.handle();
}