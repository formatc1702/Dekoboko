#include <Wire.h>
#include "SM.h"

SM accSM(accSMsample);
SM logSM(logSMwait);
SM ledSM(ledSMwait);
SM gpsSM(gpsSMparse);

void setup() {
  Serial.begin(115200);
  accSetup();
  magSetup();
  ledSetup();
  gpsSetup();
  rpmSetup();
  logStart();
}

void loop() {
  EXEC(accSM);
  EXEC(logSM);
  EXEC(ledSM);
  EXEC(gpsSM);
}
