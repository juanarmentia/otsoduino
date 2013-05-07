void searchPath(File dir, char* folder, char* subject, char*predicate, char* object, char* space, boolean deleteTriple) {	
	
        boolean tripleFound = false;

        while(true) {		
		File entry =  dir.openNextFile();
		if (!entry) {
			//no more files
			//Serial.println("**no more files**");
			break;
		}
                //Serial.print("Name: ");
		//Serial.println(entry.name());
                strcpy(directory,sepUrlConst);
                strcat(directory,folder);
                strcat(directory,sepUrlConst);
		File selectedFile = SD.open(strcat(directory,entry.name()));
                //Serial.println(selectedFile);
		//Serial.print("Ruta: ");
		//Serial.println(directory);
		memset(graph, '\0', 2000);
                //Serial.print("freeMemory()=");
                //Serial.println(freeMem);
		while (selectedFile.available()){
                        memset(subj, '\0', 80);
        	        memset(pred, '\0', 80);
                        memset(obj, '\0', 80);
                        strcpy(subj, readTripleTerm(selectedFile));
                        //Serial.println(subj);
                        //Serial.println(strlen(subj));
                        if (strlen(subj) == 0){
                          //Serial.println("break");  
                          break;
                        }  
			strcpy(pred, readTripleTerm(selectedFile));
			strcpy(obj, readTripleTerm(selectedFile));
                        //Serial.println(strlen(subj));
                        //Serial.println(strlen(pred));
                        //Serial.println(strlen(obj));
			//Serial.print("Tripleta leida: ");
			//Serial.print(subj);
			//Serial.print("|");
			//Serial.print(pred);
			//Serial.print("|");
			//Serial.print(obj);
			//Serial.println("|");
                        strcat(graph, subj);
                        strcat(graph, sepConst);
                        strcat(graph, pred);
                        strcat(graph, sepConst);
                        strcat(graph, obj);
                        strcat(graph, sepConst);
			if (strcmp(subj,subject) == 0){
				if(strcmp(pred,predicate) == 0){
					if(strcmp(obj,object) == 0){
                                                memset(selectedPath, '\0', 64);
						strcpy(selectedPath, directory);
						strcat(uriGraph, macIdentifier);
						//strcat(uriGraph, urlencode(space));
						//strcat(uriGraph, "/");
						strcat(uriGraph, strtok(directory, "."));
						//Serial.print("uriGraph: ");
						//Serial.println(uriGraph);
                                                //Serial.println("Final");
                                                tripleFound = true;
                                                //selectedFile.close();
						//return;//Salir de la función
					}
				}
			}
                        //Serial.println(int(selectedFile.read()));
                        //Serial.println(int(selectedFile.read()));
		}
                selectedFile.close();
                //Serial.println(tripleFound);
                if (tripleFound){
                    if (deleteTriple){
                        if (SD.remove(strcat(directory,extConst))){
                            //Serial.println(directory);
                            //Serial.println("borrado");
                            return;
                        }
                        else{
                            //Serial.println(directory);
                            //Serial.println("no borrado");
                            return;
                        }
                    }
                    return;
                }
                else{
                    memset(subj, '\0', 80);
        	    memset(pred, '\0', 80);
                    memset(obj, '\0', 80);
                    memset(graph, '\0', 2000);
                }
	}
        return;
}


//Read the different parts of a triple
char* readTripleTerm(File gFile){
	char rTerm[64];
	memset(rTerm, '\0', 64);
	int index = 0;
	char c = gFile.read();
	while ((gFile.available()) && (c != '|')){
		if ((c != '\n')&&(c != '\r')){
			rTerm[index++] = c;
		}
		c = gFile.read();
	}
	return rTerm;
}


