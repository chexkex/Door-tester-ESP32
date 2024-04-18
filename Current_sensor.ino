//Callibration current sensors
void CalibrateCurrentSensor(){
  
    callFactorSernsor1 = ADS.readADC(0);
    callFactorSernsor2 = ADS.readADC(1);
    callFactorSernsor3 = ADS.readADC(2);
  
  }

//Reading currentsensors and converting to mA
int ReadCurrentFrom(int sensor, int calibrationFactor)
{
    ADS.setGain(0);
    int dataFromSensorX = ADS.readADC(sensor);
    int outData = (abs(dataFromSensorX - calibrationFactor)) / 0.9783;
    return outData;
}    
