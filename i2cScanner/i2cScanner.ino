#include <Wire.h>

void setup()
{
  // run next line for NODEMCU only
//  Wire.pins(0, 2);
  Wire.begin();

  Serial.begin(9600);
  while (!Serial);  // Leonardo: wait for serial monitor
}


void loop()
{
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");

  nDevices = 0;
  for(address = 1; address < 127; address++ ) 
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0)
    {
      Serial.print("I2C device found at 0x");
      if (address<16) 
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("  !");
      nDevices++;
    }
    else if (error==4) 
    {
      Serial.print("Unknown error at 0x");
      if (address<16) 
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No devices found\n");
  else
    Serial.println("done\n");
  delay(5000);            
}
