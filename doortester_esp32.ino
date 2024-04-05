String lastDataSentNoChecksum;

String lastDataResived;
int lastDataResivedIntNoChecksum;
int lastChecksumResived;
int lastDataResivedChecksum;

bool startTestOk = false;
bool testStarted = false;

int maxForce;
int newForce;

int maxCurrent1;
int newCurrent1;
int maxCurrent2;
int newCurrent2;
int maxCurrent3;
int newCurrent3;

int totalPulse;
int totalDiffPulse;
int maxPulse100ms;
int newPulse100ms;




void setup() {
  Serial.begin(9600);
}

void loop() {

  //reading Serialport
  if(Serial.available() > 0){
    
        lastDataResived = Serial.readStringUntil('\n');
        lastDataResivedChecksum = ChecksumCalculator(lastDataResived);
        lastDataResivedIntNoChecksum = lastDataResived.substring(0 , 6).toInt();
        lastChecksumResived = lastDataResived.substring(6).toInt();

        if(lastChecksumResived != lastDataResivedChecksum){Serial.println(AddChecksum(111121));}
         
        else if(lastDataResivedIntNoChecksum == 111122){Serial.println(AddChecksum(lastDataSentNoChecksum.toInt()));}

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
                }
              else{Serial.println(AddChecksum(111113)); lastDataSentNoChecksum = 111113;}
          
          }    
    }

  //If test is started sensors are messuring. 
  if(testStarted){
    
     
    
      if(newForce > maxForce){maxForce = newForce;}
      if(newCurrent1 > maxCurrent1){maxCurrent1 = newCurrent1;}
      if(newCurrent2 > maxCurrent2){maxCurrent2 = newCurrent2;}
      if(newCurrent3 > maxCurrent3){maxCurrent3 = newCurrent3;}
      if(newPulse100ms > maxPulse100ms){maxPulse100ms = newPulse100ms;}

  }

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
