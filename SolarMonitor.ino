#include "NETSGPClient.h"
#include "heltec.h"
#include "Arduino.h"

//pins set for Heltec Wifi32 with onbard oled

constexpr const uint8_t PROG_PIN = 0; /// Programming enable pin of RF module
constexpr const uint8_t RX_PIN = 5; /// RX pin of ESP32 connect to TX of RF module
constexpr const uint8_t TX_PIN = 17; /// TX pin of ESP32 connect to RX of RF module
constexpr const uint32_t inverterID = 0x41004030; /// Identifier of your inverter (see label on inverter)

//#if defined(ESP32)
// On ESP32 debug output is on Serial and RF module connects to Serial2
#define debugSerial Serial
#define clientSerial Serial2


String dcoutput="0";
String acoutput="0";
String currTemp="0";
String totalPower = "0";

//#else
// On ESP8266 or other debug output is on Serial1 and RF module connects to Serial
// On D1 Mini connect RF module to pins marked RX and TX and use D4 for debug output
//#define debugSerial Serial1
//#define clientSerial Serial
//#endif

NETSGPClient client(clientSerial, PROG_PIN); /// NETSGPClient instance
SSD1306Wire display(0x3c, SDA_OLED, SCL_OLED, RST_OLED);

void VextON(void)
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, LOW);
  
}

void VextOFF(void) //Vext default OFF
{
  pinMode(Vext,OUTPUT);
  digitalWrite(Vext, HIGH);
  
}

void setup()
{
    debugSerial.begin(115200);
    //if defined(ESP32)
    clientSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    #//else
    //clientSerial.begin(9600);
    //#endif

    delay(1000);
    debugSerial.println("Welcome to Micro Inverter Interface by ATCnetz.de and enwi.one");


    // Make sure the RF module is set to the correct settings
    if (!client.setDefaultRFSettings())
    {
        debugSerial.println("Could not set RF module to default settings");
    }

//display on
VextON();
display.init();
display.clear();
display.display();
display.setContrast(255);


}

uint32_t lastSendMillis = 0;

void loop()
{


  
    const uint32_t currentMillis = millis();
    if (currentMillis - lastSendMillis > 2000)
    {
        lastSendMillis = currentMillis;

       
        debugSerial.println("");
        debugSerial.println("Sending request now");

        const NETSGPClient::InverterStatus status = client.getStatus(inverterID);
        if (status.valid)
        {   
  
          
  
            debugSerial.println("*********************************************");
            debugSerial.println("Received Inverter Status");
            debugSerial.print("Device: ");
            debugSerial.println(status.deviceID, HEX);
            debugSerial.println("Status: " + String(status.state));
            debugSerial.println("DC_Voltage: " + String(status.dcVoltage) + "V");
            debugSerial.println("DC_Current: " + String(status.dcCurrent) + "A");
            debugSerial.println("DC_Power: " + String(status.dcPower) + "W");
            debugSerial.println("AC_Voltage: " + String(status.acVoltage) + "V");
            debugSerial.println("AC_Current: " + String(status.acCurrent) + "A");
            debugSerial.println("AC_Power: " + String(status.acPower) + "W");
            debugSerial.println("Power gen total: " + String(status.totalGeneratedPower));
            debugSerial.println("Temperature: " + String(status.temperature));
            currTemp="Temperature: " + String(status.temperature)+"c";

            //send to display
            dcoutput="DC Power: " + String(status.dcPower) + "W";
            acoutput="AC Power: " + String(status.acPower) + "W";
            currTemp="Temperature: " + String(status.temperature)+"c";
            totalPower="Total Power Gen: " + String(status.totalGeneratedPower) + "KWh";
            
             display.clear();
             display.display();
             display.screenRotate(ANGLE_0_DEGREE);
             display.setFont(ArialMT_Plain_10);
             
             //and display
             display.drawString(0, 0, dcoutput);
             display.drawString(0, 15, acoutput);
             display.drawString(0, 30, totalPower);
             display.drawString(0, 45, currTemp);
             display.display();
             //wait 20 sec before re-sample
             delay(20000);


        }
    }
}
