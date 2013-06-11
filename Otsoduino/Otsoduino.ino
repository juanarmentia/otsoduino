#include <Ethernet.h>
#include <SD.h>
#include <SPI.h>
#include <MemoryFree.h>
#include "variables.h"
#include "conf.h"

//CONSTANTS TO SIMULATE TRIPLES
char* uriSpaceConst1   	= "http://otsopack1";
char* subjectConst1    	= "subject1";
char* predicateConst1  	= "predicate1";
char* objectConst1     	= "object1";
char* uriSpaceConst2   	= "http://otsopack2";
char* subjectConst2    	= "subject2";
char* predicateConst2  	= "predicate2";
char* objectConst2     	= "object2";
char* uriSpaceConst3   	= "http://otsopack3";
char* subjectConst3    	= "subject3";
char* predicateConst3  	= "predicate3";
char* objectConst3     	= "object3";

void setup()
{
  //wdt_disable(); 
  //delay(2000);
  Serial.begin(115200);
  pinMode(53, OUTPUT);
  while (!SD.begin(4)) {
    Serial.println("initialization failed!");
  }

  getMac(mac);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  
  freeMem = freeMemory();
  Serial.print("freeMemory()=");
  Serial.println(freeMem);
  
  startTime = millis();
  delay(1000);
//  Serial.println(writeTriple(uriSpaceConst1, subjectConst1, predicateConst1, objectConst1, firstWrite));
//  Serial.println();
//  delay(1000);
//  Serial.println(writeTriple(uriSpaceConst1, subjectConst2, predicateConst3, objectConst3, firstWrite));
//  Serial.println();
//  firstWrite = true;
//  delay(1000);
  
//  Serial.println("Nueva tripleta");    
//  Serial.println(writeTriple(uriSpaceConst3, subjectConst2, predicateConst2, objectConst2, 1, firstWrite));
//  Serial.println();
//  firstWrite = true;
//  delay(1000);
//  Serial.print("millis(): ");
//  Serial.println(startTime);
  
  //wdt_enable(WDTO_4S);
}

void loop()
{
        //wdt_reset();	
        freeMem = freeMemory();
        if(freeMem != freeMemory()){        
          Serial.print("freeMemory()=");
        }
	/*if (millis()/1000>(startTime/1000+(interval*timesCounter))){
		timesCounter++;
		Serial.println("hola");

		//Serial.println(writeTriple(uriSpaceConst1, subjectConst1, predicateConst1, objectConst1, firstWrite));
		//Serial.println();
		//delay(1000);

		//Serial.println(writeTriple(uriSpaceConst1, subjectConst2, predicateConst2, objectConst2, firstWrite));
		//Serial.println();
		//firstWrite = true;
		//delay(1000);

		//Serial.println(writeTriple(uriSpaceConst1, subjectConst1, predicateConst1, objectConst1, firstWrite));
		//Serial.println();
		//firstWrite = true;
		//delay(1000);
		//
		//Serial.println(writeTriple(uriSpaceConst2, subjectConst2, predicateConst2, objectConst2, firstWrite));
		//Serial.println();  
		//firstWrite = true;
		//delay(1000);
	}*/
  
	httpServer();
  
}


