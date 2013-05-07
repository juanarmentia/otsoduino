//***********************************************************************************************
// FUNCION   WRITE(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI del grafo guardado 
//***********************************************************************************************
char* writeTriple(char* space, char* subject, char* predicate, char* object, boolean firstTriple){
  
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
// FUNCTION   READGRAPH(SPACE) 
//***********************************************************************************************
 char* readGraph(char* spaceUri){
    
	/*Serial.println("Dentro del read by space");*/
        //Serial.println(spaceUri);

	//Hay que recorrer todos los ficheros y carpetas en busca del patron s,p,o. Cuando se encuentre, se devolvera un String
	//con todas las lineas del fichero en el que se encuentra el patron concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteracion se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patron. 
	//Cuando se encuentre el patron, se lee de nuevo ese fichero que tendra la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);
        memset(graph, '\0', 2000);

	//Se abre el fichero SPACES.TXT donde se encuentra la relaciÃ³n de espacios con nÃºmero de carpeta
	File fileSpaces = SD.open(spacesIndexConst);
        //Serial.println(fileSpaces);
        if (fileSpaces == 0){
            return NULL;
        }
        //Serial.println(fileSpaces);
        strcpy(selectedPath, readSpaceUri(fileSpaces));
	while(strcmp(selectedPath,spaceUri)!=0){
		//Serial.println(selectedPath);
                if (strcmp(selectedPath,"nothing")!=0){
                    readSpaceFolder(fileSpaces);
                }
                else return NULL;
                strcpy(selectedPath, readSpaceUri(fileSpaces));
	}
        
        char folder[30];
	memset(folder, '\0', 30);
	strcpy(folder, readSpaceFolder(fileSpaces));
        //Serial.println(folder);
        //Serial.println(strlen(folder));
        //folder[strlen(folder)-1] = '\0';
        fileSpaces.close();	
        //Serial.print("Folder: ");
	//Serial.println(folder);
	
        char rootPath[30];
        memset(rootPath, '\0', 30);
	strcpy(rootPath, sepUrlConst);
        strcat(rootPath, folder);
        strcat(rootPath, sepUrlConst);
        //Serial.println(rootPath);
        //Serial.println(strlen(rootPath));
	File root = SD.open(rootPath);
        //Serial.println(root);
        	
	//LEER TODO LO QUE HAY EN LA CARPETA folder (e.g. carpeta "SD:/8/")
	
        if(printDirectorySpace(root, 0)){
		root.close();
		return "OK";
	}else{
		root.close();
		return NULL;
	}

}


//GET /spaces/(uri: space)/graphs/(uri: graph)
//***********************************************************************************************
// FUNCTION   READGRAPH(SPACE, GRAPH) 
//***********************************************************************************************
char* readGraph(char* spaceUri, char* graphUri){
    
	//Hay que recorrer todos los ficheros y carpetas en busca del patron s,p,o. Cuando se encuentre, se devolvera un String
	//con todas las lineas del fichero en el que se encuentra el patron concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteracion se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patron. 
	//Cuando se encuentre el patron, se lee de nuevo ese fichero que tendra la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	int index = 0;
	int spaceExist = 0;
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);
	char part1Mac[15];
	memset(part1Mac, '\0',15);
	char part2Path[15];
	memset(part2Path, '\0',15);
	char path[15];
	memset(path, '\0', 15);
	char aux[15];
	memset(aux, '\0',15);
	char spaceNumber[15];
	memset(spaceNumber, '\0',15);
	char folder[15];
        memset(graph, '\0', 2000);

	sepUriGraph(graphUri, part1Mac, part2Path);
	//Serial.println(part1Mac);
	//Serial.println(part2Path);
	//Serial.println(macIdentifier);

	if (strcmp(part1Mac, macIdentifier)!=0){
		//Serial.println("no coincide");
		return NULL;
	}

	sepUriGraph(part2Path, spaceNumber, aux);
	
	File fileSpaces = SD.open(spacesIndexConst);
        //Serial.println(fileSpaces);
        if (fileSpaces == 0){
          return NULL;
        }
        while (fileSpaces.available()){
                //Serial.println(readSpaceUri(fileSpaces));
                //Serial.println(urldecode(spaceUri));
                //Serial.println(strcmp(readSpaceUri(fileSpaces),urldecode(spaceUri)));
                //Serial.println("**************************");
                strcpy(selectedPath, readSpaceUri(fileSpaces));
		if (strcmp(selectedPath,urldecode(spaceUri)) == 0){
                        memset(selectedPath, '\0', 64);
                        memset(folder, '\0', 15);
        		strcpy(folder, readSpaceFolder(fileSpaces));
        		//folder[strlen(folder)-1] = '\0';
        		if(strcmp(folder,spaceNumber)==0){
        			spaceExist = 1;
                                exit;
        		}
                }
                else{
                        //Serial.println("else");
                        readSpaceFolder(fileSpaces); 
                }
		//Serial.println(spaceExist);
	}
        //Serial.println(spaceExist);
	if (spaceExist){

		//strcat(path, sepUrlConst); 
                strcat(path, part2Path);
		strcat(path, extConst);
                //Serial.println(path);
		File fileGraph = SD.open(path, FILE_READ);
                //Serial.println(fileGraph);
		if (fileGraph) { 
			memset(graph, '\0', 2000);
			// read from the file until there's nothing else in it:
			while (fileGraph.available()) {
				char c = fileGraph.read();
				if((c!='\n')&&(c!='\r')){
					graph[index++] = c;
				}
			}
			// close the file:
			fileGraph.close();
		} else {
			// if the file didn't open, print an error:
			//Serial.print("error opening ");
			//Serial.println(path);
			return NULL;
		}

		//Serial.println(graph);
	}else return NULL;

	return graph;
}


