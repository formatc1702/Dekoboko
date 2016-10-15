#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define accSampleCount 10
#define accSmoothCount 10

int accSamples[accSampleCount];
int accCurSample = 0;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(1);

int accX, accY, accZ;

#define accZero 10

#define accSampleRateMs 10

State accSMsample() {
  accSampleNow();
  accSM.Set(accSMwait);
}

State accSMwait() {
  if(accSM.Timeout(accSampleRateMs)) {
     accSM.Set(accSMsample);
  }
}

int accMax, accMin, accAvg;

int accSmoothMaxs[accSmoothCount];
int accSmoothAvgs[accSmoothCount];
int accCurSmoothSample = 0;
int accSmoothMaxMax, accSmoothMaxAvg, accSmoothAvg;

boolean accSetup() {
  // todo: return true/false
  accel.begin();
  accel.setRange(ADXL345_RANGE_16_G);
  accel.setDataRate(ADXL345_DATARATE_100_HZ);
  //  displaySensorDetails();
  //  displayDataRate();
  //  displayRange();

  return true;
}

void accRead() {
  sensors_event_t event;
  accel.getEvent(&event);
  accX = event.acceleration.x;
  accY = event.acceleration.y;
  accZ = event.acceleration.z;
}

void accSampleNow() {
  // get value
  accRead();
  int accNew = accZ;
  // add to sampler
  accSamples[accCurSample] = accNew;
  if (++accCurSample == accSampleCount) {
    accCurSample = 0;
    accCompute();
    accSmoothMaxs[accCurSmoothSample] = accMax;
    accSmoothAvgs[accCurSmoothSample] = accAvg;
    if (++accCurSmoothSample == accSmoothCount) {
      accCurSmoothSample = 0;
    }
  }
}

void accCompute() {
  accMax = accZero;
//  accMin = accZero;
  accAvg = 0;

  for (int i = 0; i < accSampleCount; i++) {
    int val = accSamples[i];
    accAvg += val;
    if (val > accMax)
      accMax = val;
//    if (val < accMin)
//      accMin = val;
  }
  accAvg /= accSampleCount;
}

void accComputeSmooth() {
  accSmoothMaxMax = accZero;
  accSmoothMaxAvg = 0;
    
  for (int i = 0; i < accSmoothCount; i++) {
    int val = accSmoothMaxs[i];
    accSmoothMaxAvg += val;
    if (val > accSmoothMaxMax)
      accSmoothMaxMax = val;
  }
  accSmoothMaxAvg /= accSmoothCount;
}

int accGetX() {
  return accX;
}

int accGetY() {
  return accY;
}

int accGetZ() {
  return accZ;
}

int accGetLatest() {
  return accSamples[accCurSample];
}

int accGetAvg() {
  return accAvg;
}

int accGetMax() {
  return accMax;
}

//int accGetMin() {
//  return accMin;
//}

int accGetSmoothMaxMax() {
  return accSmoothMaxMax;
}

int accGetSmoothMaxAvg() {
  return accSmoothMaxAvg;
}
