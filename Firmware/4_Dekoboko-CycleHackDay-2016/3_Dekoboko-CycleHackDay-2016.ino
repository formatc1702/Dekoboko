#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

#define BT Serial1

#define SAMPLE_RATE   2 // ms
#define SAMPLE_TIME  20 // ms
#define SAMPLE_AMOUNT SAMPLE_TIME / SAMPLE_RATE // N
//#define MOV_AVG_TIME 1000 // ms
//#define MOV_AVG_AMOUNT MOV_AVG_TIME / SAMPLE_RATE // N
#define BUCKET_AMOUNT 5
#define BUCKET_TIME SAMPLE_TIME / BUCKET_AMOUNT

long samples[SAMPLE_AMOUNT];
int current_sample = 0;
long raw_max = 0;
long raw_min = 0;
//long mov_avg[MOV_AVG_AMOUNT];
long avg = 0;
long mov_max = 0;
//int mov_avg_pos = 0;
long buckets[BUCKET_AMOUNT];
int current_bucket = 0;
long counter = 0;

Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

void setup() {
  Serial.begin(9600);
  BT.begin(9600);
  BT.println("#\tmin\tmax\tavg");

  if (!accel.begin())
  {
    Serial.println("Ooops, no ADXL345 detected ... Check your wiring!");
    while (1);
  }
  accel.setRange(ADXL345_RANGE_16_G);

//  for (int i = 0; i < MOV_AVG_AMOUNT; i++) {
//    mov_avg[i] = 0;
//  }
  for (int i = 0; i < SAMPLE_AMOUNT; i++) {
    samples[i] = 0;
  }
}

void loop() {
  // Read accelerometer data
  sensors_event_t event;
  accel.getEvent(&event);
  // Convert to int, remove gravity, take absolute value
  float raw_f = event.acceleration.z;
  long  raw_i = (long)(raw_f * 100.0);
  raw_i -= 981;
  raw_i = abs(raw_i);
  // Record one smaple
  samples[current_sample] = raw_i;
  // Enough samples to fill the bucket!
  if (++current_sample >= SAMPLE_AMOUNT) {
    // bucket will store the max of all samples
    long sample_max = 0;
    for (int i = 0; i < SAMPLE_AMOUNT; i++) {
      if (samples[i] > sample_max)
        sample_max = samples[i];
    }
    buckets[current_bucket] = sample_max;
    // Enough buckets to start over! But first, send data via Bluetooth
    if (++current_bucket >= BUCKET_AMOUNT) {
      current_bucket = 0;
      // Data to send: min, max, avg of gathered buckets
      long bucket_max = 0;
      long bucket_min = 999999;
      long bucket_avg = 0;
      for (int i = 0; i < BUCKET_AMOUNT; i++) {
        if (buckets[i] > bucket_max)
          bucket_max = buckets[i];
        if (buckets[i] < bucket_min)
          bucket_min = buckets[i];
        bucket_avg += buckets[i];
      }
      bucket_avg /= BUCKET_AMOUNT;
      // Send out data
      BT.print(counter);
      BT.print('\t');
      BT.print(bucket_min);
      BT.print('\t');
      BT.print(bucket_max);
      BT.print('\t');
      BT.print(bucket_avg);
      BT.println();
      counter++;
    }
    current_sample = 0;
  }
  //  Serial.print("Current sample: ");
  //  Serial.print(current_sample);
  //  Serial.print("    Current bucket: ");
  //  Serial.println(current_bucket);

  delay(SAMPLE_RATE);
}





































//for (int i = 0; i < MOV_AVG_AMOUNT; i++) {
//  sensors_event_t event;
//  accel.getEvent(&event);
//
//  float raw_f = event.acceleration.z;
//  long  raw_i = (long)(raw_f * 100.0);
//  raw_i -= 981;
//  raw_i = abs(raw_i);
//
//  Serial.print(raw_i);
//  Serial.println();















//    samples[i] = raw_i;
//    if (raw_i > raw_max)
//      raw_max = raw_i;
//    if (raw_i < raw_min)
//      raw_min = raw_i;

//  mov_avg[mov_avg_pos++] = raw_i;
//  if (mov_avg_pos >= MOV_AVG_AMOUNT)
//    mov_avg_pos = 0;
//
//  avg     = 0;
//  mov_max = 0;
//  for (int i = 0; i < MOV_AVG_AMOUNT; i++) {
//    avg += mov_avg[i];
//    if (mov_avg[i] > mov_max)
//      mov_max = mov_avg[i];
//  }
//  avg /= MOV_AVG_AMOUNT;
//
//  delay(SAMPLE_RATE);
//}

//  Serial.print(event.acceleration.z);
//  Serial.println();



//BT.print(avg);
//BT.print("\t");
//BT.print(mov_max);
//BT.print("\t");
//BT.print(((float)mov_max / (float)avg));
//BT.println();
//delay(10);
//}

void bargraph(int val) {
  int bars = val;
  bars += 40;
  int i = 0;
  for (i = 0; i < bars; i++) {
    Serial.print("|");
  }
  for (i = bars; i < 80; i++) {
    Serial.print(" ");
  }
  Serial.println();
}

