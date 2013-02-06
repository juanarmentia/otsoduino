/*
 * EEPROM Read
 *
 * Reads the value of each byte of the EEPROM and prints it 
 * to the computer.
 * This example code is in the public domain.
 */

#include <EEPROM.h>

// start reading from the first byte (address 0) of the EEPROM
int address = 5;
unsigned int value;

void setup()
{
  Serial.begin(9600);
}

void loop()
{
  // read a byte from the current address of the EEPROM
  value = EEPROM_readint(address);
  delay(1000);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value);
  Serial.println();
  
  // advance to the next address of the EEPROM
  //address = address + 1;
  
  // there are only 512 bytes of EEPROM, from 0 to 511, so if we're
  // on address 512, wrap around to address 0
 // if (address == 512)
 //   address = 0;
    
  //delay(500);
}


// read double word from EEPROM, give starting address
unsigned long EEPROM_readlong(int address) {
  //use word read function for reading upper part
  unsigned long dword = EEPROM_readint(address);
  //shift read word up
  dword = dword << 16;
  // read lower word from EEPROM and OR it into double word
  dword = dword | EEPROM_readint(address+2);
  return dword;
}


unsigned int EEPROM_readint(int address) {
  unsigned int word = word(EEPROM.read(address), EEPROM.read(address+1));
  return word;
} 
