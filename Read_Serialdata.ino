  //reading Serialport
  void ReadSerialData(){
        lastDataResived = Serial.readStringUntil('\n');
        lastDataResivedChecksum = ChecksumCalculator(lastDataResived);
        lastDataResivedIntNoChecksum = lastDataResived.substring(0 , 6).toInt();
        lastChecksumResived = lastDataResived.substring(6).toInt();

        if(lastChecksumResived != lastDataResivedChecksum){Serial.println(AddChecksum(111121));}
         
        else if(lastDataResivedIntNoChecksum == 111122){Serial.println(AddChecksum(lastDataSentNoChecksum.toInt()));}
        else if(loadcellCall){
          if(lastDataResivedIntNoChecksum == 411111){loadcellCall = false;}
          else if(lastDataResivedIntNoChecksum == 411113){loadcellCallNoLoad = true;}
          else if(lastDataResivedIntNoChecksum == 411114){loadcellCallKnownLoad = true;}
          else{Serial.println(AddChecksum(411117)); lastDataSentNoChecksum = 411117;}
        }
        else if(lastDataResivedIntNoChecksum == 411131 && !doorCalibration){doorCalibration = true; 
          totalPulse = 0; 
          totalDiffPulse = 0; 
          doorCalRelayOnce = true;
          relayOpenDoorTime = nowTime + 500;
        digitalWrite(relayOpenDoor, LOW);}
        else if(lastDataResivedIntNoChecksum == 411132 && doorCalibration){doorCalibration = false; doorCalibrationOnce = true; checkTotalPulseOverOnce = true;}
        else if(lastDataResivedIntNoChecksum == 111111 && !printCurrentValues && !printForceValues){
              
              if(startTestOk && !testStarted){
                Serial.println(AddChecksum(111112)); 
                lastDataSentNoChecksum = 111112; 
                
                maxForce = 0;
                maxCurrent1 = 0;
                maxCurrent2 = 0;
                maxCurrent3 = 0;
                totalPulse = 0;
                totalDiffPulse = 0;
                totalPulseBefore = 0;
                maxPulse100ms = 10000;
                CalibrateCurrentSensor();
                relayOpenDoorTime = nowTime + 500;
                relayDoorSwitchtTime = nowTime + 300;
                waitTimeForStart = nowTime + 5000;
                             
                restetScaleOnec = true;
                relayDoorSwitchOnce = true;
               
                pulsesBetweenTime = 0;

                loadcellCallFactorNum = readFloatFromEEPROM(loadcellCallFactorResultAdress) / readFloatFromEEPROM(loadcellCallFactorAdress);

                scale.set_scale(loadcellCallFactorNum);
                delay(100);
                testStarted = true;
                digitalWrite(relayOpenDoor, LOW);
                                
                }
              else{Serial.println(AddChecksum(111113)); lastDataSentNoChecksum = 111113;}
          
          }

        else if(lastDataResivedIntNoChecksum == 411111){loadcellCall = !loadcellCall;}
        else if(lastDataResivedIntNoChecksum > 420000 && lastDataResivedIntNoChecksum < 430000){

          int tempcall = lastDataResivedIntNoChecksum % 10000;
          loadcellCallFactor = float(tempcall) / 10.0;
          writeFloatToEEPROM(loadcellCallFactorAdress, loadcellCallFactor);
          
          }  

        else if(lastDataResivedIntNoChecksum == 411112){
          loadcellCallFactorNum = readFloatFromEEPROM(loadcellCallFactorResultAdress) / readFloatFromEEPROM(loadcellCallFactorAdress);
          scale.set_scale(loadcellCallFactorNum);
          scale.tare();
          if(printForceValues){printForceValues = false;}
          else if(!printForceValues){printForceValues = true;} 
        }  

        else if(lastDataResivedIntNoChecksum == 411121){
          if(printCurrentValues){printCurrentValues = false;}
          else if(!printCurrentValues){CalibrateCurrentSensor(); printCurrentValues = true;}
        }
        else if(lastDataResivedIntNoChecksum == 111117){
          testStarted = false;
          sendForceOnce = true;
                sendCurrent1Once = true;
                sendCurrent2Once = true;
                sendCurrent3Once = true;
                sendTotalPulseOnce = true;
                sendTotalDiffPulseOnce = true;
                sendMaxSpeedOnce = true;
                waitTimeDataSend = nowTime;
                closeToEndOnce = false;
                sendEndOnce = true;
                sendTotalPulseOverOnce = true;
                checkTotalPulseOverOnce = true;
          }
      
    }
