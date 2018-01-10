/*
 ESP32  va envoyer un message LoRa toutes les minutes 
 */
#include <SPI.h>
#include <LoRa.h>
// Stop button is attached to PIN 0 (IO0)
#define BTN_STOP_ALARM    0

hw_timer_t * timer = NULL;
int counter = 1;
// WIFI_LoRa_32 ports

// GPIO5  -- SX1278's SCK
// GPIO19 -- SX1278's MISO
// GPIO27 -- SX1278's MOSI
// GPIO18 -- SX1278's CS
// GPIO14 -- SX1278's RESET
// GPIO26 -- SX1278's IRQ(Interrupt Request)

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 

void loraSendMessage(){

  //Serial.print("Sending packet: " + String(counter) + "at :" + String(millis()));
  //Serial.print(counter);

  Serial.print("Sending Packet");
  //String msg = "Sending packet: " + String(counter) + "at :" + String(millis());
  // send packet
  LoRa.beginPacket();
  LoRa.print("Sending packet");
  //LoRa.print(counter);
  //LoRa.print(counter);
  LoRa.endPacket();

  counter++;
}

void onTimer(){
//static unsigned int counter = 1;
  Serial.print("onTimer no. ");
  //Serial.print(counter);
  Serial.print(" at ");
  //Serial.print(millis());
  Serial.println(" ms");

  counter++;
}

void setup() {
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer
  
  // Set BTN_STOP_ALARM to input mode
  pinMode(BTN_STOP_ALARM, INPUT);

  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  //Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");

    // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &loraSendMessage, true);

  // Set alarm to call onTimer function every second (value in microseconds).
  // Repeat the alarm (third parameter)
  timerAlarmWrite(timer, 60000000, true);

  // Start an alarm
  timerAlarmEnable(timer);
}

void loop() {
  // If button is pressed
  if (digitalRead(BTN_STOP_ALARM) == LOW) {
    // If timer is still running
    if (timer) {
      // Stop and free timer
      timerEnd(timer);
      timer = NULL;
      Serial.println("Arrêt du timer !!!");
    }
  }
}
