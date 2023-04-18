#include <U8g2lib.h>
#include "DHT.h"
#include <SoftPWM.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= */ U8X8_PIN_NONE);

#define DHTPIN A1
#define DHTTYPE DHT22

SOFTPWM_DEFINE_CHANNEL(A3);
DHT dht(DHTPIN, DHTTYPE);

uint32_t DataCaptureDelay = 3000;
uint32_t DataCapture_ST = 0;

float Temp;
float Humi;
char Fancondition[4]; /* Fancondition 문자 배열 정의 */

void setup() {
  dht.begin();
  u8g2.begin();
  SoftPWM.begin(490);

  DataCapture_ST = millis();
}

void loop() {
  if ((millis() - DataCapture_ST) > DataCaptureDelay) {
    Humi = dht.readHumidity();
    Temp = dht.readTemperature();
  
  if (isnan(Humi) || isnan(Temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }



  if (Temp >= 30) {  /* DHT22의 온도가 30도 이상 측정시  */
    SoftPWM.set(100);
    delay(2000);
    strcpy(Fancondition, "ON"); /* Fancondition 상태 저장 */
  } else if (Temp <=25) { /* DHT22의 온도가 25도 이하 측정시  */
    SoftPWM.set(0);
    delay(2000);
    strcpy(Fancondition, "OFF"); /* Fancondition 상태 저장 */
  } else { /* DHT22의 온도가 25도 초과 30도 미만 측정시  */
    SoftPWM.set(65);
    delay(2000);
    strcpy(Fancondition, "ON"); /* Fancondition 상태 저장 */
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

  u8g2.drawStr(15, 58, "FAN");
  u8g2.setCursor(85, 58);
  u8g2.drawStr(85, 58, Fancondition);  /* Fancondition 상태가 문자열형이기 때문에 drawStr 사용 */
  u8g2.sendBuffer();
}