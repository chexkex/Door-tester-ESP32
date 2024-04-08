//Loading lib
#include "HX711.h"
#include "ADS1X15.h"
#include <EEPROM.h>

ADS1115 ADS(0x48);

//Com variables
String lastDataSentNoChecksum;

String lastDataResived;
int lastDataResivedIntNoChecksum;
int lastChecksumResived;
int lastDataResivedChecksum;

//Program variables
bool startTestOk = false;
bool testStarted = false;
bool sendForceOnce = false;
bool sendCurrent1Once = false;
bool sendCurrent2Once = false;
bool sendCurrent3Once = false;
bool restetScaleOnec = false;


//Force variables and loadcell
int maxForce = 0;
int newForce;
bool loadcellCall = false;
bool loadcellCallNoLoad = false;
bool loadcellCallKnownLoad = false;
float loadcellCallFactor = 0;
float loadcellCallFactorNum = 0;
int loadcellCallFactorAdress = 10; //Place where callfactor i located on the EEPROM
int loadcellCallFactorResultAdress = 100; //Place where callfactor i located on the EEPROM
const int LOADCELL_DOUT_PIN = 23;
const int LOADCELL_SCK_PIN = 4;
HX711 scale;

//Current variables
int maxCurrent1 = 0;
int newCurrent1;
int maxCurrent2 = 0;
int newCurrent2;
int maxCurrent3 = 0;
int newCurrent3;

//Speed variables
int totalPulse = 0;
int totalDiffPulse = 0;
int maxPulse100ms = 0;
int newPulse100ms = 0;
int pulseCloseToEnd = 20; //When program shoud look for end of test higher number more accurate
int tempTotalPulse = 0;
int totalPulsebefore1 = 0;
int totalPulsebefore2 = 0;
int hallpin1 = 5; //Hallpin 1
int hallpin2 = 15; //Hallpin 2
int totalPulseBefore = 0;


//Relay outputs
int relayOpenDoor = 26;
int relayDoorSwitch = 25;

//Timer variables
unsigned long nowTime = 0;
unsigned long relayOpenDoorTime = 0;
unsigned long relayDoorSwitchtTime = 0;
unsigned long waitTimeForStart = 0;
unsigned long waitTimeWhenTestIsDone = 0;
unsigned long waitTimeDataSend = 0;
unsigned long printCon = 0;
unsigned long totalPulseBehind = 0;
unsigned long timer100ms = 0;

//Calibration factor for current sensor 
int callFactorSernsor1 = 0;
int callFactorSernsor2 = 0;
int callFactorSernsor3 = 0;

//Print values realTime
bool printForceValues = false;
bool printCurrentValues = false;

//Hallsensor interupt
void IRAM_ATTR HallSensor(){
    totalPulse++;
    if(digitalRead(hallpin2) == LOW){totalDiffPulse++;}
    else{totalDiffPulse--;} 
  }


void setup() {
  Serial.begin(9600);
  pinMode(relayOpenDoor, OUTPUT);
  pinMode(relayDoorSwitch , OUTPUT);
  pinMode(hallpin1, INPUT);
  pinMode(hallpin2, INPUT);
  digitalWrite(relayOpenDoor, HIGH);
  digitalWrite(relayDoorSwitch, LOW);
  Wire.begin();
  ADS.begin();
  EEPROM.begin(512);
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);

  attachInterrupt(digitalPinToInterrupt(hallpin1), HallSensor, FALLING);
  
}

