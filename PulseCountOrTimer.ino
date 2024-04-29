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

      if (totalPulse == totalPulsebefore2 && closeToEndOnce && newCurrent1 < 500){

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
                
              }
          
          }
          else{waitTimeWhenTestIsDone = nowTime + 2000;}
  
    }
    else{
          if(maxForce > 10){digitalWrite(relayDoorSwitch, LOW);}
          
          if(maxForce > 10 && waitTimeWhenTestIsDone < nowTime && newCurrent1 < 500){
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
                digitalWrite(relayDoorSwitch, LOW);
                }

          else{waitTimeWhenTestIsDone = nowTime + 2000;}
    
        }   
        
       
    
    }  
