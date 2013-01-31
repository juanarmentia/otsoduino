//***********************************************************************************************
// FUNCIÓN   WRITE(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI del grafo guardado 
//***********************************************************************************************
char* writeTriple(char* space, char* subject, char* predicate, char* object, boolean firstTriple){
  
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
			indexSpace = EEPROM_readint(0);
			spacesFile.println(indexSpace); 
			spacesFile.close();
		}
		
		//Se crea la carpeta
    	memset(spaceFolder, '\0', 30);
		//Serial.print(spaceFolder);
		itoa(indexSpace, spaceFolder, 10);
		Serial.print("5. spaceFolder: ");
		Serial.println(spaceFolder);
		SD.mkdir(spaceFolder);
		Serial.print("Creating spaceFolder: ");
		Serial.println(spaceFolder);
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
	  
	return urlencode(uriGraphReturn);
}



//***********************************************************************************************
// FUNCIÓN   READGRAPH(SPACE, GRAPH)::Devuelve URI de un grafo como el que se le ha pasado 
//***********************************************************************************************
char* readGraph(char* space, char* graph){
  
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
	Serial.print("Folder: ");
	Serial.println(readSpaceFolder(fileSpaces));
	char folder[30];
	memset(folder, '\0', 30);
	strcpy(folder, readSpaceFolder(fileSpaces));
	fileSpaces.close();
	
	Serial.println("uriGraph");
	strcat(uriGraph, myIP);
	Serial.println(uriGraph);
	strcat(uriGraph, space);
	Serial.println(uriGraph);
	strcat(uriGraph, sepUrlConst);
	Serial.println(uriGraph);
	strcat(uriGraph,folder);
	Serial.println(uriGraph);
	strcat(uriGraph, sepUrlConst);
	Serial.println(uriGraph);
	strcat(uriGraph, graph);
	Serial.println(uriGraph);

	return urlencode(uriGraph);
}


//***********************************************************************************************
// FUNCIÓN   READGRAPH(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI de un grafo que contiene ésta tripleta
//***********************************************************************************************
char* readGraph(char* space, char* subject, char* predicate, char* object){
  
	//Hay que recorrer todos los ficheros y carpetas en busca del patrón s,p,o. Cuando se encuentre, se devolverá un String
	//con todas las líneas del fichero en el que se encuentra el patrón concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteración se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patrón. 
	//Cuando se encuentre el patrón, se lee de nuevo ese fichero que tendrá la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);

	//Se abre el fichero SPACES.TXT donde se encuentra la relación de espacios con número de carpeta
	File fileSpaces = SD.open("SPACES.TXT");
	while(strcmp(readSpaceUri(fileSpaces),space)!=0){
		//readSpaceFolder(fileSpaces);
	}
	//Serial.print("Folder: ");
	//Serial.println(readSpaceFolder(fileSpaces));
	char folder[30];
	memset(folder, '\0', 30);
	strcpy(folder, readSpaceFolder(fileSpaces));
	fileSpaces.close();
	//Serial.print("folder: ");
	//Serial.println(folder);
	
	File root = SD.open("/");

	//Serial.println("********************************** 1");
	searchPath(root, 0, true, folder, subject, predicate, object, space);
	//Serial.println("********************************** 2");
	Serial.println("");
	Serial.print("selectedPath: ");
	Serial.println(selectedPath);
	Serial.print("uriGraph: ");
	Serial.println(uriGraph);
	Serial.print("Graph: ");
	Serial.println(graph);

	root.close();
	
	//return urlencode(uriGraph);
	return graph;
}


