//Tripleta RDF
struct Triple
{
  String s; //Subject: URI
  String p; //Predicate: URI
  String o; //Object: any type of data
};

//struct Triple ArrayTriples[3];

struct Graph{
  String URI; //URI in the graph 
  Triple* Triples; //Lista de Tripletas (puntero a un Triple)
};



//struct ArrayTriples ListaTripletas;
 
String URL_part1 = "http://otsopack/";
int URL_part2;

//Graph Grafos[2];
int contGrafos=0;

String sujeto = "s";
String predicado = "p";
String objeto = "o";
String sujeto_aux;
String predicado_aux;
String objeto_aux;



void writeGraph(struct Graph *tripletas){
//  Serial.println(contGrafos);
//  //Esta parte debería venir en el argumento
//  for (int n=0;n<1;n++){ //entra 3 veces
//    sujeto_aux = "";
//    sujeto_aux = sujeto+n;
//    predicado_aux = "";
//    predicado_aux = predicado+n;
//    objeto_aux = "";
//    objeto_aux = objeto+n;
//    
//  }
  //Se crea la URI para el nuevo conjunto de tripletas
  URL_part2 = random(0,1000);
  String new_uri = "";
  new_uri = URL_part1 + URL_part2;
  Serial.println(new_uri);
  //Se introduce la nueva URI que irá asociada al conjunto de trip
  tripletas->URI = new_uri;
  Serial.print("|");
  Serial.print(tripletas->URI);
  Serial.println("|");
//  //Se introduce en el grafo el conjunto de tripletas
//  for (int i=0; i<(sizeof(Grafos[contGrafos].Triples)/sizeof(ArrayTriples)); i++){    // entra 2 veces
//    Serial.print("  Conjunto de tripletas: ");
//    Serial.println(contGrafos);
//    for (int m=0; m<(sizeof(ListaTripletas)/sizeof(Triple)); m++){ //entra 3 veces
//      Grafos[contGrafos].Triples[i][m] = ListaTripletas[m];
//      Serial.print("  Sujeto: ");
//      Serial.println(Grafos[contGrafos].Triples[i][m].s);
//      Serial.print("  Predicado: ");
//      Serial.println(Grafos[contGrafos].Triples[i][m].p);
//      Serial.print("  Objeto: ");
//      Serial.println(Grafos[contGrafos].Triples[i][m].o);
//      Serial.println();
//    }
//  }
  contGrafos++;
}

//Lee todos los grafos
//void readGraph(){
//  for (int i=0; i<(sizeof(Grafos)/sizeof(Grafos)); i++){    
//    Serial.print("  URI: ");
//    Serial.println(Grafos[i].URI);
//    //Serial.println(sizeof(Grafos)/sizeof(Grafos));
//    for (int j=0; j<(sizeof(Grafos[contGrafos].Triples)/sizeof(ArrayTriples)); j++){
//      for (int k=0; k<(sizeof(ListaTripletas)/sizeof(Triple)); k++){ //entra 3 veces
//        Serial.print("    Sujeto: ");
//        Serial.println(Grafos[i].Triples[j][k].s);
//        Serial.print("    Predicado: ");
//        Serial.println(Grafos[i].Triples[j][k].p);
//        Serial.print("    Objeto: ");
//        Serial.println(Grafos[i].Triples[j][k].o);
//      }
//    }
//  }
//}
