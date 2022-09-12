//nano every -> master
//at+role1

#include <SoftwareSerial.h>
SoftwareSerial HM10(2,3); //RX, TX -> arduino nano every 기준 (블루투스 모듈 기준 x)

void setup() {
  Serial.begin(9600); //PC-아두이노간 통신
  HM10.begin(9600); //아두이노-블루투스모듈간 통신
}

void loop() {
  //노트북 -> 아두이노 -> 블루투스
  if(Serial.available()){
    HM10.write(Serial.read());
  }

  //블루투스모듈 -> 아두이노 -> 노트북
  if(HM10.available()){
    Serial.write(HM10.read());
  }
} 
