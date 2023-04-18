#include <U8g2lib.h>
#include "DHT.h"
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= */ U8X8_PIN_NONE);

#define DHTPIN A1    /* DHTPIN 정의*/
#define DHTTYPE DHT22  /* DHTTYPE 정의*/
#define SOILHUMI A6    /* SOILHUMI 정의*/
#define PUMP 16
DHT dht(DHTPIN, DHTTYPE);

uint32_t DataCaptureDelay = 3000;   /* 3초 간격으로 데이터 측정 3초 = 3000*/
uint32_t DataCapture_ST = 0;     

float Temp;
float Humi;
int Soilhumi = 0;

void setup() {
  dht.begin();
  u8g2.begin();
  pinMode(PUMP, OUTPUT);
  DataCapture_ST = millis();
}

void loop() {
  if ((millis() - DataCapture_ST) > DataCaptureDelay) {  /* 기존 OLED 코드에 토양 습도 변수 추가 */
    Humi = dht.readHumidity();
    Temp = dht.readTemperature();
    Soilhumi = map(analogRead(SOILHUMI), 0, 1023, 100, 0);  

  if (isnan(Humi) || isnan(Temp)) {   /* Humi, Temp 가 무한값일 때 걸러주는 코드 만약 조건에 해당되면 해당 loop 중단하고 다음 loop 시작 */
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  if (Soilhumi <= 30) { /* 실험시에는 10으로 설정*/
    digitalWrite(PUMP, HIGH); /* 펌프 동작, digitalWrite에서 high는 1, low는 0으로 동작 정지를 뜻함 */ 
    delay(2000);
  } else if (Soilhumi >= 60) { /* 실험시에는 30으로 설정*/
    digitalWrite(PUMP, LOW); /* 펌프 정지 */
    delay(2000);
  }

  OLEDdraw();
  DataCapture_ST = millis();
  }
}

void OLEDdraw() {
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_ncenB08_te);
  u8g2.drawStr(1, 15, "Taeksgugu");

  u8g2.drawStr(15, 36, "Temp.");
  u8g2.setCursor(85, 36);
  u8g2.print(Temp);
  u8g2.drawStr(114, 36, "\xb0");
  u8g2.drawStr(119, 36, "C");

  u8g2.drawStr(15, 47, "Humidity");
  u8g2.setCursor(85, 47);
  u8g2.print(Humi);
  u8g2.drawStr(116, 47, "%");

  u8g2.drawStr(15, 58, "Soil Humid");  /* 원래 Soil Humidity라고 적었는데 OLED 화면에서 수치와 겹치는 현상 발생해서 Humid로 줄임 */
  u8g2.setCursor(85, 58);
  u8g2.print(Soilhumi);
  u8g2.drawStr(116, 58, "%");

  u8g2.sendBuffer();
}