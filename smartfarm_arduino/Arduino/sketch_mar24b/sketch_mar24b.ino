#include "DHT.h"

#define DHTPIN A1
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

uint32_t DataCaptureDelay = 2000;
uint32_t StartTime = 0;

void setup() {
  Serial.begin(9600);
  dht.begin();
  StartTime = millis();
}

void loop() {
  if ((millis() - StartTime) > DataCaptureDelay) {
    float h = dht.readHumidity();
    float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("% Temperature: "));
  Serial.print(t);
  Serial.println(F("C"));

  StartTime = millis();
  }
}
