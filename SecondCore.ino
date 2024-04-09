void secondTask(void * parameter) {
  while (true) {
    
    if(totalPulse > 500 && restetScaleOnec){scale.tare(); restetScaleOnec = false;}
    //else if(!restetScaleOnec && timeReadLoadcell + 100 < nowTime){newForce = abs(10 * scale.get_units()); timeReadLoadcell = nowTime;}
     else if(!restetScaleOnec){newForce = abs(10 * scale.get_units()); timeReadLoadcell = nowTime;}
   
    vTaskDelay(10 / portTICK_PERIOD_MS); // Exempel på en liten fördröjning
  }
}
