
#include <avr/pgmspace.h>
#include <avr/wdt.h>

// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);
IPAddress broadcastServer(192,168,59, 177);
EthernetClient clientOtsopack;
char* ipBroadcast = "255.255.255.255";

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
char* spacesIndexConst  = "SPACES.TXT";
char* nothingConst     	= "nothing";
char* extConst         	= ".txt";
char* sepConst	     	= "|";
char* sepUrlConst	= "/";
char* pointConst	= ".";
char* nullConst        	= "\0";
char* anySubj           = "?s";
char* anyPred           = "?p";
char* anyObj            = "?o";

int freeMem;

