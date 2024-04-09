//Not in use interrupts

/*
//Hallsensor interupt
void IRAM_ATTR HallSensor(){
    totalPulse++;
    pulsesBetweenTime++;
    if(pulsesBetweenTime == 10){
      timeFor10Pulses = nowTime - timeBetweenRead;
      timeBetweenRead = nowTime;
      pulsesBetweenTime = 0;
      
      }
    if(digitalRead(hallpin2) == LOW){totalDiffPulse++;}
    else{totalDiffPulse--;} 
  }*/
