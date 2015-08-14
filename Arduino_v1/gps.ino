#include <SoftwareSerial.h>
#include <Adafruit_GPS.h>
SoftwareSerial gpsSoftSerial(4, 5);
Adafruit_GPS gps(&gpsSoftSerial);

#define GPSECHO  false
#define GPSLOG false
boolean usingInterrupt = false;

boolean gpsLastPosLogged = false;

#define gpsFixLed 5

void gpsSetup() {
  pinMode(gpsFixLed, OUTPUT);
  gps.begin(9600);
  gps.sendCommand(PMTK_SET_BAUD_9600);
  gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  gps.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);
  useInterrupt(true);
}

State gpsSMparse() {
  if (gps.newNMEAreceived()) {
    gpsLastPosLogged = false;
    if (!gps.parse(gps.lastNMEA()))
      return;
    if (gps.fix)
      digitalWrite(gpsFixLed, HIGH);
    else
      digitalWrite(gpsFixLed, LOW);

    /*
        if (GPSLOG) {
          Serial.print("\nTime: ");
          Serial.print(gps.hour, DEC); Serial.print(':');
          Serial.print(gps.minute, DEC); Serial.print(':');
          Serial.print(gps.seconds, DEC); Serial.print('.');
          Serial.println(gps.milliseconds);
          Serial.print("Date: ");
          Serial.print(gps.day, DEC); Serial.print('/');
          Serial.print(gps.month, DEC); Serial.print("/20");
          Serial.println(gps.year, DEC);
          Serial.print("Fix: "); Serial.print((int)gps.fix);
          Serial.print(" quality: "); Serial.println((int)gps.fixquality);
          if (gps.fix) {
            Serial.print("Location: ");
            Serial.print(gps.latitude, 4); Serial.print(gps.lat);
            Serial.print(", ");
            Serial.print(gps.longitude, 4); Serial.println(gps.lon);
            Serial.print("Location (in degrees, works with Google Maps): ");
            Serial.print(gps.latitudeDegrees, 4);
            Serial.print(", ");
            Serial.println(gps.longitudeDegrees, 4);

            Serial.print("Speed (knots): "); Serial.println(gps.speed);
            Serial.print("Angle: "); Serial.println(gps.angle);
            Serial.print("Altitude: "); Serial.println(gps.altitude);
            Serial.print("Satellites: "); Serial.println((int)gps.satellites);
          }
          Serial.println("--------------");
        } // */
  }
}


String gpsGetLog() {
  String str = "";
  if (!gpsLastPosLogged)
    str += "*";
  str += "\t";
  str += "20" + leadingZeros(gps.year) + "/" + leadingZeros(gps.month)  + "/" + leadingZeros(gps.day) + " ";
  str +=        leadingZeros(gps.hour) + ":" + leadingZeros(gps.minute) + ":" + leadingZeros(gps.seconds);
  str += "\t";
  if (gps.fix)
    str += String(gps.latitudeDegrees, 5);
  str += "\t";
  if (gps.fix)
    str += String(gps.longitudeDegrees, 5);
  str += "\t";
  if (gps.fix)
    str += String(gps.angle, 0);
  str += "\t";
  if (gps.fix)
    str += String(gps.speed * 1.852, 1); // knots to kph

  gpsLastPosLogged = true;
  return str;
}

String leadingZeros(int val) {
  String str = "";
  if (val < 10)
    str += "0";
  str += (String)val;
  return str;
}

boolean gpsNewData() {
  return gps.newNMEAreceived();
}

// Adafruit Stuff

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = gps.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;
  // writing direct to UDR0 is much much faster than Serial.print
  // but only one character can be written at a time.
#endif
}

void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}