//Read the side where space`s URI is written
char* readSpaceUri(File gFile){
	char rSpaceUri[64];
	memset(rSpaceUri, '\0', 64);
	int index = 0;
        char c = gFile.read();
	if (c!=-1){
	    while ((c != '|') && (c != -1)){
	        if ((c != '\n')&&(c != '\r')){
		    rSpaceUri[index++] = c;
		}
		c = gFile.read();
	    }
	    delay(200);
	}
	else{
	    //Serial.println("nothing");
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
        //Serial.println(int(c));
	while ((c != '\n') && (c != '\r') && (c != ' ') && (c != -1)){
		rSpaceFolder[index++] = c;
		delay(200);
		c = gFile.read();
                //Serial.println(int(c));
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


//Separating uriGraph
void sepUriGraph(char* uri_Graph, char* macPart, char* pathPart){
	int divided = 0;
	int indexPath = 0;
	int indexMac = 0;
	for(int i=0; i<strlen(uri_Graph); i++){
		if (divided){
			pathPart[indexPath++] = uri_Graph[i];
		}else if (uri_Graph[i] != '/') {
			macPart[indexMac++] = uri_Graph[i];
		}else divided = 1;
	}
}


//Read all the graphs of a space
int printDirectorySpace(File dir, int numTabs) {
   
        char subj[80];
	memset(subj, '\0', 80);
	char pred[80];
	memset(pred, '\0', 80);
	char obj[80];
	memset(obj, '\0', 80);
	int index = 0;
	//memset(graphBySpace, '\0', 2000);
        memset(graph, '\0', 2000);
	
	while(true) {

		File entry =  dir.openNextFile();
		if (! entry) {
			// no more files
			//Serial.println("**nomorefiles**");
			//Serial.println(graph);
			return 1;
		}
		for (int i=0; i<numTabs; i++) {
			//Serial.print('\t');
		}
		//Serial.print(entry.name());
		if (entry.isDirectory()) {
			//Serial.println("/");
			printDirectorySpace(entry, numTabs+1);
		} else {
			// files have sizes, directories do not
			//Serial.print("\t\t");
			//Serial.println(entry.size(), DEC);

			if (entry) { 	
				// read from the file until there's nothing else in it:
				while (entry.available()) {
					char c = entry.read();
					if((c!='\n')&&(c!='\r')){
						graph[index++] = c;
					}
				}
				graph[index++] = '$';
				
				// close the file:
				entry.close();
			} else {
				// if the file didn't open, print an error:
				//Serial.print("error opening ");
				//Serial.println(entry.name());
				return 0;
			}

		}
	}
}

//Separating array in RDF-N3 graphs
void separateGraph(char* groupGraph, EthernetClient client){
	char rTerm[512];
	memset(rTerm, '\0', 512);
	int index = 0;
	int position = 0;
	int aux;
	for (int i=0;i<strlen(groupGraph);i++){
                //Serial.println(groupGraph[i]);
		if(groupGraph[i]!='$'){
			rTerm[index++] = groupGraph[i];
		}
		else{
			//Serial.println("creating graph");
                        createGraph(rTerm, client);
                        
			//client.println("New graph");
			memset(rTerm, '\0', 512);
			index = 0;
		}
	}
}




//void searchPath(File dir, int numTabs, boolean firstFile, char* folder, char* subject, char*predicate, char* object, char* space) {	
//	
//        Serial.println("Inicio --------------------- ");
//        Serial.println(dir);
//        Serial.println(numTabs);
//        Serial.println(firstFile);
//        Serial.println(folder);
//
//        while(true) {		
//		/*if(firstFile){
//			memset(directory, '\0', 30);
//			memset(directorySpace, '\0', 30);
//		}*/
//		File entry =  dir.openNextFile();
//		if (!entry) {
//			//no more files
//			//Serial.println("**no more files**");
//			break;
//		}
//                Serial.print("Name: ");
//		Serial.println(entry.name());
//		if (entry.isDirectory()) {
//                        Serial.println("Es un directorio");
//			//Serial.print("Directory name: ");
//			//Serial.println(entry.name());
//			/*if(atoi(entry.name()) == atoi(folder)){
//				strcpy(directory, entry.name());
//				strcat(directory, "/");
//				Serial.print("Directory: ");
//				Serial.println(directory);
//				strcpy(directorySpace,directory);
//				selectedDirectory = true;
//			}
//			else
//			{
//				selectedDirectory = false;
//			}*/
//			firstFile = false;
//			searchPath(entry, numTabs+1, firstFile, folder, subject, predicate, object, space);
//		} 
//               /* else if(selectedDirectory) {
//			        //Serial.println("hola");
//                        	
//	                        
//                                strcpy(directory,directorySpace);
//				//Serial.println("");
//				//Serial.print("Abriendo ");
//				//Serial.print(directory);
//				//Serial.println(entry.name());
//				File selectedFile = SD.open(strcat(directory,entry.name()));
//				//Serial.print("Ruta: ");
//				//Serial.println(directory);
//				//Serial.print("Tripleta a buscar: |");
//				//Serial.print(subject);
//				//Serial.print("|");
//				//Serial.print(predicate);
//				//Serial.print("|");
//				//Serial.print(object);
//				//Serial.println("|");
//				/*memset(graph, '\0', 2000);
//				while (selectedFile.available()){
//                                        memset(subj, '\0', 80);
//        	                        memset(pred, '\0', 80);
//                                	memset(obj, '\0', 80);
//					strcpy(subj, readSpaceUri(selectedFile));
//					strcpy(pred, readSpaceUri(selectedFile));
//					strcpy(obj, readSpaceUri(selectedFile));
//					strcat(graph, subj);
//					strcat(graph, sepConst);
//					strcat(graph, pred);
//					strcat(graph, sepConst);
//					strcat(graph, obj);
//					strcat(graph, sepConst);
//					//Serial.print("Tripleta leida: ");
//					//Serial.print(subj);
//					//Serial.print("|");
//					//Serial.print(pred);
//					//Serial.print("|");
//					//Serial.print(obj);
//					//Serial.println("|");
//					if (strcmp(subj,subject) == 0){
//						if(strcmp(pred,predicate) == 0){
//							if(strcmp(obj,object) == 0){
//                                                                memset(selectedPath, '\0', 64);
//								strcpy(selectedPath, directory);
//								strcat(uriGraph, myIP);
//								//strcat(uriGraph, urlencode(space));
//								//strcat(uriGraph, "/");
//								strcat(uriGraph, strtok(directory, "."));
//								//Serial.print("uriGraph: ");
//								//Serial.println(uriGraph);
//								return;//Salir de la función
//							}
//						}
//					}
//				}
//
//				//Serial.println("");
//		}*/
//	}
//	//Serial.print("Fin -------------------- ");
//	//Serial.println(numTabs);
//}

