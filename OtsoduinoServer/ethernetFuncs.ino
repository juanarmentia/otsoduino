void httpServer(){
	//Serial.println("listen for incoming clients");
	// listen for incoming clients
	EthernetClient client = server.available();
	char url[128];
	memset(url, '\0', 128);
	
	char* get = "GET ";
	char* space = "/spaces/";
	char* graph = "/graph/";

	int option = 0;
	int i=0;
	boolean urlArrived = false;
	int contBlanks = 0;
	int contSep = 0;
	
	char bufferSpace[15];
	memset(bufferSpace, '\0', 15);
	char bufferGraph[15];
	memset(bufferGraph, '\0', 15);
	char bufferWildcards[15];
	memset(bufferWildcards, '\0', 15);
	char bufferSpaceVar[64];
	memset(bufferSpaceVar, '\0', 64);
	char bufferGraphVar[64];
	memset(bufferGraphVar, '\0', 64);
	char bufferSubjectVar[64];
	memset(bufferSubjectVar, '\0', 64);
	char bufferPredicateVar[64];
	memset(bufferPredicateVar, '\0', 64);
	char bufferObjectVar[64];
	memset(bufferObjectVar, '\0', 64);

	char urlSpace[64];
	char urlSubject[64];
	char urlPredicate[64];
	char urlObject[64];
	char urlGraph[64];
	
	int indexBuffer = 0;
	boolean isGraphVar = true;
	char identifyGet[5];
	memset(identifyGet, '\0', 5);
	identifyGet[0] = '0';
	identifyGet[1] = '0';
	identifyGet[2] = '0';
	int firstChar = 1;

	if (client) {
	// an http request ends with a blank line
	boolean currentLineIsBlank = true;
	while (client.connected()) {
		if (client.available()) {
		char c = client.read();
		//client.print(c);
		//Serial.print(c);
		// if you've gotten to the end of the line (received a newline
		// character) and the line is blank, the http request has ended,
		// so you can send a reply

		if(urlArrived){
			url[i] = c;
			i++;
		}
		
		if (c == ' ' && contBlanks < 2) {
			contBlanks++;
			if (urlArrived == true) {
				urlArrived =false;
			}
			else urlArrived = true;
		}
		
		
		
		

		if (c == '\n' && currentLineIsBlank) {
			
			url[i-1]='\0';
			//client.print("*");
			//client.print(url);
			//client.println("*");
			//client.println(strlen(url));
			//Serial.println(url);
			strcpy(identifyGet,"000");
			Serial.print("indetifyGet: ");
			Serial.println(identifyGet);
			for(int j=0; j < strlen(url); j++){

				if (url[j] == '/'){
					contSep++;
					indexBuffer = 0;

				}
				if (url[j] != '/'){
					//Serial.print("contSep: ");
					//Serial.println(contSep);
					//Serial.print("indetifyGet_in: ");
					//Serial.println(identifyGet);
					switch (contSep)
					{
						case 1:{ //"spaces"
							bufferSpace[indexBuffer] = url[j];
							indexBuffer++;
						}break;
						case 2:{ //spaceUrl
							bufferSpaceVar[indexBuffer++] = url[j];
						}break;
						case 3:{ //"graph" or "graphs"
							identifyGet[0] = '1';
							bufferGraph[indexBuffer++] = url[j];
						}break;
						case 4:{ //graphNumber or "wildcards"
							if (isGraphVar){
								if ((url[j] == 'w' || url[j] == 'W')&&(firstChar)){
									isGraphVar = false;
									bufferWildcards[indexBuffer++] = url[j];
									identifyGet[2] = '1';
								}
								else{
									bufferGraphVar[indexBuffer++] = url[j];
									identifyGet[1] = '1';
									firstChar = 0;
								}
							}
							else
							{
								bufferWildcards[indexBuffer++] = url[j];
							}
						
						}break;
						case 5:{ //Subject
							bufferSubjectVar[indexBuffer++] = url[j];
						}break;
						case 6:{ //Predicate
							bufferPredicateVar[indexBuffer++] = url[j];
						}break;
						case 7:{ //Object
							bufferObjectVar[indexBuffer++] = url[j];
						}break;
						default:{
							client.println("Default");
						}break;
					}
				}
			}
 
			//Aquí hay que enviar lo que recibamos en el readGraph(). Tratar el string y enviarlo
          
			// send a standard http response header
			//client.println("HTTP/1.1 200 OK");
			//client.println("Content-Type: text/html");
			/*client.print("url: ");
			client.println(url);
			//client.println("<br />");
			client.print("bufferSpace: ");
			client.println(bufferSpace);
			//client.println("<br />");
			client.print("bufferSpaceVar: ");
			client.println(bufferSpaceVar);
			//client.println("<br />");
			client.print("bufferGraph: ");
			client.println(bufferGraph);
			//client.println("<br />");
			client.print("bufferGraphVar: ");
			client.println(bufferGraphVar);

			if (contSep>4){
				client.print("bufferWildcards: ");
				client.println(bufferWildcards);
				//client.println("<br />");
				client.print("bufferSubjectVar: ");
			    client.println(bufferSubjectVar);
				//client.println("<br />");
				client.print("bufferPredicateVar: ");
			    client.println(bufferPredicateVar);
				//client.println("<br />");
				client.print("bufferObjectsVar: ");
			    client.println(bufferObjectVar);
				//client.println("<br />");
			}*/

			contSep = 0;
			isGraphVar = true;

			//client.println(identifyGet);
			Serial.println(identifyGet);

			if(strcmp(identifyGet, "110")==0){
				//client.println("110 dentro");
				
				memset(urlSpace, '\0', 64);
				strcpy(urlSpace,urldecode(bufferSpaceVar));
				memset(urlGraph, '\0', 64);
				strcpy(urlGraph,urldecode(bufferGraphVar));
				createGraph(readGraph(urlSpace, urlGraph), client);

			}
			if(strcmp(identifyGet, "101")==0){
				//client.println("101 dentro");

				memset(urlSpace, '\0', 64);
				strcpy(urlSpace,urldecode(bufferSpaceVar));
				memset(urlSubject, '\0', 64);
				strcpy(urlSubject,urldecode(bufferSubjectVar));
				memset(urlPredicate, '\0', 64);
				strcpy(urlPredicate,urldecode(bufferPredicateVar));
				memset(urlObject, '\0', 64);
				strcpy(urlObject,urldecode(bufferObjectVar));
				createGraph(readGraph(urlSpace, urlSubject, urlPredicate, urlObject), client);
				//FUNCIÓN QUE CREA EL GRAFO DEL STRING RECIBIDO EN READGRAPH()
			}

			break;
			memset(bufferSpace, '\0', 15);
			memset(bufferGraph, '\0', 15);
			memset(bufferWildcards, '\0', 15);
			memset(bufferSpaceVar, '\0', 15);
			memset(bufferGraphVar, '\0', 15);
			memset(bufferSubjectVar, '\0', 64);
			memset(bufferPredicateVar, '\0', 64);
			memset(bufferObjectVar, '\0', 64);
			identifyGet[0] = '0';
			identifyGet[1] = '0';
			identifyGet[2] = '0';
		}
		if (c == '\n') {
			// you're starting a new line
			currentLineIsBlank = true;
		} 
		else if (c != '\r') {
			// you've gotten a character on the current line
			currentLineIsBlank = false;
		}
		}
	}
	// give the web browser time to receive the data
	delay(1);
	Serial.println("closing the connection");
	// close the connection:
	client.stop();
	Serial.println("Connection closed");
	Serial.println("*****************");
	Serial.println("");
	}
}


