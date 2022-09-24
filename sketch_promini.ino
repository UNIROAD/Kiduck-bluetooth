/*
  arduino pro mini(슬레이브코드)
*/
#include <SoftwareSerial.h>

int trigPin = 6;
int echoPin = 7;

int L=6; //물통 바닥 길이

SoftwareSerial HM10(4, 5);

void setup() {
  Serial.begin(9600); //PC-아두이노간
  HM10.begin(9600); //아두이노-블루투스모듈
  pinMode(echoPin, INPUT);   // echoPin 입력    
  pinMode(trigPin, OUTPUT);  // trigPin 출력   
}


void loop() { // run over and over
  
  long duration, distance, water, r;
  digitalWrite(trigPin, HIGH);  // trigPin에서 초음파 발생(echoPin도 HIGH)        
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);    // echoPin 이 HIGH를 유지한 시간을 저장 한다.
  distance = ((float)(340 * duration) / 1000) / 2 ; 
  water = L*L*(120-distance)/ 10; //물 부피 계산
 
  delay(3000);

  if(Serial.available()){
    HM10.println(water);
   }
}
