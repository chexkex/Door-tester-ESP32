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
