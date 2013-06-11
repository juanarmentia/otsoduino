//***********************************************************************************************
// FUNCION   WRITE(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI del grafo guardado 
//***********************************************************************************************
char* writeTriple(char* space, char* subject, char* predicate, char* object,int objType, boolean firstTriple){
  
	File graphFile;
	//Serial.print("1. Dentro de writeTriple: ");
	//Serial.println(indexSpace);
	char spaceUri[64];    //URI del espacio
	memset(spaceUri, '\0', 64);      
	char spaceFolder[30];    //Nombre de la carpeta correspondiente al espacio
	memset(spaceFolder, '\0', 30);
  
  
	//Se empiela a leer spaces.txt para ver si existe el espacio
	graphFile = SD.open(spacesIndexConst); //No utilizar nombres largos de fichero (formato 8.3)
	strcpy(spaceUri, readSpaceUri(graphFile));
	//Serial.print("1.5. spaceUri: ");
	//Serial.print(spaceUri);
	//Serial.print("|");
	//Serial.println(space);
  
	//Si la URI leida en el fichero no es la misma que hemos pasado a la funcion o el fichero no esta vacio, se sigue leyendo.
	while((strcmp(spaceUri, space)!=0)&&(strcmp(spaceUri, nothingConst)!=0)){
		//Serial.println("dentro");
                c = graphFile.read();
                while ((c != '\n') && (c != '\r')){
			c = graphFile.read();
                        //Serial.println(c);
		}
		memset(spaceUri, '\0', 64);
		strcpy(spaceUri, readSpaceUri(graphFile));
		//Serial.print("1.75. spaceUri: ");
		//Serial.println(spaceUri);
                //Serial.println(strcmp(spaceUri, space));
                //Serial.println(strcmp(spaceUri, nothingConst));
	}

	//Si ha salido del while anterior y el fichero no esta vacio, es que el espacio que se ha pasado a la funcion existe en el fichero y leemos el nombre de su carpeta asociada.
	if (strcmp(spaceUri, nothingConst)!=0){
		//Leer la parte del nombre de la carpeta
		memset(spaceFolder, '\0', 30);
                //Serial.print("2. spaceFolder: ");
		strcpy(spaceFolder, readSpaceFolder(graphFile));
		
		//Serial.println(spaceFolder);
	}
	graphFile.close();
  
	//Serial.print("3. spaceUri(Nothing): ");
	//Serial.println(spaceUri);
	//Si no se ha encontrado el espacio en el fichero, se introduce en el spaces.txt y se crea una carpeta
	if (strcmp(spaceUri, nothingConst)==0){ 
		//Serial.println("4. spaceUri es 'nothing'");
		spacesFile = SD.open(spacesIndexConst, FILE_WRITE);
		if (spacesFile) { 
			spacesFile.print(space);
			spacesFile.print(sepConst);
			indexSpace = EEPROM_readint(0);
			spacesFile.println(indexSpace); 
			spacesFile.close();
		}
		
		//Se crea la carpeta
    	memset(spaceFolder, '\0', 30);
		//Serial.print(spaceFolder);
		itoa(indexSpace, spaceFolder, 10);
		//Serial.print("5. spaceFolder: ");
		//Serial.println(spaceFolder);
		SD.mkdir(spaceFolder);
		//Serial.print("Creating spaceFolder: ");
		//Serial.println(spaceFolder);
		indexSpace++;
		EEPROM_writeint(0, indexSpace);
	}
  
	//Guardar en la SD en el directorio SD:\space_uri\uri.txt las tripletas "s;p;o"
  
	if (firstTriple){
		//Se crea la URI para el nuevo conjunto de tripletas y se comprueba si existe
		indexGraph = EEPROM_readint(5);
		memset(URL_part2, '\0', 30);
		itoa(indexGraph, URL_part2, 10);
		indexGraph++;
		EEPROM_writeint(5, indexGraph);
		firstWrite = false;
	}
	else
	{
		indexGraph = EEPROM_readint(5)-1;
		memset(URL_part2, '\0', 30);
		itoa(indexGraph, URL_part2, 10);
	}
    
	char uriGraph[64];
	memset(uriGraph, '\0', 64);
	strcat(uriGraph, spaceFolder);
	strcat(uriGraph, sepUrlConst);
	strcat(uriGraph, URL_part2);
	strcat(uriGraph, extConst);
  
	delay(300);
  
	// open a new file and immediately close it:
	//Serial.print("7. Creating ");
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
	//Serial.println(uriGraphcpy);
	graphFile = SD.open(uriGraphcpy, FILE_WRITE); //No utilizar nombres largos de fichero
	if (graphFile){
		graphFile.print(subject);
		graphFile.print('|');
		graphFile.print(predicate);
		graphFile.print('|');
                graphFile.print(objType);
		graphFile.print('|');
                graphFile.print(object);
		graphFile.println('|');
		graphFile.close();
	}

	char uriGraphReturn[64];
	memset(uriGraphReturn, '\0', 64);
        strcat(uriGraphReturn, macIdentifier);
        strcat(uriGraphReturn, sepUrlConst);
	strcat(uriGraphReturn, space);
	strcat(uriGraphReturn, sepUrlConst);
	strcat(uriGraphReturn, strtok(uriGraphcpy, ".")); 
	  
	return urlencode(uriGraphReturn);
}