void loop() {

  
  nowTime = millis();
  //reading Serialport
  if(Serial.available() > 0){
        lastDataResived = Serial.readStringUntil('\n');
        lastDataResivedChecksum = ChecksumCalculator(lastDataResived);
        lastDataResivedIntNoChecksum = lastDataResived.substring(0 , 6).toInt();
        lastChecksumResived = lastDataResived.substring(6).toInt();

        if(lastChecksumResived != lastDataResivedChecksum){Serial.println(AddChecksum(111121));}
         
        else if(lastDataResivedIntNoChecksum == 111122){Serial.println(AddChecksum(lastDataSentNoChecksum.toInt()));}
        else if(loadcellCall){
          if(lastDataResivedIntNoChecksum == 411113){loadcellCallNoLoad = true;}
          else if(lastDataResivedIntNoChecksum == 411114){loadcellCallKnownLoad = true;}
          else{Serial.println(AddChecksum(411117)); lastDataSentNoChecksum = 411117;}
        }
        else if(lastDataResivedIntNoChecksum == 111111){
              
              if(startTestOk){
                Serial.println(AddChecksum(111112)); 
                lastDataSentNoChecksum = 111112; 
                testStarted = true;
                maxForce = 0;
                maxCurrent1 = 0;
                maxCurrent2 = 0;
                maxCurrent3 = 0;
                totalPulse = 0;
                totalDiffPulse = 0;
                maxPulse100ms = 0;
                CalibrateCurrentSensor();
                relayOpenDoorTime = nowTime + 500;
                relayDoorSwitchtTime = nowTime + 300;
                waitTimeForStart = nowTime + 5000;
                timer100ms = nowTime;
                digitalWrite(relayOpenDoor, LOW);
                restetScaleOnec = true;

                loadcellCallFactorNum = readFloatFromEEPROM(loadcellCallFactorResultAdress) / readFloatFromEEPROM(loadcellCallFactorAdress);

                scale.set_scale(loadcellCallFactorNum);
                                
                }
              else{Serial.println(AddChecksum(111113)); lastDataSentNoChecksum = 111113;}
          
          }

        else if(lastDataResivedIntNoChecksum == 411111){loadcellCall = true;}
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
      
    }

  //If test is started sensors are messuring. Reading sensors and add to max if its bigger
  if(testStarted){
    
      if(relayOpenDoorTime < nowTime){digitalWrite(relayOpenDoor, HIGH);}
      if(relayDoorSwitchtTime < nowTime && totalPulse > 20){digitalWrite(relayDoorSwitch, HIGH);}
      if(totalPulse > 500 && restetScaleOnec){scale.tare(); restetScaleOnec = false;}
      newCurrent1 = ReadCurrentFrom(0, callFactorSernsor1);
      newCurrent2 = ReadCurrentFrom(1, callFactorSernsor2);
      newCurrent3 = ReadCurrentFrom(2, callFactorSernsor3);
    
      if(newForce > maxForce){maxForce = newForce;}
      if(newCurrent1 > maxCurrent1){maxCurrent1 = newCurrent1;}
      if(newCurrent2 > maxCurrent2){maxCurrent2 = newCurrent2;}
      if(newCurrent3 > maxCurrent3){maxCurrent3 = newCurrent3;}
      if(newPulse100ms > maxPulse100ms){maxPulse100ms = newPulse100ms;}


      //Se över HUR DETTA FUNGEAR MED STOPPA TEST SE KONSTIGT UT!!!!!!!
      if(totalPulseBehind + 3000 < nowTime){totalPulseBehind = nowTime; totalPulsebefore2 = totalPulsebefore1; totalPulsebefore1 = totalPulse;}

      if(timer100ms + 100 < nowTime){

        newPulse100ms = totalPulse - totalPulseBefore;
        totalPulseBefore = totalPulse;
        timer100ms = nowTime;
        
        }

  }

  //Stops test
  if(testStarted && (waitTimeForStart < nowTime)){
    
        tempTotalPulse = totalPulse / pulseCloseToEnd;
        if(tempTotalPulse > totalDiffPulse){
          digitalWrite(relayDoorSwitch, LOW);
        if (totalPulse > (totalPulsebefore2 - 3) && totalPulse < (totalPulsebefore2 + 3)){

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
                waitTimeDataSend = nowTime;
                
              }
          
          }
          else{waitTimeWhenTestIsDone = nowTime + 2000;}
        }
    
    }

  //Send data
  if(!testStarted){

      if(waitTimeDataSend + 200 < nowTime && sendForceOnce){Serial.println(AddChecksum(maxForce)); lastDataSentNoChecksum = maxForce; sendForceOnce = false;}
      if(waitTimeDataSend + 400 < nowTime && sendCurrent1Once){Serial.println(AddChecksum(maxCurrent1)); lastDataSentNoChecksum = maxCurrent1; sendCurrent1Once = false;}
      if(waitTimeDataSend + 600 < nowTime && sendCurrent2Once){Serial.println(AddChecksum(maxCurrent2)); lastDataSentNoChecksum = maxCurrent2; sendCurrent2Once = false;}
      if(waitTimeDataSend + 800 < nowTime && sendCurrent3Once){Serial.println(AddChecksum(maxCurrent3)); lastDataSentNoChecksum = maxCurrent3; sendCurrent3Once = false;}
      //if(waitTimeDataSend + 1000 < nowTime){Serial.println(AddChecksum(maxForce)); lastDataSentNoChecksum = maxForce;}
    
    }  
  
  if(printForceValues || printCurrentValues){
    
    if(printCon < nowTime){
        if(printForceValues){Serial.println(scale.get_units(), 1);}
        if(printCurrentValues){Serial.println(ReadCurrentFrom(0, callFactorSernsor1));}
        printCon = nowTime + 500; 
      }
    }  

  if(loadcellCall){
    
    if (scale.is_ready()) {
        if(loadcellCallNoLoad){
          delay(1000);
          scale.set_scale();
          delay(1000);
          scale.tare();
          Serial.println(AddChecksum(411115));
          lastDataSentNoChecksum = 411115;
          loadcellCallNoLoad = false;
          }
          else if(loadcellCallKnownLoad){

            delay(1000);
            long reading = scale.get_units(10);
            writeFloatToEEPROM(loadcellCallFactorResultAdress, reading);
            Serial.println(AddChecksum(411116));
            lastDataSentNoChecksum = 411116;
            loadcellCallKnownLoad = false;
            loadcellCall = false;
            }  
     } 
    
    delay(1000);
    
    }  

}

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
    int outData = (abs(dataFromSensorX - calibrationFactor)) / 2.55555;
    return outData;
}  

