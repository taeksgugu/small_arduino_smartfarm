#include <DHT.h>
#include <AsyncTimer.h>
#include <Vegemite.h>
#include <SoftPWM.h>

auto SOILMOIST_PIN = A6;
auto DHT22_PIN = A1;
auto FAN_PIN = A3;
auto PUMP_PIN = 16;
auto LAMP_PIN = 17;

SOFTPWM_DEFINE_CHANNEL(FAN_PIN);
DHT dht(DHT22_PIN, DHT22);
AsyncTimer t;
Vegemite v;

bool currentPumpWorking = false;

void setup() {
    Serial.begin(250000);
    SoftPWM.begin(490);
    dht.begin();
   
    pinMode(SOILMOIST_PIN, INPUT);
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(LAMP_PIN, OUTPUT);
   
    v.requestSubscription("config-light");
    v.requestSubscription("config-fan");
    v.requestSubscription("pump-water");
    v.requestSubscription("config-auto"); /* auto mode를 파악하기 위해 값을 가져옴*/
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
      int fanSpeed = int(v.recv("config-fan")*65); /* SliderControl에서 0,1,2로 나눠서 65를 곱해 */
      int autoConf = int(v.recv("config-auto"));  /*  */
      float humidity = dht.readHumidity();
      float temperature = dht.readTemperature();
      int soilMoist = map(analogRead(SOILMOIST_PIN), 0, 1023, 100, 0);
      if (autoConf != 1) {
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
        SoftPWM.set(fanSpeed);
      } else {
        digitalWrite(LAMP_PIN, temperature < 25 ? HIGH : LOW); /* 온도가 25보다 낮을 때 켜짐 */
        digitalWrite(PUMP_PIN, soilMoist < 40 ? HIGH : LOW); /* 습도가 40보다 낮을 때 켜짐 */
        if (humidity > 75) { /* 습도 75보다 높을 때 켜짐 */
          SoftPWM.set(100);
        } else {
          SoftPWM.set(0);
        }
      }


    }, 500);
}

void loop() {
    v.subscribe();
    t.handle();
}