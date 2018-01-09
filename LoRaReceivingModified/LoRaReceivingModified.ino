#include <SPI.h>
#include <LoRa.h>
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
#define BAND    433E6

void setup() {
  
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  display.init();

  // display.flipScreenVertically();

  display.setContrast(255);

  Serial.begin(115200);
  while (!Serial); //if just the the basic function, must connect to a computer
  delay(1000);
  
  Serial.println("LoRa Receiver"); 
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  String msg= "";
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  
  if (packetSize) {
    
    // Initialize the log buffer
    // allocate memory to store 8 lines of text and 30 chars per line.
    // TRES IMPORTANT POUR LE NOMBRE DE LIGNE QUE L'ON SOUHAITE CONTROLER
    display.setLogBuffer(3, 30);
    display.clear();

    // received a packet
    Serial.print("Received packet '");
    display.println("Received packet '");
    
    // read packet
    while (LoRa.available()) {
      msg = msg + (char)LoRa.read();
      //Serial.print((char)LoRa.read());
    }
      Serial.print(msg);
      display.print(msg);
      display.println("");
      // Draw it to the internal screen buffer
      display.drawLogBuffer(0, 0);
      // Display it on the screen
      display.display();

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
