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
