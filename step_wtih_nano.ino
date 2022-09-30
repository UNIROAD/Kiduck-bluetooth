//nano every -> master
//at+role1

#include <SoftwareSerial.h>
#include <Wire.h>
//#include <Scheduler.h>
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int steps = 0;
int tmp_AcX;
int tmp_AcY;


SoftwareSerial HM10(4,5); //TX, RX -> arduino nano every 기준 (블루투스 모듈 기준 x)

// ===============================================================
void setup() {
  // BLTH start---------------------------------
  Serial.begin(9600); //PC-아두이노간 통신
  HM10.begin(9600); //아두이노-블루투스모듈간 통신
  // BLTH end---------------------------------

  // accel start----------------------------------
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  // accel end---------------------------------
 
  Serial.println(1111);
  
//    while(Serial.available()){
////    Serial.write(Serial.read());
//    HM10.write(Serial.read());
//  }
// 
//
//  //블루투스모듈 -> 아두이노 -> 노트북
//  while(HM10.available()){
//    Serial.write(HM10.read());
////    Serial.print(HM10.read());
//  }


//  HM10.write("AT");
//  while(1){
//    HM10.write("AT+START");
//    Sti
//    while(HM10.available()){
//        
//    }
//  }
}
// ==================================================================

void loop() {
  //노트북 -> 아두이노 -> 블루투스
  
  while(Serial.available()){
////    Serial.write(Serial.read());
    HM10.write(Serial.read());
  }
// 
//
//  //블루투스모듈 -> 아두이노 -> 노트북
  while(HM10.available()){
    Serial.write(HM10.read());
//    Serial.print(HM10.read());
  }
  
// ------------------------------------------------------------
  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  if (   (abs(tmp_AcX - AcX) > 3000)   ||    (  abs( tmp_AcY - AcY ) > 3000)    ){
                steps++;
             }
  Serial.println(steps);
  tmp_AcX = AcX;
  tmp_AcY = AcY;
  delay(777);

   
//  HM10.println(steps);
   

} 
