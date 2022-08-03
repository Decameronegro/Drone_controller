/*****************************************************************************
 * DRONE_control_version1p1
 *****************************************************************************
 * 
 * Replaced the analog joystick for a pair of buttons to rise up and down the
 * BLDC speeds. 
 * PWM cycle ranges between 1000 and 2000 us, value which will be maped between
 * 0 and 99% (to fit a 2-digit 7-segment display.
 */
 
#ifndef _DEBUG
#define _DEBUG
#endif

#define CE_PIN          9
#define CSN_PIN         10
#define UP_BUTTON       2
#define DOWN_BUTTON     3

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <printf.h>

struct Data_Package {
  uint16_t xPos1;
  uint16_t xPos2;
  uint16_t yPos1;
  uint16_t yPos2;
};

Data_Package datos;

const byte slaveAddress[5] = {'R', 'x', 'A', 'A', 'A'};
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio

volatile int localValue = 0;
volatile bool diodo = false;
volatile int remoteValue = 0;
//volatile bool upButton = false;
//volatile bool downButton = false;

void setup() {
#ifdef _DEBUG
  Serial.begin(115200);
  while(!Serial) {};
  Serial.println("SimpleTx Starting");
#endif

  printf_begin();

  radio.begin();
  radio.setDataRate( RF24_250KBPS );
  radio.setRetries(3, 5); // delay, count
  radio.openWritingPipe(slaveAddress);
  radio.printDetails();
  reset_data();
#ifdef _DEBUG
  Serial.println("Initial data on RX side:");
  print_datos();
  //while(1) {};
#endif
  pinMode(UP_BUTTON,INPUT_PULLUP);
  pinMode(DOWN_BUTTON,INPUT_PULLUP);
  //pinMode(CONFIRM_BTN,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(UP_BUTTON), upInt, FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWN_BUTTON), downInt, FALLING);
  //attachInterrupt(digitalPinToInterrupt(CONFIRM_BTN), confInt, FALLING);
}


//====================
void reset_data() {
  // Reset the values of the data variable to the initial point
  datos.xPos1 = 0;
  datos.xPos2 = 0;
  datos.yPos1 = 0;
  datos.yPos2 = 0;
}

//====================
#ifdef _DEBUG
void print_datos() {
  Serial.print("Setting engines to ");
  Serial.print(datos.xPos1);
  Serial.println(" %");
  /*Serial.println(datos.xPos2);
  Serial.println(datos.yPos1);
  Serial.println(datos.yPos2);*/
}
#endif

//====================

void loop() {
  delay(500);
  send();
  //Serial.println(remoteValue);
  //update_datos();
  //print_datos();
}

//====================

void update_datos() {
  datos.xPos1 = localValue;
  datos.yPos1 = 0;
  datos.xPos2 = 0;
  datos.yPos2 = 0;
}

//====================
void send() {

  update_datos();
  bool rslt;
  rslt = radio.write( &datos, sizeof(datos) );
  // Always use sizeof() as it gives the size as the number of bytes.
  // For example if dataToSend was an int sizeof() would correctly return 2
#ifdef _DEBUG
  if (rslt) {
    Serial.print("ACK - ");
    //updateMessage();
    print_datos();
  }
  else {
    Serial.print("TX FAILED|");
  }
#endif
}

//================

void upInt() {
  //upButton = true;
  if(localValue < 99) localValue++;
  diodo = !diodo;
}

void downInt() {
  //downButton = true;
  if(localValue > 0) localValue--;
  diodo = !diodo;
}

/*void confInt() {
  diodo = true;
  remoteValue = localValue;
}*/
