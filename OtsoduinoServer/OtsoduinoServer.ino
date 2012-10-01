/*
  Web Server
 
 A simple web server that shows the value of the analog input pins.
 using an Arduino Wiznet Ethernet shield. 
 
 Circuit:
 * Ethernet shield attached to pins 10, 11, 12, 13
 * Analog inputs attached to pins A0 through A5 (optional)
 
 created 18 Dec 2009
 by David A. Mellis
 modified 4 Sep 2010
 by Tom Igoe
 
 */

#include <SPI.h>
#include <Ethernet.h>
#include <SD.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
//byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x79, 0xC1 };
//IPAddress ip(192,168,59, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
//EthernetServer server(80);

File spacesFile;

unsigned long startTime;                      //Momento en el que inicia el programa
unsigned long timesCounter = 0;               //Número de veces que se ha ejecutado el Write para cálcular cuando se tiene que ejecutar de nuevo
unsigned int interval = 3000;                   //Intervalo de tiempo para que se ejecute el write
boolean firstWrite = true;                    //Para controlar si es la primera tripleta de un grafo o son las siguientes las que se escriben
unsigned int iURL_part2 = 0;                   //Segunda parte de la uri del grafo --> Random(100-999)
char URL_part2[30];

boolean firstFile = true;
char directory[30];

char c;                                       //Carácter en el que se almacena el carácter leído de fichero
unsigned int indexSpace = EEPROM_readint(0);  //Se guarda el índice del espacio para guardar en spaces.txt (space1000, space1001...)


boolean selectedDirectory = false;
char selectedPath[64];
char uriGraph[64];

//CONSTANTS
char* uriSpaceConst		= "http://otsopack3";
char* subjectConst1		= "subject1";
char* predicateConst1	= "predicate1";
char* objectConst1		= "object1";
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
  //Serial.print("EEPROM: ");
  //Serial.println(indexSpace);
  //Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(53, OUTPUT);
  while (!SD.begin(53)) {
    Serial.println("initialization failed!");
    //return;
  }
  //Serial.println("initialization done.");
  
  
  // start the Ethernet connection and the server:
//  Ethernet.begin(mac, ip);
//  server.begin();
  //Serial.println(Ethernet.localIP());
  
  startTime = millis();
}

void loop()
{
	if (millis()/1000>(startTime/1000+(interval*timesCounter))){
	timesCounter++;
	//    Serial.println(startTime);
	//    Serial.println(millis());
	Serial.println();
    
	//Serial.println(readGraph(uriSpaceConst, subjectConst2, predicateConst2, objectConst2));

	
	Serial.println(writeTriple(uriSpaceConst, subjectConst1, predicateConst1, objectConst1));
	Serial.println();
	delay(1000);
	
	Serial.println(writeTriple(uriSpaceConst, subjectConst2, predicateConst2, objectConst2));
	delay(300);
	firstWrite = true;
	delay(1000);
	    
	//writeTriple("http://otsopack2", "subject1", "predicate1", "object1");
	//Serial.println();
	//Serial.println();
	//writeTriple("http://otsopack2", "subject3", "predicate3", "object3", firstWrite);
	//Serial.println();  
	//firstWrite = true;
	//delay(1000);
	}
  
	//httpServer();
  
  
}


//void httpServer(){
//  // listen for incoming clients
//  EthernetClient client = server.available();
//  if (client) {
//    // an http request ends with a blank line
//    boolean currentLineIsBlank = true;
//    while (client.connected()) {
//      if (client.available()) {
//        char c = client.read();
//        // if you've gotten to the end of the line (received a newline
//        // character) and the line is blank, the http request has ended,
//        // so you can send a reply
//        if (c == '\n' && currentLineIsBlank) {
//          
//          //Aquí hay que enviar lo que recibamos en el readGraph(). Tratar el string y enviarlo
//          
//          // send a standard http response header
//          client.println("HTTP/1.1 200 OK");
//          client.println("Content-Type: text/html");
//          client.println();
//
//          // output the value of each analog input pin
//          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
//            client.print("analog input ");
//            client.print(analogChannel);
//            client.print(" is ");
//            client.print(analogRead(analogChannel));
//            client.println("<br />");
//          }
//          break;
//        }
//        if (c == '\n') {
//          // you're starting a new line
//          currentLineIsBlank = true;
//        } 
//        else if (c != '\r') {
//          // you've gotten a character on the current line
//          currentLineIsBlank = false;
//        }
//      }
//    }
//    // give the web browser time to receive the data
//    delay(1);
//    // close the connection:
//    client.stop();
//  }
//}



