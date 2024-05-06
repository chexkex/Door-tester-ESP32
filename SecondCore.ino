void secondTask(void * parameter) {
  while (true) {
    if(testStarted){

      if(totalPulse > 500 && restetScaleOnec && pulseTrueTimerFalse && loadCellReset < nowTime){scale.tare(); restetScaleOnec = false;}

      else if(restetScaleOnec && waitTimeForStart + 5000 < nowTime){scale.tare(); restetScaleOnec = false;}
      else if(!restetScaleOnec){newForce = abs(10 * scale.get_units()); /*timeReadLoadcell = nowTime;*/}
      
      newCurrent1 = ReadCurrentFrom(0, callFactorSernsor1);
      newCurrent2 = ReadCurrentFrom(1, callFactorSernsor2);
      newCurrent3 = ReadCurrentFrom(2, callFactorSernsor3);
    }
    else{vTaskDelay(10 / portTICK_PERIOD_MS);}
    vTaskDelay(2 / portTICK_PERIOD_MS);
    
  }
}
