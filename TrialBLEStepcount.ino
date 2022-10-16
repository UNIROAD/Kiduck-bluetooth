#include <SoftwareSerial.h>   //Software Serial Port
#define RxD 4
#define TxD 5

#define MASTER 1    //change this macro to define the Bluetooth as Master or not 

#include <Wire.h>

SoftwareSerial blueToothSerial(RxD,TxD);//the software serial port 

char recv_str[100];
//=====================================//
//  *****  [걸음수] 변수 = steps  ****** //
//=====================================//
const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY;
int steps = 0;
int tmp_AcX;
int tmp_AcY;

void setup() 
{
    Serial.begin(9600);   //Serial port for debugging
    pinMode(RxD, INPUT);    //UART pin for Bluetooth
    pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
    Serial.println("\r\nPower on!!");
    setupBlueToothConnection(); //initialize Bluetooth
    
    // accel start----------------------------------
    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);
    // accel end---------------------------------
    
    //this block is waiting for connection was established.
//    while(1)
//    {
//        if(recvMsg(100) == 0)
//        {
//            if(strcmp((char *)recv_str, (char *)"OK+CONN") == 0)
//            {
//                Serial.println("connected\r\n");
//                break;
//            }
//        }
//        delay(200);
//    }
 sendBlueToothCommand("AT+START");//start module to take effect
} 

void loop() 
{
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
//    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)


    #if MASTER  //central role
    //in master mode, the bluetooth send message periodically. 
    delay(400);
    if (   (abs(tmp_AcX - AcX) > 3000)   ||    (  abs( tmp_AcY - AcY ) > 3000)    ){
                steps++;
             }

    blueToothSerial.println(steps);
    tmp_AcX = AcX;
    tmp_AcY = AcY;  
//    Serial.println("send: Steps");
//    blueToothSerial.print("hi");
    delay(100);
    //get any message to print
    if(recvMsg(1000) == 0)
    {
        Serial.print("recv: ");
        Serial.print((char *)recv_str);
        Serial.println("");
    }
    #else   //peripheral role
    delay(200);


    //the slave role only send message when received one.
    if(recvMsg(1000) == 0)
    {
        Serial.print("recv: ");
        Serial.print((char *)recv_str);
        Serial.println("");
        Serial.println("send: ACK");
        blueToothSerial.print("ACK");//return back message
    }
    #endif

}

//used for compare two string, return 0 if one equals to each other
int strcmp(char *a, char *b)
{
    unsigned int ptr = 0;
    while(a[ptr] != '\0')
    {
        if(a[ptr] != b[ptr]) return -1;
        ptr++;
    }
    return 0;
}

//configure the Bluetooth through AT commands
int setupBlueToothConnection()
{
    #if MASTER
    Serial.println("this is MASTER\r\n");
    #else
    Serial.println("this is SLAVE\r\n");
    #endif

    Serial.print("Setting up Bluetooth link\r\n");
    delay(2000);//wait for module restart
    blueToothSerial.begin(9600);

    //wait until Bluetooth was found
    while(1)
    {
        if(sendBlueToothCommand("AT") == 0)
        {
            if(strcmp((char *)recv_str, (char *)"OK") == 0)
            {
                Serial.println("Bluetooth exists\r\n");
                break;
            }
        }
        delay(500);
    }

    //configure the Bluetooth
    sendBlueToothCommand("AT+DEFAULT");//restore factory configurations
    delay(2000);
    //sendBlueToothCommand("AT+AUTH1");//enable authentication
    //sendBlueToothCommand("AT+PASS123456");//set password
    //sendBlueToothCommand("AT+NOTI1");//set password
    //set role according to the macro
    sendBlueToothCommand("AT+NAME?");
    //sendBlueToothCommand("AT+NAMEZETAHM-10");
    #if MASTER
    sendBlueToothCommand("AT+ROLEM");//set to master mode
    #else
    sendBlueToothCommand("AT+ROLES");//set to slave mode
    #endif
    sendBlueToothCommand("AT+RESTART");//restart module to take effect
    delay(2000);//wait for module restart

    

    //check if the Bluetooth always exists
    if(sendBlueToothCommand("AT") == 0)
    {
        if(strcmp((char *)recv_str, (char *)"OK") == 0)
        {
            Serial.print("Setup complete\r\n\r\n");
            return 0;;
        }
    }
    
    return -1;
}

//send command to Bluetooth and return if there is a response received
int sendBlueToothCommand(char command[])
{
    Serial.print("send: ");
    Serial.print(command);
    Serial.println("");

    blueToothSerial.print(command);
    delay(300);

    if(recvMsg(200) != 0) return -1;

    Serial.print("recv: ");
    Serial.print(recv_str);
    Serial.println("");
    return 0;
}

//receive message from Bluetooth with time out
int recvMsg(unsigned int timeout)
{
    //wait for feedback
    unsigned int time = 0;
    unsigned char num;
    unsigned char i;

    //waiting for the first character with time out
    i = 0;
    while(1)
    {
        delay(50);
        if(blueToothSerial.available())
        {
            recv_str[i] = char(blueToothSerial.read());
            i++;
            break;
        }
        time++;
        if(time > (timeout / 50)) return -1;
    }

    //read other characters from uart buffer to string
    while(blueToothSerial.available() && (i < 100))
    {                                              
        recv_str[i] = char(blueToothSerial.read());
        i++;
    }
    recv_str[i] = '\0';

    return 0;
}
