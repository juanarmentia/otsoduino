
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include <MemoryFree.h>
#include <avr/wdt.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x87, 0xF3 };
IPAddress ip(192,168,59, 177);          // SmartLab
//IPAddress ip(192,168,1, 177);           // Home
char* myIP = "http://192.168.59.177/";  // SmartLab
//char* myIP = "http://192.168.1.177/";   // Home

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

File spacesFile;			      //File to save the relations between spaces' uris and folders

unsigned long startTime;                      //Momento en el que inicia el programa
unsigned long timesCounter = 0;               //Numero de veces que se ha ejecutado el Write para calcular cuando se tiene que ejecutar de nuevo
int interval = 30000;                         //Intervalo de tiempo para que se ejecute el write
boolean firstWrite = true;                    //Para controlar si es la primera tripleta de un grafo o son las siguientes las que se escriben
char URL_part2[30];			      //Segunda parte de la uri del grafo --> (1000.txt, 1001.txt ...)

boolean firstFile = true;	              //Boolean para controlar los accesos a los diferentes directorios en la funcion searchDirectory, de la funcion readGraph()
char directory[30];			      //Variable que se utiliza para almacenar ruta del archivo que se esta leyendo en la funcion searchPath, de la funcion readGraph()
char directorySpace[30];		      //Variable que se utiliza para almacenar la ruta del espacio que se esta leyendo en la funcion searchPath, de la funcion readGraph()

char c;                                       //Caracter en el que se almacena el caracter leido de fichero
int indexSpace = 0;		              //Se guarda el indice del espacio para guardar en spaces.txt (space1000, space1001...)
int indexGraph = 0;		              //Se guarda el indice de los grafos (1000, 1001...)

int objectType;                               //Tipo de objeto
                                              //     0: url
                                              //     1: int 
                                              //     2: unsigned int
                                              //     3: double
                                              //     4: boolean
                                              //     5: string

boolean selectedDirectory = false;
char selectedPath[64];
char uriGraph[64];
char graph[2000];			      //Grafo que se devuelve en los read  (Se queda corto de memoria)
//char graphBySpace[2000];		      //Grafo que se crea en el printDirectorySpace
char macIdentifier[15];			      //Variable to save the MAC to create de graph's uris
char subj[80];
char pred[80];
char obj[80];
char objType[2];

//CONSTANTS
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
char* spacesIndexConst  = "SPACES.TXT";
char* nothingConst     	= "nothing";
char* extConst         	= ".txt";
char* sepConst	     	= "|";
char* sepUrlConst	= "/";
char* pointConst	= ".";
char* nullConst        	= "\0";

int freeMem;

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




//void PrintFile(File myFile, char* myFileName, char* directory){
//	if (strcmp(directory, "\0")!= 0){
//		strcat(directory,myFileName);
//		strcpy(myFileName,directory);
//	}
//	// re-open the file for reading:
//	myFile = SD.open(myFileName);
//	if (myFile) {
//		Serial.println("**********************************");
//		Serial.println(myFileName);
//  
//		// read from the file until there's nothing else in it:
//		while (myFile.available()) {
//			Serial.write(myFile.read());
//		}
//		// close the file:
//		myFile.close();
//	} else {
//		// if the file didn't open, print an error:
//		Serial.print("Error opening ");
//		Serial.println(myFileName);
//	}
//	Serial.println("-------------------------------------");
//}




