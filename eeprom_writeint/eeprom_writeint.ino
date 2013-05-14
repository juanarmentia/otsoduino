/*
 * EEPROM Write
 *
 * Stores values read from analog input 0 into the EEPROM.
 * These values will stay in the EEPROM when the board is
 * turned off and may be retrieved later by another sketch.
 */

#include <EEPROM.h>

// the current address in the EEPROM (i.e. which byte
// we're going to write to next)
int addressGraph = 5;
int addressSpace = 0;
unsigned int indexGraph = 2000; 
unsigned int indexSpace = 10; 

void setup()
{
  EEPROM_writeint(addressGraph, indexGraph);
  delay(1000);
  EEPROM_writeint(addressSpace, indexSpace);
}

void loop()
{
}


//write long integer into EEPROM
void EEPROM_writelong(int address, unsigned long value) {
  //truncate upper part and write lower part into EEPROM
  EEPROM_writeint(address+2, word(value));
  //shift upper part down
  value = value >> 16;
  //truncate and write
  EEPROM_writeint(address, word(value));
}



//write word to EEPROM
void EEPROM_writeint(int address, int value) {
  EEPROM.write(address,highByte(value));
  EEPROM.write(address+1 ,lowByte(value));
}


