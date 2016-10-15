#include <Grove_LED_Bar.h>

#define ledRateMs 10

#define ledMaxG 30
#define ledMinG 10

#define bumpLed 6
#define bumpThresh 1.5

Grove_LED_Bar bar(9, 8, 1);

int ledstate = 0;
int ledsmoothstate = 0;
int ledsmoothcounter = 0;

float bumpOld = 0;

void ledSetup() {
  bar.begin();
  pinMode(bumpLed,OUTPUT);
}

void ledSetBits(int bits) {
  bar.setBits(bits);
}

void ledSetLevel(int level) {
  bar.setLevel(level);
}

void ledSetLed(int led, int level) {
  bar.setLed (led, level);
}

State ledSMupdate() {
  accSampleNow();
  accCompute();
  accComputeSmooth();
  int ledmax = constrain(map(accGetMax(), ledMinG, ledMaxG, 0, 10), 0, 10);
  if (ledmax > ledstate)
    ledstate = ledmax;
  else if (ledstate > 0)
    ledstate--;

  int ledsmoothmax = constrain(map(accGetSmoothMaxMax(), ledMinG, ledMaxG, 0, 10), 0, 10);
  if (ledsmoothmax >= ledsmoothstate) {
    ledsmoothstate = ledsmoothmax;
    ledsmoothcounter = 0;
  }
  else if (++ledsmoothcounter == 10) {
    ledsmoothstate--;
    ledsmoothcounter = 0;
  }
  
  float bumpNew = (float)accGetSmoothMaxMax() / (float)accGetSmoothMaxAvg();
  if (bumpNew - bumpOld > bumpThresh) {
    digitalWrite(bumpLed, HIGH);
  } else {
    digitalWrite(bumpLed, LOW);
  }
  bumpOld = bumpNew;
  
  

  ledSetLevel(ledstate);
  ledSetLed(ledsmoothstate, 1);

  ledSM.Set(ledSMwait);
}

State ledSMwait() {
  if (ledSM.Timeout(ledRateMs)) {
    ledSM.Set(ledSMupdate);
  }
}