// GET /spaces/(uri: space)/graphs/wildcards/(uri: subject)/(uri: predicate)/(uri: object)
//***********************************************************************************************
// FUNCTION   READGRAPH(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI de un grafo que contiene esta tripleta
//***********************************************************************************************
char* readGraph(char* space, char* subject, char* predicate, char* object){
    

	//Hay que recorrer todos los ficheros y carpetas en busca del patron s,p,o. Cuando se encuentre, se devolvera un String
	//con todas las lineas del fichero en el que se encuentra el patron concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteracion se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patron. 
	//Cuando se encuentre el patron, se lee de nuevo ese fichero que tendra la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);
        memset(graph, '\0', 2000);

	//Se abre el fichero SPACES.TXT donde se encuentra la relacion de espacios con numero de carpeta
	File fileSpaces = SD.open(spacesIndexConst);
        if (fileSpaces == 0){
                return NULL;
        }
       
        strcpy(selectedPath, readSpaceUri(fileSpaces));
	while(strcmp(selectedPath,space)!=0){
		//Serial.println("selectedPath: ");
                //Serial.println(selectedPath);
                if (strcmp(selectedPath,"nothing")!=0){
                    readSpaceFolder(fileSpaces);
                }
                else return NULL;
                strcpy(selectedPath, readSpaceUri(fileSpaces));
                //Serial.println(selectedPath);
	}
        
        char folder[30];
	memset(folder, '\0', 30);
	strcpy(folder, readSpaceFolder(fileSpaces));
        //folder[strlen(folder)-1] = '\0';
        fileSpaces.close();
        //Serial.println(folder);
        
        char rootPath[30];
        memset(rootPath, '\0', 30);
	strcpy(rootPath, sepUrlConst);
        strcat(rootPath, folder);
        strcat(rootPath, sepUrlConst);
	File root = SD.open(rootPath);
        
        //File root = SD.open("/");
        //root.rewindDirectory();
        //Serial.println(root);
      	//Serial.println("********************************** 1");
      	searchPath(root, folder, subject, predicate, object, space, 0);
      	//Serial.println("********************************** 2");
      	//Serial.println("");
      	//Serial.print("selectedPath: ");
      	//Serial.println(selectedPath);
      	//Serial.print("uriGraph: ");
      	//Serial.println(uriGraph);
      	//Serial.print("Graph: ");
      	//Serial.println(graph);
      
      	root.close();
	
      	//return urlencode(uriGraph);
        //return graph;
        //Serial.print("graph: ");
        //Serial.print(graph);
        //Serial.print("|");
        //Serial.println(strlen(graph));
        if (strlen(graph) == 0)
            return NULL;
        else 
            return graph;
}





