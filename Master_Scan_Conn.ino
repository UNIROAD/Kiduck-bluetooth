#include <SoftwareSerial.h>   //Software Serial Port

#define RxD 4
#define TxD 5

SoftwareSerial blueToothSerial(RxD, TxD); //the software serial port

char recv_str[100];

// no changed funtions
int recvMsg(unsigned int timeout);
int recvcmp(const char *str);
int strcmp(char *a, const char *b);

// new functions to make a BLE connection with the bottle
int connectWithBottle(); // Make a BLE Connetion with the Kiduck bottle
int scanBottle(); // Scan BLE Device and Return the Index of the Kidcuk Bottle
char readChar(); // Read only one Character from BLE connection
bool cmpMsg(String msg, int from = 0); // Compare Message to Characters which are collected by readChar()

// modified functions fit to the bottle
int sendBlueToothCommand(char command[], int timeout = 200);// add "timeout"
void bleSetup(); // BLE setup for the connection with Bottle
int setupBlueToothConnection(); // init ble settings

void setup() {
  Serial.begin(9600);
  bleSetup();
}

int drink = 0;

void loop() {
  if(recvMsg(200) == 0){
    if(recvcmp("OK+LOST") == 0) Serial.println("disconnected");
    Serial.println(recv_str);
    Serial.print("total: ");
    drink += int(recv_str);
    Serial.println(drink);
  }  
}

void bleSetup(){
  pinMode(RxD, INPUT);    //UART pin for Bluetooth
  pinMode(TxD, OUTPUT);   //UART pin for Bluetooth
  Serial.println("\r\nPower on!!");
  setupBlueToothConnection(); //initialize Bluetooth
  //this block is waiting for connection was established.
  if(connectWithBottle() == 0){
    Serial.println("connected");
  } else {
    Serial.println("fail to connect");
  }
}


int setupBlueToothConnection(){
  Serial.print("Setting up Bluetooth link\r\n");
  delay(2000);//wait for module restart
  blueToothSerial.begin(9600);

  //wait until Bluetooth was found
  while (1){
    delay(500);
    if (sendBlueToothCommand("AT") != 0) continue;
    if (recvcmp("OK") == 0) break;
  }
  Serial.println("Bluetooth exists\r\n");

  //configure the Bluetooth
  sendBlueToothCommand("AT+RENEW");//restore factory configurations
  delay(2000);
  sendBlueToothCommand("AT+IMME1");
  sendBlueToothCommand("AT+NAMEKIDUCK");
  sendBlueToothCommand("AT+ROLE1");//set to slave mode
  sendBlueToothCommand("AT+RESTART");//restart module to take effect
  delay(2000);//wait for module restart

  //check if the Bluetooth always exists
  if (sendBlueToothCommand("AT") == 0){
    if (recvcmp("OK") == 0){
      Serial.print("Setup complete\r\n\r\n");
      return 0;
    }
  }
  return -1;
}

int connectWithBottle(){
  String connectMsg = "AT+CONN";
  String confirmMsg = "OK+CONN";
  
  int bottleIndex = scanBottle();
  if(bottleIndex < 0) return -1;

  connectMsg += String(bottleIndex);
  confirmMsg += String(bottleIndex) + confirmMsg;
  if(sendBlueToothCommand(connectMsg.c_str(), 3000) == 0){
    if(recvcmp(confirmMsg.c_str()) == 0){
      Serial.println("connected\r\n");
      return 0;
    }      
  }
  
  return -1;
}

int scanBottle(){
  String RECV_OK = "OK+";
  String SCAN_FINISH = "DISCE";
  String SCAN_NAME = "NAME:";
  String KIDUCK_BOTTLE = "KDB";
  int listCount = 0;
  int index = -1;
  if(sendBlueToothCommand("AT+DISC?", 3000) == 0){
    if(recvcmp("OK+DISCS") == 0){
      while(1){
        if(!cmpMsg(RECV_OK)) continue;
        Serial.println("OK");
        char nextChar = readChar();
        if(nextChar == 'D'){
          if(!cmpMsg(SCAN_FINISH, 1)) continue;
          Serial.println("END");
          break;
        } else if(nextChar == 'N'){
          if(!cmpMsg(SCAN_NAME, 1)) continue;
          Serial.println("NAME");
          listCount++;
          if(cmpMsg(KIDUCK_BOTTLE)) {index = listCount - 1;}          
        }
      }
    }
  }
  return index;
}

char readChar(){  
  while(1){
    if(blueToothSerial.available()){
      return char(blueToothSerial.read());
    }    
  }
}

bool cmpMsg(String msg, int from = 0){
  // Serial.print("cmpMsg: ");
  // Serial.println(msg);
  for(int i=from;i<msg.length();i++){
    if(readChar() != msg[i]) return false;
  }
  return true;
}

int sendBlueToothCommand(char command[], int timeout = 200){
  Serial.print("send: ");
  Serial.println(command);

  blueToothSerial.print(command);
  delay(300);

  if (recvMsg(timeout) != 0) return -1;

  Serial.print("recv: ");
  Serial.println(recv_str);
  return 0;
}

int recvMsg(unsigned int timeout){
  //wait for feedback
  unsigned char num;
  unsigned char len = 100;
  int i = 0;

  //waiting for the first character with time out
  for(unsigned int time = 0;!blueToothSerial.available();time++){
    delay(50);
    if (time > (timeout / 50)) return -1;
  }

  //read other characters from uart buffer to string
  for(;blueToothSerial.available() && (i < 100);i++)
    recv_str[i] = char(blueToothSerial.read());

  recv_str[i] = '\0';

  return 0;
}

int recvcmp(const char *str) {return strcmp((char *)recv_str, str);}

int strcmp(char *a, const char *b){
  for(unsigned int ptr = 0; a[ptr] != '\0'; ptr++){
    if (a[ptr] != b[ptr]) return -1;
  }
  return 0;
}