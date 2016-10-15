#define logRateMs 200

State logSMlog() {
  accCompute();
  accComputeSmooth();
  Serial.print(gpsGetLog());
  Serial.print('\t');
  Serial.print(accGetMax());
  Serial.print('\t');
  Serial.print(accGetSmoothMaxAvg());
  Serial.print('\t');
  Serial.print(accGetSmoothMaxMax());
  Serial.print('\t');
  Serial.print(magGetHeading());
  Serial.print('\t');
  Serial.print(rpmGetLog());
  
  Serial.println();

  logSM.Set(logSMwait);
}

State logSMwait() {
  if(logSM.Timeout(logRateMs)) {
     logSM.Set(logSMlog);
  }
}

void logStart() {
  Serial.print("\t");
  Serial.print("DateTime\t");
  Serial.print("GpsLat\t");
  Serial.print("GpsLong\t");
  Serial.print("GpsSpeed\t");
  Serial.print("GpsHead\t");
  Serial.print("a_cur\t");
  Serial.print("a_avg\t");
  Serial.print("a_max\t");
  Serial.print("MagHead\t");
  Serial.print("rpmDist\t");
  Serial.print("rpmSpeed");
  Serial.println();
}
