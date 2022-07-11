/*
  Read NMEA sentences over sERIAL using Ublox module SAM-M8Q, NEO-M8P, etc
  Base on SparkFun_Ublox_Arduino_Library //https://github.com/sparkfun/SparkFun_Ublox_Arduino_Library
*/

#include "SparkFun_Ublox_Arduino_Library.h"
#include "boards.h"

SFE_UBLOX_GPS myGPS;

#include <MicroNMEA.h> //https://github.com/stevemarple/MicroNMEA

char nmeaBuffer[100];
MicroNMEA nmea(nmeaBuffer, sizeof(nmeaBuffer));
int count = 0;
void setup()
{
  initBoard();
  // When the power is turned on, a delay is required.
  delay(1500);

  Serial.println("SparkFun Ublox Example");

  if (myGPS.begin(Serial1) == false) {
    Serial.println(F("Ublox GPS not detected at default I2C address. Please check wiring. Freezing."));
    while (1);
  }
}

void loop()
{
  myGPS.checkUblox(); //See if new data is available. Process bytes as they come in.
  delay(100);
  if (nmea.isValid() == true) {
    long latitude_mdeg = nmea.getLatitude();
    long longitude_mdeg = nmea.getLongitude();
    Serial.print("Latitude (deg): ");
    Serial.println(latitude_mdeg / 1000000., 6);
    Serial.print("Longitude (deg): ");
    Serial.println(longitude_mdeg / 1000000., 6);

    if (u8g2) {
      char buf[256];
      u8g2->clearBuffer();
      u8g2->drawStr(0, 12, "GPS: OK!");
      u8g2->sendBuffer();
    }
  } else {
    Serial.print("No Fix - ");
    Serial.print("Num. satellites: ");
    Serial.println(nmea.getNumSatellites());
    
    if (u8g2) {
      char buf[256];
      u8g2->clearBuffer();
      u8g2->drawStr(0, 12, "No Fix");
      snprintf(buf, sizeof(buf), "satellites : %d", nmea.getNumSatellites());
      u8g2->drawStr(0, 30, buf);
      snprintf(buf, sizeof(buf), "Counter : %d", count);
      u8g2->drawStr(0, 48, buf);
      u8g2->sendBuffer();
      count++;
    }
  }

  delay(1000); //Don't pound too hard on the I2C bus
}

//This function gets called from the SparkFun Ublox Arduino Library
//As each NMEA character comes in you can specify what to do with it
//Useful for passing to other libraries like tinyGPS, MicroNMEA, or even
//a buffer, radio, etc.
void SFE_UBLOX_GPS::processNMEA(char incoming)
{
  //Take the incoming char from the Ublox I2C port and pass it on to the MicroNMEA lib
  //for sentence cracking
  nmea.process(incoming);
}