//input data from string and calculates its checksum 
int ChecksumCalculator (String dataIn){
  
    int receivedNumber = dataIn.substring(0, 6).toInt();
    
    int calculatedChecksum = 0;
    int tempNumber = receivedNumber;

    while(tempNumber > 0){

      calculatedChecksum += tempNumber % 10;
      tempNumber /= 10;
     
      }

    calculatedChecksum %= 10;

    return calculatedChecksum;
      
  }

//Adding checksum to the string
String AddChecksum(int dataToSend) {
    String dataString = String(dataToSend);
    int checksum = 0;
    for (int i = 0; i < dataString.length(); i++) {
        checksum += dataString.charAt(i) - '0'; 
    }
    checksum %= 10; 

    dataString += String(checksum);

    return dataString; 
}  

//Write to EEPROM
void writeFloatToEEPROM(int address, float value) {
  char floatStr[20]; 
  dtostrf(value, 8, 2, floatStr); 
  for (int i = 0; i < strlen(floatStr); i++) {EEPROM.write(address + i, floatStr[i]);}
  EEPROM.write(address + strlen(floatStr), '\0'); // Nollterminera strängen
  EEPROM.commit();
}

//Read form EEPROM
float readFloatFromEEPROM(int address) {
  char floatStr[20]; 
  int i = 0;
  char currentChar = EEPROM.read(address + i);
  while (currentChar != '\0' && i < 20) {
    floatStr[i] = currentChar;
    i++;
    currentChar = EEPROM.read(address + i);
  }
  floatStr[i] = '\0'; 
  return atof(floatStr);
}
