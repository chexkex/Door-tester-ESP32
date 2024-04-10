//Loading lib
#include "HX711.h"
#include "ADS1X15.h"
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
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
bool sendEndOnce = false;
bool sendTotalPulseOverOnce = false;
bool checkTotalPulseOverOnce = false;


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
int totalPulseOver = 0;
int totalDiffPulse = 0;
int maxPulse100ms = 10000;
int newPulse100ms = 0;
int pulseCloseToEnd = 20; //When program shoud look for end of test higher number more accurate
int tempTotalPulse = 0;
int totalPulsebefore1 = 0;
int totalPulsebefore2 = 0;
int hallpin1 = 5; //Hallpin 1
int hallpin2 = 15; //Hallpin 2
int totalPulseBefore = 0;
int timeFor10PulsesInt = 0;


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
unsigned long timeReadLoadcell = 0;

//Test fos speed in interupt
/*unsigned long timeBetweenRead = 0;
int pulsesBetweenTime = 0;
unsigned long timeFor10Pulses = 0;*/

volatile unsigned long timeOfLastPulse = 0;
volatile unsigned long timeFor10Pulses = 0;
volatile int pulsesBetweenTime = 0;


//Calibration factor for current sensor 
int callFactorSernsor1 = 0;
int callFactorSernsor2 = 0;
int callFactorSernsor3 = 0;

//Print values realTime
bool printForceValues = false;
bool printCurrentValues = false;

void IRAM_ATTR HallSensor() {
  totalPulse++;
  pulsesBetweenTime++;
  
  unsigned long nowTimeMicros = micros();
  
  if (pulsesBetweenTime == 10) {
    if (timeOfLastPulse != 0) {
      timeFor10Pulses = nowTimeMicros - timeOfLastPulse;
      
    }
    timeOfLastPulse = nowTimeMicros;
    pulsesBetweenTime = 0;
  }
  
  if (digitalRead(hallpin2) == LOW) {
    totalDiffPulse++;
  } else {
    totalDiffPulse--;
  }
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

  //Starting its second core
   xTaskCreatePinnedToCore(
    secondTask,   // Funktionen för den andra uppgiften
    "SecondTask", // Namn på uppgiften
    10000,        // Stackstorlek för uppgiften (i byte)
    NULL,         // Inga parametrar för funktionen
    1,            // Prioritet för uppgiften (1 är normalt)
    NULL,         // Inget handtag till uppgiften
    0             // Vilken kärna att köra uppgiften på (0 eller 1)
  );
  
}

void loop() {
  
  nowTime = millis();
  if(Serial.available() > 0){ReadSerialData();}
  //If test is started sensors are messuring. Reading sensors and add to max if its bigger
  if(testStarted){
      
      if(relayOpenDoorTime < nowTime){digitalWrite(relayOpenDoor, HIGH);}
      if(relayDoorSwitchtTime < nowTime && totalPulse > 20 && relayDoorSwitchOnce){digitalWrite(relayDoorSwitch, HIGH); relayDoorSwitchOnce=false;}
      
      
      newCurrent1 = ReadCurrentFrom(0, callFactorSernsor1);
      newCurrent2 = ReadCurrentFrom(1, callFactorSernsor2);
      newCurrent3 = ReadCurrentFrom(2, callFactorSernsor3);
    
      if(newForce > maxForce && !restetScaleOnec){maxForce = newForce;}
      if(newCurrent1 > maxCurrent1){maxCurrent1 = newCurrent1;}
      if(newCurrent2 > maxCurrent2){maxCurrent2 = newCurrent2;}
      if(newCurrent3 > maxCurrent3){maxCurrent3 = newCurrent3;}
      if(timeFor10PulsesInt < maxPulse100ms && timeFor10PulsesInt < 9999 && timeFor10PulsesInt > 2){maxPulse100ms = timeFor10PulsesInt;}
      
      if(totalPulseBehind + 3000 < nowTime){totalPulseBehind = nowTime; totalPulsebefore2 = totalPulsebefore1; totalPulsebefore1 = totalPulse;}
      timeFor10PulsesInt = ((int)timeFor10Pulses / 10);
      
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
                sendEndOnce = true;
                waitTimeDataSend = nowTime;
                closeToEndOnce = false;
                sendTotalPulseOverOnce = true;
                checkTotalPulseOverOnce = true;
                
              }
          
          }
          else{waitTimeWhenTestIsDone = nowTime + 2000;}
        
    
    }

  //Send data
  if(!testStarted){
      if(totalPulse > 9999 && totalPulse < 100000){PulseOver10000(totalPulse); checkTotalPulseOverOnce = false;}
      digitalWrite(relayDoorSwitch, LOW);
      int maxForceInt = maxForce;
      if(waitTimeDataSend + 200 < nowTime && sendForceOnce){Serial.println(AddChecksum((120000 + maxForceInt))); lastDataSentNoChecksum = (120000 + maxForce); sendForceOnce = false;}
      if(waitTimeDataSend + 400 < nowTime && sendCurrent1Once){Serial.println(AddChecksum((130000 + maxCurrent1))); lastDataSentNoChecksum = (130000 + maxCurrent1); sendCurrent1Once = false;}
      if(waitTimeDataSend + 600 < nowTime && sendCurrent2Once){Serial.println(AddChecksum((140000 + maxCurrent2))); lastDataSentNoChecksum = (140000 + maxCurrent2); sendCurrent2Once = false;}
      if(waitTimeDataSend + 800 < nowTime && sendCurrent3Once){Serial.println(AddChecksum((150000 + maxCurrent3))); lastDataSentNoChecksum = (150000 + maxCurrent3); sendCurrent3Once = false;}
      if(waitTimeDataSend + 1000 < nowTime && sendTotalPulseOnce && totalPulse < 100000){Serial.println(AddChecksum((160000 + totalPulse))); lastDataSentNoChecksum = (160000 + totalPulse); sendTotalPulseOnce = false;}
      if(waitTimeDataSend + 1200 < nowTime && sendTotalDiffPulseOnce){Serial.println(AddChecksum((175000 + totalDiffPulse))); lastDataSentNoChecksum = (175000 + totalDiffPulse); sendTotalDiffPulseOnce = false;}
      if(waitTimeDataSend + 1400 < nowTime && sendMaxSpeedOnce){Serial.println(AddChecksum((180000 + maxPulse100ms))); lastDataSentNoChecksum = (180000 + maxPulse100ms); sendMaxSpeedOnce = false;}
      if(waitTimeDataSend + 1600 < nowTime && sendTotalPulseOverOnce && !checkTotalPulseOverOnce){Serial.println(AddChecksum((210000 + totalPulseOver))); lastDataSentNoChecksum = (210000 + totalPulseOver); sendTotalPulseOverOnce = false;}
      if(waitTimeDataSend + 1800 < nowTime && sendEndOnce){Serial.println(AddChecksum(111116)); lastDataSentNoChecksum = 111116; sendEndOnce = false;}
    
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
