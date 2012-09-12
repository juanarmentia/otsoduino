/** 
  write word to EEPROM 
**/
void EEPROM_writeint(int address, int value) {
  EEPROM.write(address,highByte(value));
  EEPROM.write(address+1 ,lowByte(value));
}

/**
  read double word from EEPROM, give starting address
**/
unsigned long EEPROM_readlong(int address) {
  //use word read function for reading upper part
  unsigned long dword = EEPROM_readint(address);
  //shift read word up
  dword = dword << 16;
  // read lower word from EEPROM and OR it into double word
  dword = dword | EEPROM_readint(address+2);
  return dword;
}

/**
  write long integer into EEPROM
**/
void EEPROM_writelong(int address, unsigned long value) {
  //truncate upper part and write lower part into EEPROM
  EEPROM_writeint(address+2, word(value));
  //shift upper part down
  value = value >> 16;
  //truncate and write
  EEPROM_writeint(address, word(value));
}

/**
  write an integer into EEPROM
**/
unsigned int EEPROM_readint(int address) {
  unsigned int word = word(EEPROM.read(address), EEPROM.read(address+1));
  return word;
} 

/**
  print a char * stored in Flash Memory (instead of SRAM) to the Serial output
**/
void prinFromMemorySerial(char *str){
  
  char *mem = str;
  
    while (pgm_read_byte(mem) != 0x00) /* Comparamos con \0, un terminador */
      Serial.print(pgm_read_byte(mem++));
  Serial.print('|');
}

/**
  print a char * stored in Flash Memory (instead of SRAM) to a File passed by parameter
**/
void prinFromMemoryToFile(char *str, File file)
{
  char *mem=str;

  while (pgm_read_byte(mem) != 0x00) /* Comparamos con \0, un terminador */
    file.print(pgm_read_byte(mem++));
  file.print('|');
}

/**
  print a char * stored in Flash Memory (instead of SRAM) to a server where the client is connected
**/
//void prinFromMemoryToWeb(char *str)
//{
//  char *mem=str;
//
//  while (pgm_read_byte(mem) != 0x00) /* Comparamos con \0, un terminador */
//    client.print(pgm_read_byte(mem++));
//}


/**
  print a char * stored in Flash Memory (instead of SRAM) to a server where the client is connected
**/
//void concatenateToTweet(char *str)
//{
//  char *mem=str;
//
//  while (pgm_read_byte(mem) != 0x00) /* Comparamos con \0, un terminador */
//    msgTwitter.concat(pgm_read_byte(mem++));
//}