//GET /spaces/(uri: space)/graphs
//***********************************************************************************************
// FUNCTION   READOTHERSGRAPH(SPACE) 
//***********************************************************************************************
char* readOthersGraph(char* spaceUri){
    char httpGET[128];
    memset(httpGET, '\0', 128);
    strcat(httpGET, "GET http://");
    strcat(httpGET, ipBroadcast);
    strcat(httpGET, "/spaces/");
    strcat(httpGET, spaceUri);
    strcat(httpGET, "/graphs");
    // if you get a connection, report back via serial:
    if (clientOtsopack.connect(broadcastServer, 80)) {
      Serial.println("connected");
      Serial.println(httpGET);
      // Make a HTTP request:
      clientOtsopack.println(httpGET);
      clientOtsopack.println();
    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
    }
    memset(httpGET, '\0', 128);
  
}


//GET /spaces/(uri: space)/graphs/(uri: graph)
//***********************************************************************************************
// FUNCTION   READOTHERSGRAPH(SPACE, GRAPH) 
//***********************************************************************************************
char* readOthersGraph(char* spaceUri, char* graphUri){
    char httpGET[128];
    memset(httpGET, '\0', 128);
    strcat(httpGET, "GET http://");
    strcat(httpGET, ipBroadcast);
    strcat(httpGET, "/spaces/");
    strcat(httpGET, spaceUri);
    strcat(httpGET, "/graphs/");
    strcat(httpGET, graphUri);
    // if you get a connection, report back via serial:
    if (clientOtsopack.connect(broadcastServer, 80)) {
      Serial.println("connected");
      Serial.println(httpGET);
      // Make a HTTP request:
      clientOtsopack.println(httpGET);
      clientOtsopack.println();
    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
    }
    memset(httpGET, '\0', 128);    
}


// GET /spaces/(uri: space)/graphs/wildcards/(uri: subject)/(uri: predicate)/(uri: object)
//***********************************************************************************************
// FUNCTION   READOTHERSGRAPH(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI de un grafo que contiene esta tripleta
//***********************************************************************************************
char* readOthersGraph(char* spaceUri, char* subject, char* predicate, char* object){
    char httpGET[128];
    memset(httpGET, '\0', 128);
    strcat(httpGET, "GET http://");
    strcat(httpGET, ipBroadcast);
    strcat(httpGET, "/spaces/");
    strcat(httpGET, spaceUri);
    strcat(httpGET, "/graphs/wildcards/");
    strcat(httpGET, subject);
    strcat(httpGET, sepUrlConst);
    strcat(httpGET, predicate);
    strcat(httpGET, sepUrlConst);
    strcat(httpGET, object);
    strcat(httpGET, sepUrlConst);
    // if you get a connection, report back via serial:
    if (clientOtsopack.connect(broadcastServer, 80)) {
      Serial.println("connected");
      Serial.println(httpGET);
      // Make a HTTP request:
      clientOtsopack.println(httpGET);
      clientOtsopack.println();
    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
    }
    memset(httpGET, '\0', 128);    
}





// DELETE /spaces/(uri: space)/graphs/(uri: graph)
//***********************************************************************************************
// FUNCTION   DELETEOTHERSGRAPH(SPACE, GRAPH) 
//***********************************************************************************************
char* deleteOthersGraph(char* spaceUri, char* graphUri){
    char httpGET[128];
    memset(httpGET, '\0', 128);
    strcat(httpGET, "DELETE http://");
    strcat(httpGET, ipBroadcast);
    strcat(httpGET, "/spaces/");
    strcat(httpGET, spaceUri);
    strcat(httpGET, "/graphs/");
    strcat(httpGET, graphUri);
    // if you get a connection, report back via serial:
    if (clientOtsopack.connect(broadcastServer, 80)) {
      Serial.println("connected");
      Serial.println(httpGET);
      // Make a HTTP request:
      clientOtsopack.println(httpGET);
      clientOtsopack.println();
    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
    }
    memset(httpGET, '\0', 128);
}


// DELETE /spaces/(uri: space)/graphs/wildcards/(uri: subject)/(uri: predicate)/(uri: object)
//***********************************************************************************************
// FUNCTION   DELETEOTHERSGRAPH(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI de un grafo que contiene esta tripleta
//***********************************************************************************************
char* deleteOthersGraph(char* spaceUri, char* subject, char* predicate, char* object){
    char httpGET[128];
    memset(httpGET, '\0', 128);
    strcat(httpGET, "DELETE http://");
    strcat(httpGET, ipBroadcast);
    strcat(httpGET, "/spaces/");
    strcat(httpGET, spaceUri);
    strcat(httpGET, "/graphs/wildcards/");
    strcat(httpGET, subject);
    strcat(httpGET, sepUrlConst);
    strcat(httpGET, predicate);
    strcat(httpGET, sepUrlConst);
    strcat(httpGET, object);
    strcat(httpGET, sepUrlConst);
    // if you get a connection, report back via serial:
    if (clientOtsopack.connect(broadcastServer, 80)) {
      Serial.println("connected");
      Serial.println(httpGET);
      // Make a HTTP request:
      clientOtsopack.println(httpGET);
      clientOtsopack.println();
    } 
    else {
      // kf you didn't get a connection to the server:
      Serial.println("connection failed");
    }
    memset(httpGET, '\0', 128);
}