// DELETE /spaces/(uri: space)/graphs/(uri: graph)
//***********************************************************************************************
// FUNCTION   DELETEGRAPH(SPACE, GRAPH) 
//***********************************************************************************************
char* deleteGraph(char* spaceUri, char* graphUri){
    
	//Hay que recorrer todos los ficheros y carpetas en busca del patron s,p,o. Cuando se encuentre, se devolvera un String
	//con todas las lineas del fichero en el que se encuentra el patron concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteracion se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patron. 
	//Cuando se encuentre el patron, se lee de nuevo ese fichero que tendra la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	int index = 0;
	int spaceExist = 0;
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);
	char part1Mac[15];
	memset(part1Mac, '\0',15);
	char part2Path[15];
	memset(part2Path, '\0',15);
	char path[15];
	memset(path, '\0', 15);
	char aux[15];
	memset(aux, '\0',15);
	char spaceNumber[15];
	memset(spaceNumber, '\0',15);
	char folder[15];
        memset(graph, '\0', 2000);

	sepUriGraph(graphUri, part1Mac, part2Path);
	//Serial.println(part1Mac);
	//Serial.println(part2Path);
	//Serial.println(macIdentifier);

	if (strcmp(part1Mac, macIdentifier)!=0){
		//Serial.println("no coincide");
		return NULL;
	}

	sepUriGraph(part2Path, spaceNumber, aux);
	
	File fileSpaces = SD.open(spacesIndexConst);
        //Serial.println(fileSpaces);
        if (fileSpaces == 0){
          return NULL;
        }
        while (fileSpaces.available()){
                //Serial.println(readSpaceUri(fileSpaces));
                //Serial.println(urldecode(spaceUri));
                //Serial.println(strcmp(readSpaceUri(fileSpaces),urldecode(spaceUri)));
                //Serial.println("**************************");
                strcpy(selectedPath, readSpaceUri(fileSpaces));
		if (strcmp(selectedPath,urldecode(spaceUri)) == 0){
                        memset(selectedPath, '\0', 64);
                        memset(folder, '\0', 15);
        		strcpy(folder, readSpaceFolder(fileSpaces));
        		//folder[strlen(folder)-1] = '\0';
        		if(strcmp(folder,spaceNumber)==0){
        			spaceExist = 1;
                                exit;
        		}
                }
                else{
                        //Serial.println("else");
                        readSpaceFolder(fileSpaces); 
                }
		//Serial.println(spaceExist);
	}
        //Serial.println(spaceExist);
	if (spaceExist){

		//strcat(path, sepUrlConst); 
                strcat(path, part2Path);
		strcat(path, extConst);
                //Serial.println(path);
		File fileGraph = SD.open(path, FILE_READ);
                //Serial.println(fileGraph);
		if (fileGraph) { 
			memset(graph, '\0', 2000);
			// read from the file until there's nothing else in it:
			while (fileGraph.available()) {
				char c = fileGraph.read();
				if((c!='\n')&&(c!='\r')){
					graph[index++] = c;
				}
			}
			// close the file:
			fileGraph.close();
		} else {
			// if the file didn't open, print an error:
			//Serial.print("error opening ");
			//Serial.println(path);
			return NULL;
		}

		//Serial.println(graph);
	}else return NULL;
        Serial.println(path);
        if (SD.remove(path)){
              //Serial.println("borrado");
        }
        else{ 
              //Serial.println("no borrado");
        }
        
	return graph;

}


// DELETE /spaces/(uri: space)/graphs/wildcards/(uri: subject)/(uri: predicate)/(uri: object)
//***********************************************************************************************
// FUNCTION   DELETEGRAPH(SPACE, SUBJECT, PREDICATE, OBJECT)::Devuelve URI de un grafo que contiene esta tripleta
//***********************************************************************************************
char* deleteGraph(char* space, char* subject, char* predicate, char* object){
    

	//Hay que recorrer todos los ficheros y carpetas en busca del patron s,p,o. Cuando se encuentre, se devolvera un String
	//con todas las lineas del fichero en el que se encuentra el patron concatenadas (String = "uri|s;p;o|s;p;o|s;p;o");
 
	//Utilizar openNextFile() y isDirectory(). Con openNextFile() se recorren los ficheros, cada iteracion se comprueba si es directorio con isDirectory().
	//Si es directorio se guarda el nombre en una variable space_uri o uri. Y si es fichero, se guarda su nombre y se parsea en busca del patron. 
	//Cuando se encuentre el patron, se lee de nuevo ese fichero que tendra la ruta siguiente (SD:\space_uri\uri\nombre_fichero.txt) y cada tripleta se mete en un String respuesta = "uri|s;p;o|s;p;o|s;p;o".
	
	memset(selectedPath, '\0', 64);
	memset(uriGraph, '\0', 64);
        memset(graph, '\0', 2000);

	//Se abre el fichero SPACES.TXT donde se encuentra la relacion de espacios con numero de carpeta
	File fileSpaces = SD.open(spacesIndexConst);
        if (fileSpaces == 0){
                return false;
        }
       
        strcpy(selectedPath, readSpaceUri(fileSpaces));
	while(strcmp(selectedPath,space)!=0){
		//Serial.println("selectedPath: ");
                //Serial.println(selectedPath);
                if (strcmp(selectedPath,"nothing")!=0){
                    readSpaceFolder(fileSpaces);
                }
                else return NULL;
                strcpy(selectedPath, readSpaceUri(fileSpaces));
                //Serial.println(selectedPath);
	}
        
        char folder[30];
	memset(folder, '\0', 30);
	strcpy(folder, readSpaceFolder(fileSpaces));
        //folder[strlen(folder)-1] = '\0';
        fileSpaces.close();
        //Serial.println(folder);
        
        char rootPath[30];
        memset(rootPath, '\0', 30);
	strcpy(rootPath, sepUrlConst);
        strcat(rootPath, folder);
        strcat(rootPath, sepUrlConst);
	File root = SD.open(rootPath);
        
        //File root = SD.open("/");
        //root.rewindDirectory();
        //Serial.println(root);
      	//Serial.println("********************************** 1");
      	searchPath(root, folder, subject, predicate, object, space, 1);
      	//Serial.println("********************************** 2");
      	root.close();
      
        if (strlen(graph) == 0)
            return NULL;
        else 
            return graph;

}



