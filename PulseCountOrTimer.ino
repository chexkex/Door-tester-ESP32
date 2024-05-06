

void PulseCountOrTimerMe(){

if(doorCalibration){
  if(pulseTrueTimerFalse){
      tempTotalPulse = totalPulse / pulseCloseToEnd;
      if(relayOpenDoorTime < nowTime){digitalWrite(relayOpenDoor, HIGH);}
      if(totalPulse > 200 && tempTotalPulse > abs(totalDiffPulse)){digitalWrite(relayDoorSwitch, LOW);}
      else if(totalPulse > 30 && doorCalRelayOnce){digitalWrite(relayDoorSwitch, HIGH); doorCalRelayOnce == false;}
  }
  else{

        if(relayOpenDoorTime < nowTime){digitalWrite(relayOpenDoor, HIGH);}
        if(relayOpenDoorTime + 1000 < nowTime){digitalWrite(relayDoorSwitch, HIGH);}
    
      }
      
      
 }

if(pulseTrueTimerFalse){
  if(!doorCalibration && doorCalibrationOnce){
    
    if(totalPulseBehind + 5000 < nowTime){totalPulseBehind = nowTime; totalPulsebefore2 = totalPulsebefore1; totalPulsebefore1 = totalPulse;}

    if (totalPulse == totalPulsebefore2){
       doorCalibrationOnce = false;
       
      if(totalPulse > 9999 && totalPulse < 100000){
        
        PulseOver10000(totalPulse); 
        checkTotalPulseOverOnce = false;
        Serial.println(AddChecksum(160000 + totalPulse));
        delay(100);
        Serial.println(AddChecksum(210000 + totalPulseOver));
        }
        else{Serial.println(AddChecksum(160000 + totalPulse));}
        delay(100);
        Serial.println(AddChecksum(111116));
                   
          
      }
      
    }  
}

if(testStarted){
  
  if(pulseTrueTimerFalse){
  
     if(relayDoorSwitchtTime < nowTime && totalPulse > 30 && relayDoorSwitchOnce){digitalWrite(relayDoorSwitch, HIGH); relayDoorSwitchOnce=false;}
     
     if(timeFor10PulsesInt < maxPulse100ms && timeFor10PulsesInt < 9999 && timeFor10PulsesInt > 2){maxPulse100ms = timeFor10PulsesInt;}
     
     if(totalPulseBehind + 3000 < nowTime){totalPulseBehind = nowTime; totalPulsebefore2 = totalPulsebefore1; totalPulsebefore1 = totalPulse;}
      timeFor10PulsesInt = ((int)timeFor10Pulses / 10);
  
  }
  else{
    
      if(relayDoorSwitchtTime < nowTime && relayDoorSwitchOnce){digitalWrite(relayDoorSwitch, HIGH); relayDoorSwitchOnce=false;}
    
    }
  
  }    


if(testStarted && (waitTimeForStart < nowTime)){
        
    if(pulseTrueTimerFalse){
  
       tempTotalPulse = totalPulse / pulseCloseToEnd;

      if(tempTotalPulse > abs(totalDiffPulse)){closeToEndOnce = true; digitalWrite(relayDoorSwitch, LOW);}


      if ((totalPulse == totalPulsebefore2 && closeToEndOnce && newCurrent1 < 500) || testError){

      


            //Calling test is over          
            if(waitTimeWhenTestIsDone < nowTime){
              
                Serial.println(AddChecksum(111114));
                lastDataSentNoChecksum = 111114;
                waitTimeDataSend = nowTime;
                testStarted = false;
                sendForceOnce = true;
                sendCurrent1Once = true;
                sendCurrent2Once = true;
                sendCurrent3Once = true;
                sendTotalPulseOnce = true;
                sendTotalDiffPulseOnce = true;
                sendMaxSpeedOnce = true;
                sendEndOnce = true;
                waitTimeDataSend = nowTime;
                closeToEndOnce = false;
                sendTotalPulseOverOnce = true;
                checkTotalPulseOverOnce = true;

                testError = false;

                
              }
          
          }
          else{waitTimeWhenTestIsDone = nowTime + 2000;}
  
    }
    else{
          if(maxForce > 10){digitalWrite(relayDoorSwitch, LOW);}
          Serial.println(newCurrent1);
          
          if(maxForce > 10 &&  newCurrent1 < 100){
                if(waitTimeWhenTestIsDone < nowTime){
                Serial.println(AddChecksum(111114));
                lastDataSentNoChecksum = 111114;
                waitTimeDataSend = nowTime;
                testStarted = false;
                sendForceOnce = true;
                sendCurrent1Once = true;
                sendCurrent2Once = true;
                sendCurrent3Once = true;
                sendTotalPulseOnce = true;
                sendTotalDiffPulseOnce = true;
                sendMaxSpeedOnce = true;
                sendEndOnce = true;
                waitTimeDataSend = nowTime;
                closeToEndOnce = false;
                sendTotalPulseOverOnce = true;
                checkTotalPulseOverOnce = true;
                digitalWrite(relayDoorSwitch, LOW);}
                }

          else{waitTimeWhenTestIsDone = nowTime + 2000;}
    
        }   
        
       
    
    }  

}
