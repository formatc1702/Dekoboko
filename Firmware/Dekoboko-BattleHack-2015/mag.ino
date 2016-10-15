#include <Adafruit_HMC5883_U.h>

#define magDeclination 0.06

Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(2);

void magSetup() {
  mag.begin();
}

int magGetHeading() {
  sensors_event_t event;
  mag.getEvent(&event);
  float heading = atan2(event.magnetic.y, event.magnetic.x);
  heading += magDeclination;
  if (heading < 0)
    heading += 2 * PI;
  if (heading > 2 * PI)
    heading -= 2 * PI;
  return heading * 180/M_PI;
}

// */