char* writeTriple(char* space, char* subject, char* predicate, char* object){
  
	File graphFile;
	Serial.print("1. Dentro de writeTriple: ");
	Serial.println(indexSpace);
	char spaceUri[64];    //URI del espacio
	memset(spaceUri, '\0', 64);      
	char spaceFolder[30];    //Nombre de la carpeta correspondiente al espacio
	memset(spaceFolder, '\0', 30);
  
  
	//Se empiela a leer spaces.txt para ver si existe el espacio
	graphFile = SD.open(spacesIndexConst); //No utilizar nombres largos de fichero (formato 8.3)
	strcpy(spaceUri, readSpaceUri(graphFile));
	Serial.print("1.5. spaceUri: ");
	Serial.print(spaceUri);
	Serial.print("|");
	Serial.println(space);
  
	//Si la URI leída en el fichero no es la misma que hemos pasado a la función o el fichero no está vacío, se sigue leyendo.
	while((strcmp(spaceUri, space)!=0)&&(strcmp(spaceUri, nothingConst)!=0)){
	while (c != '\n'){
		c = graphFile.read();
	}
	memset(spaceUri, '\0', 64);
	strcpy(spaceUri, readSpaceUri(graphFile));
	Serial.print("1.75. spaceUri: ");
	Serial.println(spaceUri);
	}
  
	//Si ha salido del while anterior y el fichero no está vacío, es que el espacio que se ha pasado a la función existe en el fichero y leemos el nombre de su carpeta asociada.
	if (strcmp(spaceUri, nothingConst)!=0){
	//Leer la parte del nombre de la carpeta
	memset(spaceFolder, '\0', 30);
	strcpy(spaceFolder, readSpaceFolder(graphFile));
	Serial.print("2. spaceFolder: ");
	Serial.println(spaceFolder);
	}
	graphFile.close();
  
	Serial.print("3. spaceUri(Nothing): ");
	Serial.println(spaceUri);
	//Si no se ha encontrado el espacio en el fichero, se introduce en el spaces.txt y se crea una carpeta
	if (strcmp(spaceUri, nothingConst)==0){ 
	Serial.println("4. spaceUri es 'nothing'");
	spacesFile = SD.open(spacesIndexConst, FILE_WRITE);
	if (spacesFile) { 
		spacesFile.print(space);
		spacesFile.print(sepConst);
		spacesFile.println(indexSpace); 
		spacesFile.close();
	}
	//Se crea la carpeta
    
	memset(spaceFolder, '\0', 30);
	Serial.print(spaceFolder);
	itoa(indexSpace, spaceFolder, 10);
	Serial.print("5. spaceFolder: ");
	Serial.println(spaceFolder);
	SD.mkdir(spaceFolder);
	indexSpace++;
	EEPROM_writeint(0, indexSpace);
	}
  
	//Guardar en la SD en el directorio SD:\space_uri\uri.txt las tripletas "s;p;o"
  
	//Serial.println(firstWrite);
	if (firstWrite){
	//Se crea la URI para el nuevo conjunto de tripletas y se comprueba si existe
	memset(URL_part2, '\0', 30);
	iURL_part2 = random(100,999);
	itoa(iURL_part2, URL_part2, 10);
	}
    
	char uriGraph[64];
	memset(uriGraph, '\0', 64);
	strcat(uriGraph, spaceFolder);
	strcat(uriGraph, sepUrlConst);
	strcat(uriGraph, URL_part2);
	strcat(uriGraph, extConst);
	Serial.print("1uriGraph: ");
	Serial.println(uriGraph);
  
	//Serial.print(SD.exists(uriGraph));
	//Serial.println("II");
	//Serial.print(firstWrite);
	//Serial.println("II");
  
	while((SD.exists(uriGraph))&&(firstWrite)){
	Serial.println("dentro del while");
	memset(URL_part2, '\0', 30);
	iURL_part2 = random(100,999);
	itoa(iURL_part2, URL_part2, 10);
	Serial.println(URL_part2);
	memset(uriGraph, '\0', 64);
	strcat(uriGraph, spaceFolder);
	strcat(uriGraph, sepUrlConst);
	strcat(uriGraph, URL_part2);
	strcat(uriGraph, extConst);
	Serial.print("2uriGraph: ");
	Serial.print(uriGraph);
	}

	//firstWrite = false;
	delay(3000);
  
	// open a new file and immediately close it:
	Serial.print("7. Creating ");
	delay(3000);
	char uriGraphcpy[64];
	memset(uriGraphcpy, '\0', 64);
	char * pch;
	pch = strtok (uriGraph,"\r");
	strcat(uriGraphcpy, pch);
	while (pch != NULL)
	{
		pch = strtok (NULL, "\n");
		strcat(uriGraphcpy, pch);
	}
	Serial.println(uriGraphcpy);
	graphFile = SD.open(uriGraphcpy, FILE_WRITE); //No utilizar nombres largos de fichero
	if (graphFile){
	Serial.println(subject);
	graphFile.print(subject);
	graphFile.print('|');
	graphFile.print(predicate);
	graphFile.print('|');
	graphFile.print(object);
	graphFile.println('|');
	graphFile.close();
	}

	char uriGraphReturn[64];
	memset(uriGraphReturn, '\0', 64);
	strcat(uriGraphReturn, space);
	strcat(uriGraphReturn, sepUrlConst);
	strcat(uriGraphReturn, strtok(uriGraphcpy, ".")); 
	  
	return uriGraphReturn;
}