// http://  to  http%3a%2f%2f
char* urlencode(const char* msg)
{
    const char *hex = "0123456789abcdef";
    String sEncodedMsg = "";

    while (*msg!='\0'){
        if( ('a' <= *msg && *msg <= 'z')
                || ('A' <= *msg && *msg <= 'Z')
                || ('0' <= *msg && *msg <= '9') ) {
            sEncodedMsg += *msg;
        } else {
            sEncodedMsg += '%';
            sEncodedMsg += hex[*msg >> 4];
            sEncodedMsg += hex[*msg & 15];
        }
        msg++;
    }
    
    char encodedMsg[80];
    memset(encodedMsg, '\0', 80);
    sEncodedMsg.toCharArray(encodedMsg,80);
    return encodedMsg;
}

// http%3a%2f%2f  to  http://
char* urldecode(char* url) {
	char dest[128];
	memset(dest, '\0', 128);
	char buffer[4];
    memset(buffer, '\0', 4);
	int i;
	int contSpecialChar = 0;

	for(i = 0; i < strlen(url); i++) {
		for(int j = 1; j < 3; j++){
			buffer[j-1] = buffer[j];
		}
		buffer[2] = url[i];
		dest[i-2*contSpecialChar] = url[i];
		if(strcmp(buffer, "%3A")==0  || strcmp(buffer, "%3a")==0){
			contSpecialChar++;
			dest[i-2*contSpecialChar] = ':';		
		}
		if(strcmp(buffer, "%2F")==0 || strcmp(buffer, "%2f")==0){
			contSpecialChar++;
			dest[i-2*contSpecialChar] = '/';			
		}
	}
	return dest;
}



int isUrl(char* term){
	if(term[0]=='h' & term[0]=='t' & term[0]=='t' & term[0]=='p' & term[0]==':' & term[0]=='/'){
		return 1;
	}
	else return 0;
}

void getMac(byte* macAddress){
	memset(macIdentifier, '\0', 15);
	String sMac = "";
	sMac = sMac + String(mac[0],HEX);
	sMac = sMac + String(mac[1],HEX);
	sMac = sMac + String(mac[2],HEX);
	sMac = sMac + String(mac[3],HEX);
	sMac = sMac + String(mac[4],HEX);
	sMac = sMac + String(mac[5],HEX);
	sMac.toCharArray(macIdentifier, 15);

}