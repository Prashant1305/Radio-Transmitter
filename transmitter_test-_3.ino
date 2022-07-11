/*
        DIY Arduino based RC Transmitter
  by Dejan Nedelkovski, www.HowToMechatronics.com
  Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
// Define the digital inputs
#define jB1 1  // Joystick button 1
#define jB2 0  // Joystick button 2
#define t1 7   // Toggle switch 1
#define t2 4   // Toggle switch 2
#define t3 8   // Toggle switch 3
#define t4 9   // Toggle switch 4
#define t5 2   // Toggle switch 5
#define t6 3   // Toggle switch 6 


RF24 radio(5, 6);   // nRF24L01 (CE, CSN)
const byte address[6] = "00001"; // Address
// Max size of this struct is 32 bytes - NRF24L01 buffer limit
struct Data_PackageD {
  short j1PotX;
  short j1PotY;
  byte j1Button;
  short j2PotX;
  short j2PotY;
  byte j2Button;
  short pot1;
  short pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte tSwitch3;
  byte tSwitch4;
  byte tSwitch5;
  byte tSwitch6;
};
struct Data_Package {
  byte j1PotX;
  byte j1PotY;
  byte j1Button;
  byte j2PotX;
  byte j2PotY;
  byte j2Button;
  byte pot1;
  byte pot2;
  byte tSwitch1;
  byte tSwitch2;
  byte tSwitch3;
  byte tSwitch4;
  byte tSwitch5;
  byte tSwitch6;
};
Data_PackageD dataD; //Create a variable with the above structure for drone
Data_Package data;
void setup() {
  Serial.begin(9600);
  // Define the radio communication
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MAX);
  radio.stopListening();

  // Activate the Arduino internal pull-up resistors
  pinMode(jB1, INPUT_PULLUP);
  pinMode(jB2, INPUT_PULLUP);
  pinMode(t1, INPUT);
  pinMode(t2, INPUT);
  pinMode(t3, INPUT);
  pinMode(t4, INPUT);
  pinMode(t5, INPUT);
  pinMode(t6, INPUT);
}
void loop() {
  if (digitalRead(t4) == 1) // drone mode
  { // Read all digital inputs
    dataD.j1Button = digitalRead(jB1);
    dataD.j2Button = digitalRead(jB2);
    dataD.tSwitch1 = digitalRead(t1);
    dataD.tSwitch2 = digitalRead(t2);
    dataD.tSwitch3 = digitalRead(t3);
    dataD.tSwitch4 = digitalRead(t4);
    dataD.tSwitch5 = digitalRead(t5);
    dataD.tSwitch6 = digitalRead(t6);
    // Read all analog inputs and map them to one Byte value
    dataD.j1PotX = map(analogRead(A1), 0, 1023, 0, 1000); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 1000
    dataD.j1PotY = map(analogRead(A0), 0, 1023, 0, 1000);
    dataD.j2PotX = map(analogRead(A2), 0, 1023, 0, 1000);
    dataD.j2PotY = map(analogRead(A3), 0, 1023, 0, 1000);
    dataD.pot1 = map(analogRead(A7), 0, 1023, 0, 1000);
    dataD.pot2 = map(analogRead(A6), 0, 1023, 0, 1000);
    float refin=dataD.pot2/997.0;
    
    int diffj1poty=(dataD.j1PotY-508)*refin;
    dataD.j1PotY=diffj1poty+508;

    int diffj1potx=(dataD.j1PotX-493)*refin;
    dataD.j1PotX=diffj1potx+493;

    dataD.j2PotX = (dataD.j2PotX * (dataD.pot1 / 1000.0)) ; // multiplying with pot1

    Serial.print(dataD.j1PotY);
    Serial.print(dataD.j2PotY);

    // Send the whole data from the structure to the receiver
    radio.write(&dataD, sizeof(Data_PackageD));
    delay(1);
  }
  else // car mode
  {
    // Read all analog inputs and map them to one Byte value
    data.j1PotX = map(analogRead(A1), 0, 1023, 0, 255); // Convert the analog read value from 0 to 1023 into a BYTE value from 0 to 255
    data.j1PotY = map(analogRead(A0), 0, 1023, 0, 255);
    data.j2PotX = map(analogRead(A2), 0, 1023, 255, 0);
    data.j2PotY = map(analogRead(A3), 0, 1023, 255, 0);
    data.pot1 = map(analogRead(A7), 0, 1023, 0, 255);
    data.pot2 = map(analogRead(A6), 0, 1023, 0, 255);
    Serial.print(data.j1PotY);
    Serial.print(data.j2PotY);
    // Read all digital inputs
    data.j1Button = digitalRead(jB1);
    data.j2Button = digitalRead(jB2);
    data.tSwitch1 = digitalRead(t1);
    data.tSwitch2 = digitalRead(t2);
    data.tSwitch3 = digitalRead(t3);
    data.tSwitch4 = digitalRead(t4);
    data.tSwitch5 = digitalRead(t5);
    data.tSwitch6 = digitalRead(t6);
    // Send the whole data from the structure to the receiver
    radio.write(&data, sizeof(Data_Package));
    delay(1);
  }
}