char* readGraph(char* space, char* subject, char* predicate, char* object){
  
	//Hay que recorrer todos los ficheros y carpetas en busca del patrón s,p,o. Cuando se encuentre, se devolverá un String
	//con todas las líneas del fichero en el que se encuentra el patrón concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteración se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patrón. 
	//Cuando se encuentre el patrón, se lee de nuevo ese fichero que tendrá la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);

	File fileSpaces = SD.open("SPACES.TXT");
	while(strcmp(readSpaceUri(fileSpaces),space)!=0){
		readSpaceFolder(fileSpaces);
	}
	//Serial.print("Folder: ");
	//Serial.println(readSpaceFolder(fileSpaces));
	char folder[30];
	memset(folder, '\0', 30);
	strcpy(folder, readSpaceFolder(fileSpaces));
	fileSpaces.close();
	
	File root = SD.open("/");

	searchPath(root, 0, true, folder, subject, predicate, object, space);
	Serial.println(selectedPath);
	//Serial.println(uriGraph);

	root.close();
	
	return uriGraph;
}


void searchPath(File dir, int numTabs, boolean firstFile, char* folder, char* subject, char*predicate, char* object, char* space) {
    
	while(true) {

		if(firstFile){
			memset(directory, '\0', 30);
		}
		File entry =  dir.openNextFile();
		if (! entry) {
			//Serial.println("**nomorefiles**");
			break;
		}
		if (entry.isDirectory()) {
			if(atoi(entry.name()) == atoi(folder)){
				strcpy(directory, entry.name());
				strcat(directory, "/");
				selectedDirectory = true;
			}
			else
			{
				selectedDirectory = false;
			}
			firstFile = false;
			searchPath(entry, numTabs+1, firstFile, folder, subject, predicate, object, space);
		} else if(selectedDirectory) {
				File selectedFile = SD.open(strcat(directory,entry.name()));
				if (strcmp(readSpaceUri(selectedFile),subject) == 0)
					if(strcmp(readSpaceUri(selectedFile),predicate) == 0)
						if(strcmp(readSpaceUri(selectedFile),object) == 0){
							strcpy(selectedPath, directory);
							strcat(uriGraph, space);
							strcat(uriGraph, "/");
							strcat(uriGraph, strtok(directory, "."));
							exit;
						}
		}
	}
}

void PrintFile(File myFile, char* myFileName, char* directory){

	if (strcmp(directory, "\0")!= 0){
		strcat(directory,myFileName);
		strcpy(myFileName,directory);

	}
	// re-open the file for reading:
	myFile = SD.open(myFileName);
	if (myFile) {
		Serial.println("**********************************");
		Serial.println(myFileName);
  
		// read from the file until there's nothing else in it:
		while (myFile.available()) {
			Serial.write(myFile.read());
		}
		// close the file:
		myFile.close();
	} else {
		// if the file didn't open, print an error:
		Serial.print("Error opening ");
		Serial.println(myFileName);
	}
	Serial.println("-------------------------------------");
}

//Read the side where space`s URI is written
char* readSpaceUri(File gFile){
  char rSpaceUri[64];
  memset(rSpaceUri, '\0', 64);
  int index = 0;
  char c = gFile.read();
  if (c!=-1){
    while ((c != '|')){     
      rSpaceUri[index++] = c;
      c = gFile.read();      
    }
    delay(200);
  }
  else{
    return "nothing"; 
  }
  return rSpaceUri;
}

//Read the side where the folder, relationated with the space's URI, is written
char* readSpaceFolder(File gFile){
  char rSpaceFolder[64];
  memset(rSpaceFolder, '\0', 64);
  int index = 0;
  char c = gFile.read();
  while ((c != '\n')&&(c != ' ')){
    
    rSpaceFolder[index++] = c;
    delay(200);
    c = gFile.read();
  }
  return rSpaceFolder;
}