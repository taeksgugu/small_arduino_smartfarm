#include <U8g2lib.h>
#include "DHT.h"
#include <SoftPWM.h>
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset= */ U8X8_PIN_NONE);

#define DHTPIN A1    /* DHTPIN 정의*/
#define DHTTYPE DHT22  /* DHTTYPE 정의*/
#define SOILHUMI A6    /* SOILHUMI 정의*/
#define PUMP 16       /* PUMP 정의*/
#define LAMP 17       /* LAMP 정의*/
SOFTPWM_DEFINE_CHANNEL(A3);
DHT dht(DHTPIN, DHTTYPE);

float Temp;
float Humi;
int Soilhumi = 0;
char Fancondition[4]; /* Fancondition 문자 배열 정의 */
char Lampcondition[4]; /* Lampcondition 문자 배열 정의 */
char Pumpcondition[4]; /* Pumpcondition 문자 배열 정의 */
char FPLstr[12];
char THSstr[16];
uint32_t DataCaptureDelay = 2000;   /* 2초 간격으로 데이터 측정 2초 = 2000*/
uint32_t DataCaptureDelay2 = 3000;   /* 3초 간격으로 데이터 측정 2초 = 3000*/
uint32_t DataCapture_ST = 0;    
uint32_t DataCapture_ST2 = 0;     
uint32_t TimeSum;
uint32_t TimeCompare;
uint32_t StartTime = 0;

void setup() {
  dht.begin();
  u8g2.begin();
  SoftPWM.begin(490);
  pinMode(PUMP, OUTPUT);
  pinMode(LAMP, OUTPUT);
  StartTime = millis();
  DataCapture_ST = millis();
  DataCapture_ST2 = millis();
}

void loop() {
  if ((millis() - DataCapture_ST) > DataCaptureDelay) {  /* 기존 OLED 코드에 토양 습도 변수 추가 */
    Humi = dht.readHumidity();
    Temp = dht.readTemperature();

  if (isnan(Humi) || isnan(Temp)) {   /* Humi, Temp 가 무한값일 때 걸러주는 코드 만약 조건에 해당되면 해당 loop 중단하고 다음 loop 시작 */
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  /* FAN 상태 control*/  
  if (Temp >= 29) {  /* DHT22의 온도가 29도 이상 측정시  */
    SoftPWM.set(65);
    strcpy(Fancondition, "ON"); /* Fancondition 상태 저장 */
  } else if (Temp <=20) { /* DHT22의 온도가 20도 이하 측정시  */
    SoftPWM.set(0);
    strcpy(Fancondition, "OFF"); /* Fancondition 상태 저장 */
  } else { /* DHT22의 온도가 20도 초과 29도 미만 측정시  */
    SoftPWM.set(65);
    strcpy(Fancondition, "ON"); /* Fancondition 상태 저장 */
  }  
  DataCapture_ST = millis();
  }
  /* LAMP 상태 control*/
  TimeSum = (uint32_t) random(1000, 5000);
  Serial.print(TimeSum);
  TimeCompare = (millis() - StartTime) / TimeSum;
  if (TimeCompare % 2 == 0) {
    digitalWrite(LAMP, HIGH);
    strcpy(Lampcondition, "ON");
  } else {
    digitalWrite(LAMP, LOW);
    strcpy(Lampcondition, "OFF");
  }



  /* PUMP 상태 control*/
  if ((millis() - DataCapture_ST2) > DataCaptureDelay2) {  /* 기존 OLED 코드에 토양 습도 변수 추가 */
    Soilhumi = map(analogRead(SOILHUMI), 0, 1023, 100, 0);

  if (Soilhumi <= 30) { /* 실험시에는 10으로 설정*/
    digitalWrite(PUMP, HIGH); /* 펌프 동작, digitalWrite에서 high는 1, low는 0으로 동작 정지를 뜻함 */ 
    strcpy(Pumpcondition, "ON");
  } else if (Soilhumi >= 60) { /* 실험시에는 30으로 설정*/
    digitalWrite(PUMP, LOW); /* 펌프 정지 */
    strcpy(Pumpcondition, "OFF");
  }
  DataCapture_ST2 = millis();
  }

  // FPLstr = Fancondition + Pumpcondition + Lampcondition;
  OLEDdraw();
}

void OLEDdraw() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_te);
  u8g2.drawStr(1, 15, "Taeksgugu");
  u8g2.drawStr(1, 36, "T/H/S");
  u8g2.setCursor(55, 36);
  u8g2.print((int)Temp);
  u8g2.setCursor(75, 36);
  u8g2.print((int)Humi);
  u8g2.setCursor(95, 36);
  u8g2.print(Soilhumi);


  u8g2.drawStr(1, 47, "F/P/L");
  u8g2.drawStr(55, 47, Fancondition);  /* Fancondition 상태가 문자열형이기 때문에 drawStr 사용 */
  u8g2.drawStr(75, 47, Pumpcondition);  /* Pupmondition 상태가 문자열형이기 때문에 drawStr 사용 */
  u8g2.drawStr(95, 47, Lampcondition);  /* Lampcondition 상태가 문자열형이기 때문에 drawStr 사용 */

  u8g2.drawStr(1, 58, "LED");
  u8g2.setCursor(55, 58);
  u8g2.print((int)TimeSum/1000);  /* Timesum을 초 단위로 변환 */
  u8g2.sendBuffer();
}