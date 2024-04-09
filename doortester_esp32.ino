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
bool startTestOk = true;
bool testStarted = false;
bool sendForceOnce = false;
bool sendCurrent1Once = false;
bool sendCurrent2Once = false;
bool sendCurrent3Once = false;
bool restetScaleOnec = false;
bool relayDoorSwitchOnce = false;
bool closeToEndOnce = false;
bool sendTotalPulseOnce = false;
bool sendTotalDiffPulseOnce = false;
bool sendMaxSpeedOnce = false;


//Force variables and loadcell
float maxForce = 0;
float newForce = 0;
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
unsigned long maxPulse100ms = 0;
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
unsigned long times100msControle = 0;

//Test fos speed in interupt
unsigned long timeBetweenRead = 0;
int pulsesBetweenTime = 0;
unsigned long timeFor10Pulses = 0;


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
    pulsesBetweenTime++;
    if(pulsesBetweenTime == 10){
      timeFor10Pulses = nowTime - timeBetweenRead;
      timeBetweenRead = nowTime;
      pulsesBetweenTime = 0;
      
      }
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
  startTestOk = true;
  
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
              
              if(startTestOk && !testStarted){
                Serial.println(AddChecksum(111112)); 
                lastDataSentNoChecksum = 111112; 
                testStarted = true;
                maxForce = 0;
                maxCurrent1 = 0;
                maxCurrent2 = 0;
                maxCurrent3 = 0;
                totalPulse = 0;
                totalDiffPulse = 0;
                totalPulseBefore = 0;
                maxPulse100ms = 0;
                CalibrateCurrentSensor();
                relayOpenDoorTime = nowTime + 500;
                relayDoorSwitchtTime = nowTime + 300;
                waitTimeForStart = nowTime + 5000;
                timer100ms = nowTime;
                digitalWrite(relayOpenDoor, LOW);
                restetScaleOnec = true;
                relayDoorSwitchOnce = true;
                timer100ms = nowTime;
                times100msControle = nowTime;
                timeBetweenRead = nowTime;
                pulsesBetweenTime = 0;

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
      if(relayDoorSwitchtTime < nowTime && totalPulse > 20 && relayDoorSwitchOnce){digitalWrite(relayDoorSwitch, HIGH); relayDoorSwitchOnce=false;}
      if(totalPulse > 500 && restetScaleOnec){scale.tare(); restetScaleOnec = false;}
      else if(!restetScaleOnec){newForce = abs(10 * scale.get_units());}
      
      newCurrent1 = ReadCurrentFrom(0, callFactorSernsor1);
      newCurrent2 = ReadCurrentFrom(1, callFactorSernsor2);
      newCurrent3 = ReadCurrentFrom(2, callFactorSernsor3);
    
      if(newForce > maxForce){maxForce = newForce;}
      if(newCurrent1 > maxCurrent1){maxCurrent1 = newCurrent1;}
      if(newCurrent2 > maxCurrent2){maxCurrent2 = newCurrent2;}
      if(newCurrent3 > maxCurrent3){maxCurrent3 = newCurrent3;}
      if(timeFor10Pulses < maxPulse100ms){maxPulse100ms = newPulse100ms;}

      if(totalPulseBehind + 3000 < nowTime){totalPulseBehind = nowTime; totalPulsebefore2 = totalPulsebefore1; totalPulsebefore1 = totalPulse;}
      maxPulse100ms = (int)timeFor10Pulses;
      /*if(timer100ms + 100 < nowTime){

        unsigned long tempTime = nowTime - times100msControle;
        newPulse100ms = (totalPulse - totalPulseBefore);
        float tempPulses = newPulse100ms / tempTime;
        newPulse100ms = tempPulses * 100;
        Serial.println(totalPulse);
        Serial.println(totalPulseBefore);
        Serial.println(newPulse100ms);
        Serial.println("");
        
        totalPulseBefore = totalPulse;
        timer100ms = nowTime;
        times100msControle = nowTime;
        }*/

  }

  //Stops test
  if(testStarted && (waitTimeForStart < nowTime)){
        
        tempTotalPulse = totalPulse / pulseCloseToEnd;
        if(tempTotalPulse > abs(totalDiffPulse)){closeToEndOnce = true; digitalWrite(relayDoorSwitch, LOW);}
          
        if (totalPulse > (totalPulsebefore2 - 1) && totalPulse < (totalPulsebefore2 + 1) && closeToEndOnce){

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
                waitTimeDataSend = nowTime;
                closeToEndOnce = false;
                
              }
          
          }
          else{waitTimeWhenTestIsDone = nowTime + 2000;}
        
    
    }

  //Send data
  if(!testStarted){
      int maxForceInt = maxForce;
      if(waitTimeDataSend + 200 < nowTime && sendForceOnce){Serial.println(AddChecksum((120000 + maxForceInt))); lastDataSentNoChecksum = (120000 + maxForce); sendForceOnce = false;}
      if(waitTimeDataSend + 400 < nowTime && sendCurrent1Once){Serial.println(AddChecksum((130000 + maxCurrent1))); lastDataSentNoChecksum = (130000 + maxCurrent1); sendCurrent1Once = false;}
      if(waitTimeDataSend + 600 < nowTime && sendCurrent2Once){Serial.println(AddChecksum((140000 + maxCurrent2))); lastDataSentNoChecksum = (140000 + maxCurrent2); sendCurrent2Once = false;}
      if(waitTimeDataSend + 800 < nowTime && sendCurrent3Once){Serial.println(AddChecksum((150000 + maxCurrent3))); lastDataSentNoChecksum = (150000 + maxCurrent3); sendCurrent3Once = false;}
      if(waitTimeDataSend + 1000 < nowTime && sendTotalPulseOnce){Serial.println(AddChecksum((160000 + totalPulse))); lastDataSentNoChecksum = (160000 + totalPulse); sendTotalPulseOnce = false;}
      if(waitTimeDataSend + 1200 < nowTime && sendTotalDiffPulseOnce){Serial.println(AddChecksum((175000 + totalDiffPulse))); lastDataSentNoChecksum = (175000 + totalDiffPulse); sendTotalDiffPulseOnce = false;}
      if(waitTimeDataSend + 1400 < nowTime && sendMaxSpeedOnce){Serial.println(AddChecksum((180000 + maxPulse100ms))); lastDataSentNoChecksum = (180000 + maxPulse100ms); sendMaxSpeedOnce = false;}
    
    }  
  
  if(printForceValues || printCurrentValues){
    
    if(printCon < nowTime){
        float temptemp = abs(10 * scale.get_units(5));
        int convertForce = 120000 + ((int)temptemp);
        if(printForceValues){Serial.println(AddChecksum(convertForce));}
        if(printCurrentValues){Serial.println(AddChecksum((130000 + ReadCurrentFrom(0, callFactorSernsor1))));Serial.println(AddChecksum((140000 + ReadCurrentFrom(1, callFactorSernsor2))));
        Serial.println(AddChecksum((150000 + ReadCurrentFrom(2, callFactorSernsor3))));}
        printCon = nowTime + 2500; 
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
            delay(1000);
            Serial.println(AddChecksum(411116));
            lastDataSentNoChecksum = 411116;
            loadcellCallKnownLoad = false;
            loadcellCall = false;
            }  
     }    
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
