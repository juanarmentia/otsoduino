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
unsigned int interval = 30;                   //Intervalo de tiempo para que se ejecute el write
boolean firstWrite = true;                    //Para controlar si es la primera tripleta de un grafo o son las siguientes las que se escriben
int iURL_part2 = 0;                   //Segunda parte de la uri del grafo --> Random(100-999)
char URL_part2[30];

char c;                                       //Carácter en el que se almacena el carácter leído de fichero
unsigned int indexSpace = EEPROM_readint(0);  //Se guarda el índice del espacio para guardar en spaces.txt (space1000, space1001...)

//char cUriGraph[18]; // to Store the name of each file "XXX.txt"

void setup()
{
  delay(5000);
  Serial.begin(9600);
  Serial.print("EEPROM: ");
  Serial.println(indexSpace);
  Serial.print("Initializing SD card...");
  // On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  // Note that even if it's not used as the CS pin, the hardware SS pin
  // (10 on most Arduino boards, 53 on the Mega) must be left as an output
  // or the SD library functions will not work.
  pinMode(10, OUTPUT);
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  
  
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
    Serial.println(timesCounter);
    
    writeTriple("http://otsopack3", "subject1", "predicate1", "object1");
    //Serial.println();
    delay(300);
    writeTriple("http://otsopack3", "subject2", "predicate2", "object2");
    Serial.println();
    delay(300);
    firstWrite = true;
    delay(1000);
//    
//    writeTriple("http://otsopack2", "subject1", "predicate1", "object1");
//    Serial.println();
//    Serial.println();
//    writeTriple("http://otsopack2", "subject3", "predicate3", "object3", firstWrite);
//    Serial.println();  
//    firstWrite = true;
//    myFile = SD.open("/");
//    printDirectory(myFile, 0);
//    delay(1000);
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



char* readSpaceUri(File gFile){
  char* rSpaceUri = "";
  c = gFile.read();
  if (c!=-1){
    while (c != '|'){
       rSpaceUri += c;
       c = gFile.read();
    }
  }
  else{
    rSpaceUri = "nothing"; 
  }
  return rSpaceUri;
}

char* readSpaceFolder(File gFile){
  char* rSpaceFolder = "";
  c = gFile.read();
  while (c != '\n'){
    rSpaceFolder += c;
    c = gFile.read();
  }
  return rSpaceFolder;
}

char* writeTriple(char* space, char* subject, char* predicate, char* object){
  
  File graphFile;
  Serial.print("1. Dentro de writeTriple: ");
  Serial.println(indexSpace);
  char* spaceUri = "";      //URI del espacio
  char* spaceFolder = "";   //Nombre de la carpeta correspondiente al espacio
  
  
  //Se empiela a leer spaces.txt para ver si existe el espacio
  graphFile = SD.open("spaces.txt"); //No utilizar nombres largos de fichero (formato 8.3)
  spaceUri = readSpaceUri(graphFile);
  //Serial.println("1.5. spaceUri: " + spaceUri);
  
  //Si la URI leída en el fichero no es la misma que hemos pasado a la función o el fichero no está vacío, se sigue leyendo.
  while((spaceUri != space)&&(spaceUri != "nothing")){
    while (c != '\n'){
      c = graphFile.read();
    }
    spaceUri = readSpaceUri(graphFile);
  }
  
  //Si ha salido del while anterior y el fichero no está vacío, es que el espacio que se ha pasado a la función existe en el fichero y leemos el nombre de su carpeta asociada.
  if (spaceUri != "nothing"){
    //Leer la parte del nombre de la carpeta
    spaceFolder = readSpaceFolder(graphFile);
    Serial.print("2. spaceFolder: ");
    Serial.println(spaceFolder);
  }
  graphFile.close();
  
  Serial.print("3. spaceUri(Nothing): |");
  Serial.print(spaceUri);
  Serial.println("|");
  //Si no se ha encontrado el espacio en el fichero, se introduce en el spaces.txt y se crea una carpeta
  if (spaceUri == "nothing"){ 
    Serial.println("4. spaceUri es 'nothing'");
    spacesFile = SD.open("spaces.txt", FILE_WRITE);
    if (spacesFile) { 
      spacesFile.print(space);
      spacesFile.print("|SP");
      spacesFile.println(indexSpace); 
      spacesFile.close();
    }
    //Se crea la carpeta
    
    spaceFolder = "SP"; 
    sprintf(spaceFolder, "%d", indexSpace);
    Serial.print("5. spaceFolder: ");
    Serial.println(spaceFolder);
    Serial.println(indexSpace);
    Serial.print("|");
    Serial.print(spaceFolder);
    Serial.println("|");
    SD.mkdir(spaceFolder);
    indexSpace++;
    EEPROM_writeint(0, indexSpace);
  }
  
  //Guardar en la SD en el directorio SD:\space_uri\uri.txt las tripletas "s;p;o"
  
  //Serial.println(firstWrite);
  if (firstWrite){
    //Se crea la URI para el nuevo conjunto de tripletas y se comprueba si existe
    iURL_part2 = random(100,999);
    sprintf(URL_part2, "%d", iURL_part2);
  }
    
  char uriGraph[18];
  uriGraph[0]=0;
  strcat(uriGraph, spaceFolder);
  strcat(uriGraph, "/");
  strcat(uriGraph, URL_part2);
  strcat(uriGraph, ".txt\0");
  Serial.print("uriGraph: |");
  Serial.print(uriGraph);
  Serial.println("|");
  
  
  while((SD.exists(uriGraph))&&(firstWrite)){
    iURL_part2 = random(100,999);
    sprintf(URL_part2, "%d", iURL_part2);
    uriGraph[0]=0;
    strcat(uriGraph, spaceFolder);
    strcat(uriGraph, "/");
    strcat(uriGraph, URL_part2);
    strcat(uriGraph, ".txt\0");
    Serial.print("uriGraph: |");
    Serial.print(uriGraph);
    Serial.print("|");
  }
  
  Serial.println();
  firstWrite = false;
  
  
  // open a new file and immediately close it:
  Serial.print("7. Creating |");
  Serial.print(uriGraph);
  Serial.println("|");
  graphFile = SD.open(uriGraph, FILE_WRITE); //No utilizar nombres largos de fichero
  Serial.println("Opened");
  if (graphFile){
    //Serial.println(subject+predicate+object);
    graphFile.print(subject);
    graphFile.print('|');
    graphFile.print(predicate);
    graphFile.print('|');
    graphFile.println(object);
    graphFile.close();
  }
  Serial.print("8. uri: |");
  Serial.print(space);
  Serial.print("/");
  Serial.print(URL_part2);
  Serial.println("|");
  
  char* uri = "http://otsopack/";
  return strcat(uri,URL_part2);
}

//String readGraph(String space, String subject, String predicate, String object){
//  
//  //Hay que recorrer todos los ficheros y carpetas en busca del patrón s,p,o. Cuando se encuentre, se devolverá un String
//  //con todas las líneas del fichero en el que se encuentra el patrón concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
//  
//  //Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteración se comprueba si es directorio con isDirectory().
//  //Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patrón. 
//  //Cuando se encuentre el patrón, se lee de nuevo ese fichero que tendrá la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
//  
//  // re-open the file for reading:
//  myFile = SD.open("test.txt");
//  if (myFile) {
//    Serial.println("test.txt:");
//   
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//        Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//  
//  return "";
//}
//
//
//void printDirectory(File dir, int numTabs) {
//   while(true) {
//
//     File entry =  dir.openNextFile();
//     if (! entry) {
//       // no more files
//       Serial.println("**nomorefiles**");
//       break;
//     }
//     for (uint8_t i=0; i<numTabs; i++) {
//       Serial.print('\t');
//     }
//     Serial.print(entry.name());
//     if (entry.isDirectory()) {
//       Serial.println("/");
//       printDirectory(entry, numTabs+1);
//     } else {
//       // files have sizes, directories do not
//       Serial.print("\t\t");
//       Serial.println(entry.size(), DEC);
//     }
//   }
//}
