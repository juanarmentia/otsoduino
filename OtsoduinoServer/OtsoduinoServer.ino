
#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x79, 0xC1 };
IPAddress ip(192,168,59, 177);
char* myIP = "http://192.168.59.177/";

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

File spacesFile;

unsigned long startTime;                      //Momento en el que inicia el programa
unsigned long timesCounter = 0;               //Número de veces que se ha ejecutado el Write para cálcular cuando se tiene que ejecutar de nuevo
unsigned int interval = 30000;                 //Intervalo de tiempo para que se ejecute el write
boolean firstWrite = true;                    //Para controlar si es la primera tripleta de un grafo o son las siguientes las que se escriben
char URL_part2[30];							  //Segunda parte de la uri del grafo --> (1000.txt, 1001.txt ...)

boolean firstFile = true;					  //Boolean para controlar los accesos a los diferentes directorios en la función searchDirectory, de la función readGraph()
char directory[30];							  //Variable que se utiliza para almacenar ruta del archivo que se está leyendo en la función searchPath, de la función readGraph()
char directorySpace[30];							  //Variable que se utiliza para almacenar la ruta del espacio que se está leyendo en la función searchPath, de la función readGraph()

char c;                                       //Carácter en el que se almacena el carácter leído de fichero
unsigned int indexSpace = 0;				  //Se guarda el índice del espacio para guardar en spaces.txt (space1000, space1001...)
unsigned int indexGraph = 0;				  //Se guarda el índice de los grafos (1000, 1001...)


boolean selectedDirectory = false;
char selectedPath[64];
char uriGraph[64];
char graph[2500];

//CONSTANTS
char* uriSpaceConst1	= "http://otsopack1";
char* subjectConst1		= "subject1";
char* predicateConst1	= "predicate1";
char* objectConst1		= "object1";
char* uriSpaceConst2	= "http://otsopack2";
char* subjectConst2		= "subject2";
char* predicateConst2	= "predicate2";
char* objectConst2		= "object2";
char* spacesIndexConst	= "SPACES.TXT";
char* nothingConst		= "nothing";
char* extConst			= ".txt";
char* sepConst			= "|";
char* sepUrlConst		= "/";
char* pointConst		= ".";
char* nullConst			= "\0";


void setup()
{
  delay(2000);
  Serial.begin(115200);

  pinMode(4, OUTPUT);
  while (!SD.begin(4)) {
    Serial.println("initialization failed!");
  }

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  //readGraph(uriSpaceConst1,"1107");
  readGraph(uriSpaceConst1, subjectConst2, predicateConst2, objectConst2);
  startTime = millis();
  Serial.print("millis(): ");
  Serial.println(startTime);
}

void loop()
{
	/*if (millis()/1000>(startTime/1000+(interval*timesCounter))){
		timesCounter++;
		Serial.println("hola");
		//Serial.println(readGraph(uriSpaceConst1, subjectConst2, predicateConst2, objectConst2));

		//Serial.println(writeTriple(uriSpaceConst1, subjectConst1, predicateConst1, objectConst1, firstWrite));
		//Serial.println();
		//delay(1000);

		//Serial.println(writeTriple(uriSpaceConst1, subjectConst2, predicateConst2, objectConst2, firstWrite));
		//Serial.println();
		//firstWrite = true;
		//delay(1000);
	 //   
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
  
	//httpServer();
  
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

