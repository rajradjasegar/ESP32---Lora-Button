/*
 * Ce programme va envoyer un message LoRa dès que le bouton sera appuyé
 */
 
#include <SPI.h>
#include <LoRa.h>
#include<Arduino.h>
#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`

//OLED pins to ESP32 GPIOs via this connecthin:
//OLED_SDA -- GPIO4
//OLED_SCL -- GPIO15
//OLED_RST -- GPIO16

 SSD1306  display(0x3c, 4, 15);



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

int counter = 0;

// Initialisation des constantes :
const int buttonPin = 2; // Numéro de la broche à laquelle est connecté le bouton poussoir
int buttonState = 0; // variable qui sera utilisée pour stocker l'état du bouton

void setup() {
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  display.init();

  // display.flipScreenVertically();

  display.setContrast(255);

  
  // indique que la broche buttonPin est une entrée :
  pinMode(buttonPin, INPUT);
  pinMode(25, OUTPUT);
 
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  //Serial.println("LoRa Sender");

  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Initial OK!");
}

void loop() {

  String msg;

// lit l'état du bouton et stocke le résultat
// dans buttonState :
buttonState = digitalRead(buttonPin);
// Si buttonState est maintenu appuyé (c'est à dire qu'il est à 5V) 
if (buttonState == HIGH) {

  // Initialize the log buffer
  // allocate memory to store 8 lines of text and 30 chars per line.
  // TRES IMPORTANT POUR LE NOMBRE DE LIGNE QUE L'ON SOUHAITE CONTROLER
  display.setLogBuffer(3, 30);
  display.clear();
  
  Serial.print("BTN PUSHED Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("BTN PUSHED : ");
  LoRa.print(counter);
  LoRa.endPacket();

  display.println("Sent packet : '");
  display.print("BTN PUSHED : '");
  display.println(counter);
  // Draw it to the internal screen buffer
  display.drawLogBuffer(0, 0);
  // Display it on the screen
  display.display();

  counter++;
  digitalWrite(25, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(25, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
  
}

}