void searchPath(File dir, int numTabs, boolean firstFile, char* folder, char* subject, char*predicate, char* object, char* space) {
	//Serial.print("Inicio ---------------------");
	//Serial.println(numTabs);
	char subj[80];
	memset(subj, '\0', 80);
	char pred[80];
	memset(pred, '\0', 80);
	char obj[80];
	memset(obj, '\0', 80);

	while(true) {
		
		if(firstFile){
			memset(directory, '\0', 30);
			memset(directorySpace, '\0', 30);
		}
		File entry =  dir.openNextFile();
		if (!entry) {
			//no more files
			Serial.println("**no more files**");
			break;
		}/*else{
			Serial.print("Name: ");
			Serial.println(entry.name());
		}*/
		if (entry.isDirectory()) {
			//Serial.print("Directory name: ");
			//Serial.println(entry.name());
			if(atoi(entry.name()) == atoi(folder)){
				strcpy(directory, entry.name());
				strcat(directory, "/");
				Serial.print("Directory: ");
				Serial.println(directory);
				strcpy(directorySpace,directory);
				selectedDirectory = true;
			}
			else
			{
				selectedDirectory = false;
			}
			firstFile = false;
			searchPath(entry, numTabs+1, firstFile, folder, subject, predicate, object, space);
		} else if(selectedDirectory) {
				strcpy(directory,directorySpace);
				//Serial.println("");
				Serial.print("Abriendo ");
				Serial.print(directory);
				Serial.println(entry.name());
				File selectedFile = SD.open(strcat(directory,entry.name()));
				//Serial.print("Ruta: ");
				//Serial.println(directory);
				Serial.print("Tripleta a buscar: |");
				Serial.print(subject);
				Serial.print("|");
				Serial.print(predicate);
				Serial.print("|");
				Serial.print(object);
				Serial.println("|");
				memset(graph, '\0', 2500);
				while (selectedFile.available()){
					strcpy(subj, readSpaceUri(selectedFile));
					strcpy(pred, readSpaceUri(selectedFile));
					strcpy(obj, readSpaceUri(selectedFile));
					strcat(graph, subj);
					strcat(graph, sepConst);
					strcat(graph, pred);
					strcat(graph, sepConst);
					strcat(graph, obj);
					strcat(graph, sepConst);
					Serial.print("Tripleta leida: ");
					Serial.print(subj);
					Serial.print("|");
					Serial.print(pred);
					Serial.print("|");
					Serial.print(obj);
					Serial.println("|");
					if (strcmp(subj,subject) == 0){
						if(strcmp(pred,predicate) == 0){
							if(strcmp(obj,object) == 0){
								strcpy(selectedPath, directory);
								strcat(uriGraph, myIP);
								//strcat(uriGraph, urlencode(space));
								//strcat(uriGraph, "/");
								strcat(uriGraph, strtok(directory, "."));
								//Serial.print("uriGraph: ");
								//Serial.println(uriGraph);
								return;//Salir de la función
							}
						}
					}
				}

				Serial.println("");
		}
	}
	//Serial.print("Fin -------------------- ");
	//Serial.println(numTabs);
}


//Read the side where space`s URI is written
char* readSpaceUri(File gFile){
	char rSpaceUri[64];
	memset(rSpaceUri, '\0', 64);
	int index = 0;
	char c = gFile.read();
	if (c!=-1){
		while ((c != '|')){
			if ((c != '\n')&&(c != '\r')){
				rSpaceUri[index++] = c;
			}
			c = gFile.read();
		}
		delay(200);
	}
	else{
		return "nothing"; 
	}
	//Serial.print("Termino leido: |");
	//Serial.print(rSpaceUri);
	//Serial.println("|");
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
	//Serial.print("readSpaceFolder(): ");
	//Serial.println(rSpaceFolder);
	return rSpaceFolder;
}

//Creating the RDF-N3 graph
void createGraph(char* strGraph, EthernetClient client){
	char rTerm[256];
	memset(rTerm, '\0', 256);
	int index = 0;
	int position = 0;
	int aux;
	for (int i=0;i<strlen(strGraph);i++){
		if(strGraph[i]!='|'){
			rTerm[index++] = strGraph[i];
		}
		else{
			position++;
			aux = position;
			while (aux > 3){
				aux = aux - 3;
			}
			switch (aux){
				case 1:{ //Subject
					client.print('<');
					client.print(rTerm);
					client.print('>');
				}break;
				case 2:{ //Predicate
					client.print(" ");
					client.print('<');
					client.print(rTerm);
					client.print('>');
				}break;
				case 3:{ //Object
					client.print(" ");
					if (isUrl(rTerm)){
						client.print('<');
						client.print(rTerm);
						client.print('>');
						client.print('.');
					}else{
						client.print('"');
						client.print(rTerm);
						client.print('"');
						client.println('.');
					}
				}break;
			}
			memset(rTerm, '\0', 256);
			index = 0;
		}
	}

}
