#include<Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

 SSD1306  display(0x3c, 4, 15);



//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;

int ledPin = 25;                // choose the pin for the LED
int inputPin = 2;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status


int counter = 0;

#define SS      18
#define RST     14
#define DI0     26
#define BAND    433E6  //915E6 

void setup() {

  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  display.init();

  // display.flipScreenVertically();

  display.setContrast(255);
  
  pinMode(ledPin, OUTPUT);      // declare LED as output
  pinMode(inputPin, INPUT);     // declare sensor as input
  Serial.begin(115200);

  Serial.print("calibrating sensor ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }

  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  //Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");

  
}

void loop(){

  // Initialize the log buffer
  // allocate memory to store 8 lines of text and 30 chars per line.
  // TRES IMPORTANT POUR LE NOMBRE DE LIGNE QUE L'ON SOUHAITE CONTROLER
  display.setLogBuffer(3, 30);
  display.clear();
  
  val = digitalRead(inputPin);  // read input value
  Serial.println(val);
  if (val == HIGH) { // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    delay(150);

    if (pirState == LOW) {
      // we have just turned on
      Serial.println("Motion detected!");
      
      // send packet
      LoRa.beginPacket();
      LoRa.print("Motion detected! ");
      LoRa.print(counter);
      LoRa.endPacket();

      // display on screen
      display.print("Motion detected! ");
      display.println(counter);
      
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    delay(300);
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      display.print("Motion ended! ");
      display.println(counter);

      // send packet
      LoRa.beginPacket();
      LoRa.print("Motion ended! ");
      LoRa.print(counter);
      LoRa.endPacket();
      
      // We only want to print on the output change, not state
      pirState = LOW;
      counter++;
    }
  }

  // Draw it to the internal screen buffer
  display.drawLogBuffer(0, 0);
  // Display it on the screen
  display.display();
}
