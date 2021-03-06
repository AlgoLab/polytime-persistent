//controllare uso di var globale specieRealizzate
//passare alla soluzione successiva nel caso non fosse soluzione con primo albero

#include<stdlib.h>
#include<stdio.h>
#include <iostream>
#include <fstream>
#include <time.h>

using namespace std;

int** GRB;
int* soluzione; //sequenza di caratteri che rappresenta la soluzione
int* setSpecie;
int ** estesa;
int* specieRealizzate;
int righeO, colonneO;
int indice_path; //indice nell'arrai path per iserire nuovo nodo
int* percorso; //contiene percorso da sink a sorgente; verifico se ammissibile
int* sorgenti;
int* end_nodes;
int* sorgenti_potenziali; //usato per aggiornare diagramma Hasse nella ricerca di una specie ammissibile
int* sink;
int* in_deg;
int ** matrice;   //matrice di partenza, con colonne ordinate per numero decrescente di 1
int ** matriceO;
int** inclusioni;
int** hasse;
int numero_conflitti;
int gc_vuoto; //1: vuoto; 0: grafo con conflitti
int** archi_gabry;
int* comp_colonne; //componenti matrice di partenza
int* comp_righe;   //componenti matrice di partenza
char* mapping_chars;
FILE* file;
ofstream outsi;
ofstream outno;
clock_t start,end;
double tempo;
int componenti_matrice;
int contatore_componente;
int* massimali;
int** matrice_indotta;
int righe_indotta;
int colonne_indotta;
int** matrice_cc;
int righe_cc;
int colonne_cc;
int* specie_realizzate;
int* car_universali;
int* car_attivi;
int** matriceMC; //matrice di massimali compattata
int righeMC;
int colonneMC;
int* corrispondenza_cc; //tabella di corripondenza tra colonne di matrici indotte
int* corrispondenzaMC;

#define		NOTVISITED		0
#define		VISITED			1


class ConflictGraph {
public:

ConflictGraph (int** m, int righe, int colonne);

	bool is_a_graph_with_only_singletons ();
	bool is_a_simple_graph ();
	bool is_connected();
//	bool connesso(int componente);
	bool is_a_singleton (int car);
	int* compute_connected_component (int i);
	int print_connected_component (int* cc);
	int size_connected_component (int* cc);

	bool red_conflict (int** m, int righe, int colonne, int i, int j);	// method that checks if character i and character j are in red conflict
    bool specie_non_realizzata(int**m, int colonne, int k);
    bool carattere_connesso(int** m, int righe, int k);
	// Value extraction methods - inlined - they allow the value of _vertex to be private
	int get_vertex () const { return _vertex; };
	int get_species () const { return _species; };

	int insert_edge (int i, int j);

	int compute_components();
	int scorri_colonna(int colonna); //per calcolare le componenti del grafo

	int print_graph ();
	int reset_status ();

	// Destructor
	//~ConflictGraph ();


int** cgraph;			// another representation of the conflict graph in which the arcs are indexed
int edges;				// number of edges in the graph

	int* species_sequence;		// S_q, i.e., le specie in S* ordinate a seconda del numero di 1

	//char* mapping_chars;	// serve per fare in modo che i caratteri siano indicizzati con lettere dell'alfabeto e non con interi

private:

	struct node {
		int value; node* next;
		node(int x, node* t) {value = x; next = t; }
	};

	typedef node* link;
	link* adj;		// the array containing the adjunct lists
	int _vertex;	// number of vertexes in the graph, it corresponds to the number of characters
	int _species;
	int* mapping;
	int* status;			// an array that records the status of the visits to ???? nodes or verteces???che contiene lo status delle visite dei vari nodi
};

// Constructor
// Construct the conflict graph from matrix m, which could have some rows removed
ConflictGraph::ConflictGraph (int** m, int righe, int colonne) {
	int i, j,k,x,y;
	int somma = 0;			// serve per creare l'array che contiene il numero di caratteri attivi per ogni specie (somma_uni)
	int removed_species = 0;
	link t;
   _vertex = colonne;
	_species = righe;

	cgraph = new int* [get_vertex()];
	for (i = 0; i < get_vertex(); i++){
		cgraph[i] = new int[get_vertex()];
	}
	// initialize the graph with no arcs => all 0s
	for (i = 0; i < get_vertex(); i++) {
		for (j = 0; j < get_vertex(); j++) {
			cgraph[i][j] = 0;
		}
	}
	edges = 0;			// initially there are no edges in the graph

	// create the array containing the adjunct lists representing the conflict graph
	// the lists are initially empty
	adj = new link [get_vertex()];
	for (i = 0; i < get_vertex(); i++) {
		adj[i] = NULL;
	}
	//cout<<"Costruisco grafo dei conflitti"<<endl;

	for (i = 0; i < get_vertex(); i++) {
            if(carattere_connesso(m, righe, i)){
		for (j = i + 1; j < get_vertex(); j++) {
                if(carattere_connesso(m, righe, j)){
            //cout<<i<<j<<endl;
            if (red_conflict(m, righe, colonne, i, j) == true) {
                               // cout<<"conflitto;"<<endl;
            	insert_edge(i, j);						// add the arc in the adjunct list
				cgraph[i][j] = cgraph[j][i] = ++edges;	// add the arc in the adjunct matrix
			}
                }
		}
		}
	}

//	cout<<"numero di archi: "<<edges<<endl;
 numero_conflitti=edges;
 if (numero_conflitti>0) gc_vuoto=0;
 else gc_vuoto=1;

	// set all the vertexes as NOTVISITED
	// serve per poi usare la funzione di visita/lettura del grafo
	status = new int [_vertex];
	for (i = 0; i < _vertex; i++) {
		status[i] = NOTVISITED;
	}

	mapping = new int [get_vertex()];
	for (i = 0; i < get_vertex(); i++) {
		mapping[i] = i;
	}

	archi_gabry=new int*[edges];
	for(i=0; i<edges; i++) archi_gabry[i]=new int[2];

	for(i=0; i<edges;i++){
             for(j=0; j<2; j++)
              archi_gabry[i][j]=-1;
             }
      x=0;

 for (i = 0; i < get_vertex(); i++) {
		t = adj[i];
	//	if (t == NULL) cout << mapping_chars[i] << endl;	// it prints also singletons (i.e., unconnected vertexes)
		while (t != NULL) {
			if (i < t->value) {
                  archi_gabry[x][0]= i;
                  archi_gabry[x][1]=t->value;
                  x=x+1;
                  }
			t = t->next;
		}
	}
 /*  cout<<"archi gabry:"<<endl;
    for(i=0; i<edges;i++){
             for(j=0; j<2; j++)
              cout<<archi_gabry[i][j];
              cout<<endl;
             }*/

}

// A method to insert a new arc in the graph between the vertex of character i and the vertex of character j
int ConflictGraph::insert_edge(int i, int j) {

	//cout<<"contatore archi: "<<contatore_archi<<endl;
	if (i < 0 || i > get_vertex()) {
		cout << "ConflictGraph::insert_edge(): Index of character out of range!" << endl;
		exit(-1);
	}

	if (j < 0 || j > get_vertex()) {
		cout << "ConflictGraph::insert_edge(): Index of character out of range!" << endl;
		exit(-1);
	}

	   // since the graph is undirected, it adds a node in the adjunct list of both i and j
	   adj[i] = new node(j, adj[i]);
	   adj[j] = new node(i, adj[j]);

	return 0;
};

// method that checks if character i and character j are in red conflict
// NB: i and j are the indexes of the matrix
bool ConflictGraph::red_conflict(int** m, int righe, int colonne, int i, int j) {
	int k;

	int flag1, flag2, flag3, flag4;
	flag1 = flag2 = flag3 = flag4 =0;

	//cout<<"cerco conflitto tra caratteri "<<i<<" "<<j<<endl;

// controllo che i due caratteri appartengono alla stessa componente e che le specie considerate non siano ancora state realizzate
	if (comp_colonne[i]==comp_colonne[j]){
      // cout<<"Stessa componente"<<endl;
	   for (k = 0; k < righe; k++) {
            if(specie_non_realizzata(m,colonne,k)){

            if ((matrice[k][i] == 0 ) & (matrice[k][j] == 0) & (comp_righe[k]==comp_colonne[i])) flag1 = 1;
    		if ((matrice[k][i] == 0) & (matrice[k][j] == 1) & (comp_righe[k]==comp_colonne[i])) flag2 = 1;
    		if (((matrice[k][i] == 1)) & (matrice[k][j] == 0) & (comp_righe[k]==comp_colonne[i])) flag3 = 1;
    		if ((matrice[k][i] == 1) & (matrice[k][j] == 1) & (comp_righe[k]==comp_colonne[i])) flag4 = 1;
    		}
		}
	}
    if (flag1 & flag2 & flag3 & flag4) return true; //sigma nero
	return false;
};

bool ConflictGraph::specie_non_realizzata(int** m, int colonne, int k){
    int i;
    for(i=0; i<colonne; i++){
        if(m[k][i]!=0) return true;
    }
    return false;
}

//il carattere k non � singoletto
bool ConflictGraph::carattere_connesso(int** m, int righe, int k){
    int i;
    for(i=0; i<righe; i++){
        if(m[i][k]!=0) return true;
    }
    return false;
}

int ConflictGraph::print_graph() {
	int i, j;
	link t;

	// print the first representation of the graph
	// the arcs and the singletons
	for (i = 0; i < get_vertex(); i++) {
		t = adj[i];
		if (t == NULL) cout << mapping_chars[i] << endl;	// it prints also singletons (i.e., unconnected vertexes)
		while (t != NULL) {
			if (i < t->value) cout << mapping_chars[i] << " - " << mapping_chars[t->value] << endl;
			t = t->next;
		}
	}


	return 0;
};

bool ConflictGraph::is_a_simple_graph () {
	int* cc;
	int i, size;
	int singleton_counter = 0;

	// it counts how many singletons are in the graph
	for (i = 0; i < get_vertex(); i++) {
		if (adj[i] == NULL) singleton_counter++;
	}

	// it searches for a non-trivial connected component (with size > 1)
	// if it finds such a component it exits the loop
	for (i = 0; i < get_vertex(); i++) {
		cc = compute_connected_component(i);
		size = size_connected_component(cc);
		//cout << "Dimensione della componente non banale: " << size << endl;
		if (size > 1) break;
	}

	if (size > 1) {
		if (size == (get_vertex() - singleton_counter)) {	// there is only one non-trivial component
			return true;
		} else {											// there is more than one non-trivial component
			return false;
		}
	} else {												// there are only singletons
		return false;
	}
}

bool ConflictGraph::is_connected () {
	int* cc;
	int i, size;
/*	int singleton_counter = 0;

	// it counts how many singletons are in the graph
	for (i = 0; i < get_vertex(); i++) {
		if (adj[i] == NULL) singleton_counter++;
	}*/

	// it searches for a non-trivial connected component (with size > 1)
	// if it finds such a component it exits the loop
	for (i = 0; i < get_vertex(); i++) {
		cc = compute_connected_component(i);
		size = size_connected_component(cc);
		//cout << "Dimensione della componente non banale: " << size << endl;
		if (size > 1) break;
	}

	if (size > 1) {
		if (size == (get_vertex())) {	// there is only one non-trivial component
			return true;
		} else {											// there is more than one non-trivial component
			return false;
		}
	} else {												// there are only singletons
		return false;
	}
}

bool ConflictGraph::is_a_graph_with_only_singletons () {
	int singleton_counter = 0;
	int i;
	//cout<<"qui";
	for (i = 0; i < get_vertex(); i++) {
		if (adj[i] == NULL) singleton_counter++;
	}
	if (singleton_counter == get_vertex()) return true;		// if all the vertexes are singletons (not connected), return true

	return false;
}

bool ConflictGraph::is_a_singleton (int car) {
	if (adj[car] == NULL) return true;
	return false;
}

int ConflictGraph::size_connected_component(int* cc) {
	int counter = 0;

	for (int i = 0; (i < get_vertex() && cc[i] != -1); i++) {
		counter++;
	}

	return counter;
};

int* ConflictGraph::compute_connected_component(int i) {
	int stack[get_vertex()+1]; // da verificare se e' giusto il + 1 che ho aggiunto
	int top = 1;
	//int index_character;
	link t;
	int* connected_vertex = new int [get_vertex()];	// DA VERIFICARE SE LA DIMENSIONE DEVE ESSERE AUMENTATA DI UNO
													// NELLA PEGGIORE DELLE IPOTESI HO CHE TUTTI I CARATTERI APPARTENGONO ALLA CC
													// E ALLORA NON HO PIU' SPAZIO PER METTERCI IL -1
	int j = 0;

	//cout << "Valore di i: " << i << endl;
	//cout << "valore di get_vertex: " << get_vertex()<< endl;
	if (i < 0 || i >= get_vertex()) {
		cout << "GraphRB::compute_connected_component(): Index out of range!" << endl;
		exit(-1);
	}
	//index_character = get_species() + i; // compute the index of the character in terms of the array adj

	stack[0] = -1;	// set the exit condition

	reset_status();

	stack[top] = i;
	status[i] = VISITED;

	while (1) {
		// exit condition
		if (stack[top] == -1)  {
			break;
		}

		connected_vertex[j] = stack[top];
		j++;

		t = adj[stack[top]];
		top--;

		// put in the stack all adjunct vertexes that has not been visited yet
		while (t != NULL) {
			if (status[t->value] == NOTVISITED ) {
				top++;
				stack[top] = t->value;
				status[t->value] = VISITED;
			} // end IF

			t = t->next;
		} // end WHILE
	} // end WHILE (1)

	connected_vertex[j] = -1;	// set the end of the connected species component

	return connected_vertex;
};

int ConflictGraph::reset_status() {
	int i;
	for (i = 0; i < _vertex; i++) {
		status[i] = NOTVISITED;
	}

	return 0;
};

 /////////////////////////////////////////////////////////////////////////////////////////////////////////////
void readMatrix(int** matrice, int righe, int colonne);
void printMatrix(int ** a, int m, int n);
int calcola_componenti(int** matrice, int righe, int colonne);
int componenti_colonna(int car, int** matrice, int ri, int co);
int componenti_riga(int r, int c_iniziale, int** matrice, int ri, int co);
void calcola_massimali(int colonne, int* caratteri_universali, int** matrice, int righe, int* specie_realizzate);
int incluso(int c, int* caratteri_universali, int righe, int* specie_realizzate, int** matrice, int colonne);
int ultimo_carattere(int c, int colonne, int* caratteri_universali);
int no_massimali(int* massimali, int colonne);
void indotta_cc(int componente);
void indotta_massimali(int**matrice, int righe, int colonne, int* massimali);
int sIncluded(int** matrice, int colonne, int s1, int s2);
int conta_uni(int** m, int riga, int colonne);
int trova_sorgenti(int colonna, int righe);
int trova_sink(int riga, int righe);
int inDeg(int colonna, int righe);
int outDeg(int riga, int righe);
void percorso_semplice(int righe);
void aggiungi_nodo(int s, int righe);
void trova_successivo(int nodo, int righe);
int cerca_ammissibile(int righe);
void realizza_specie(int** matrice, int righe, int colonne, int specie, int* specie_realizzate, int* caratteri_universali, int* caratteri_attivi, int** Grb, int* soluzione_massimali);
void realizza_specie(int** Grb, int righe, int colonne, int specie, int* specie_realizzate, int* caratteri_universali, int* caratteri_attivi);
void rendi_universale(int** matrice, int righe, int colonne, int i,int** Grb,  int* caratteri_universali, int* caratteri_attivi, int* specie_realizzate, int* soluzione_massimali);  //i: carattere da rendere universale
void rendi_universale(int** Grb, int righe, int colonne, int i, int* caratteri_universali, int* caratteri_attivi, int* specie_realizzate);  //i: carattere da rendere universale
int valuta_colonna(int** Grb, int righe, int colonne, int carattere, int* tspecie, int* tcarattere);
int valuta_riga(int** Grb, int righe, int colonne, int specie, int* tspecie, int* tcarattere);
void aggiorna_caratteri_attivi(int** Grb, int righe, int colonne, int* caratteri_attivi, int* specie_realizzate);
void aggiorna_specie_realizzate(int** Grb, int righe, int colonne, int* specie_realizzate);
int valuta_colonna(int** matrice, int righe, int colonne, int carattere, int** Grb, int* tspecie, int* tcarattere);
int valuta_riga(int** matrice, int righe, int colonne, int specie, int** Grb, int* tspecie, int* tcarattere);
void aggiorna_caratteri_attivi(int** matrice, int righe, int colonne, int* caratteri_attivi, int** Grb, int* specie_realizzate);
void aggiorna_specie_realizzate(int** matrice, int righe, int colonne, int* specie_realizzate, int** Grb);
//void aggiorna_gc(int** matrice, int righe, int colonne);
bool specie_realizzata(int** matrice,int righe,int colonne,int specie, int* caratteri_universali);
void aggiorna_sorgenti_potenziali(int specie);
int included(int** matrice, int righe, int colonne, int s1, int s2);
void percorso_cicli(int righe);
int verificaEndNode(int n);
void aggiorna_Hasse(int specie);
int determinaEndNode(int cSink);
int inserisciNodo(int nodoCorrente, int nodoDestinazione);
void percorsiSinkEN();
void percorsiENSorgenti();
void copiaSinkEn(int si, int en);
void copiaEnSorgente(int en, int so);
int realizza_percorso();
void estendi(int carattere, int** Grb);
int* riordina_percorso(int* percorso, int righe);
void zigzagPath(int righe);
void percorriZigzag();
void alberoMassimali();
void calcolaSoluzione(int** matrice, int righe, int colonne);
void completaSoluzione(int s);
void trovaMinimali();
int inserito(int c);
int conflitto(int c);
void compattaIndottaMassimali();
int specieDiversa(int s);
int specieUguali(int s1, int s2);
void calcolaZ();
int appartiene(int carattere, int specie);
void aggiornaSoluzione(int minimale);
void trovaSorgenti(int** matrice, int** hasse, int righe, int colonne);
int* costruiscoPercorso(int** matrice, int** hasse, int righe, int colonne, int i, int j);
int trovaSuccessivo(int** matrice, int** hasse, int righe, int colonne, int nodo, int sink);
void riduciMatrice(int** matrice, int righe, int colonne);
void trovaPercorsi(int** matrice, int** hasse, int righe, int colonne);
int determinaIndice(int* soluzione);
void copiaSoluzione(int* soluzione,int indice,int specie, int** matrice, int colonne);
void aggiornaGRB(int** GRB,int righeO,int colonneO,int k,int* specie_realizzate,int* car_universali, int* car_attivi);
int inclusioneCaratteri(int c1, int c2, int** matrice, int righe);
bool colonnaSingoletto(int** matrice,int righe,int colonna);
bool rigaSingoletto(int** matrice,int colonne,int riga);
bool soloSingoletti(int* componenti, int colonne);
int sottomatriceProibita(int ** a, int c1, int c2);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(int argc, char *argv[]) {
int i,j,k,ii,jj;
int itera;
char a = 'a';
char* fileName;
int n_componenti, cont_neri, cont_specie;
int n_uni;
//flag_ammissibile=1;
//flag_hasse_vuoto=0;

  mapping_chars = new char [29];
	for (i = 0; i < 29; i++) {
		mapping_chars[i] = a;
		a++;
	}

  // check command line (main) arguments
  if (argc != 2) {
    printf("Usage: check infile\n");
    return 0;
  } else {
    fileName = argv[1];
    // open file and check if empty
    if ((file = fopen(fileName, "r")) == NULL) {
      printf("File %s could not be opened.\n", fileName);
      return 0;
    }
    if (feof(file)) {
      printf("File %s is empty.", fileName);
      return 0;
    }
  }

  outsi.open("polinomiale_siPPPH.txt", ios::out);
  outno.open("polinomiale_noPPPH.txt", ios::out);

 fscanf(file, "%d %d", &righeO, &colonneO);

 start=clock();
  while(!feof(file)){
     // flag_soluzione=1;
     // flag_soluzione_trovata=0;
     // soluzione_trovata=0;
      matriceO = (int **)calloc(righeO, sizeof(int *));
       for (i = 0; i < righeO; i++){
           matriceO[i] = (int *)calloc(colonneO, sizeof(int));
       }
      readMatrix(matriceO,righeO,colonneO);
      printf("\nMatrice nera:---------------------------------------------\n");
      printMatrix(matriceO,righeO,colonneO);


      soluzione=new int[colonneO];
    for(i=0; i<colonneO; i++) soluzione[i]=-1;
    GRB = (int **)calloc(righeO, sizeof(int *));
       for (i = 0; i < righeO; i++){
           GRB[i] = (int *)calloc(colonneO, sizeof(int));
       }
       for (i = 0; i < righeO; i++){
           for (j = 0; j < colonneO; j++)
           GRB[i][j]=matriceO[i][j];
       };

       printMatrix(GRB, righeO, colonneO);

    car_universali=new int[colonneO];
    for(i=0; i<colonneO; i++) car_universali[i]=0;
    car_attivi=new int[colonneO];
    for(i=0; i<colonneO; i++) car_attivi[i]=0;
    specie_realizzate=new int[righeO];
    for(i=0; i<righeO; i++) specie_realizzate[i]=0;

    estesa = (int **)calloc(righeO, sizeof(int *));
    for (i = 0; i < righeO; i++) {
      estesa[i] = (int *)calloc(2*colonneO, sizeof(int));
    }
    for (i = 0; i < righeO; i++) {
           for (j = 0; j < 2*colonneO; j++) {
               estesa[i][j]=-1;
    }
    }

      //calcolo le componenti e richiamo algo di riduzione su ogni componente
      riduciMatrice(GRB,righeO,colonneO);
      //verifica finale sulla matrice estesa
      for (ii = 0; ii < 2*colonneO-1; ii++) {
                    for (jj = ii+1; jj < 2*colonneO; jj++) {
                          if (sottomatriceProibita(estesa,ii,jj) == 1) { //esiste sottomatrice proibita
                              cout<<"sottomatrice proibita tra colonne "<<ii <<" e "<< jj<<endl;
                              cout<<"La matrice non ammette soluzione"<<endl;


            }
        }
     }

 }

end=clock();
tempo=((double)(end-start))/CLOCKS_PER_SEC;

cout<<"tempo di esecuzione: "<<tempo;

  return 0;
}

//data una matrice di una sola componente connessa, calcola tutti i possibili percorsi safe da radice a sink
void calcolaSoluzione(int**matrice, int righe, int colonne){
    int i,j,k;
int itera;
char a = 'a';
char* fileName;
int n_componenti, cont_neri, cont_specie;

cout<<"GRB in calcolaSoluzione"<<endl;
printMatrix(GRB, righeO, colonneO);

//nsoluzione=0;
//non solo massimali, ma generale

/*soluzioneMassimali=new int*[10];  //DA RENDERE PARAMETRICO IN BASE AL NUMERO DI SOLUZIONI DI MASSIMALI
for (i = 0; i < 10; i++){
		soluzioneMassimali[i] = new int[colonne];
	}
for (i = 0; i < 10; i++) {
		for (j = 0; j < colonne; j++) {
			soluzioneMassimali[i][j] = -1;
		}
	}*/

//int* specie_realizzate;
int n_uni;
//    CarUniversali=new int[colonne];
//for(i=0; i<colonne; i++) CarUniversali[i]=0;




//S_Realizzate=new int[righe];
//for(i=0; i<righe; i++) S_Realizzate[i]=0;

 inclusioni = new int* [righe];
	for (i = 0; i < righe; i++){
		inclusioni[i] = new int[righe];
	}
	// initialize the graph with no arcs => all 0s
	for (i = 0; i < righe; i++) {
		for (j = 0; j < righe; j++) {
			inclusioni[i][j] = 0;
		}
	}

 hasse = new int* [righe];
	for (i = 0; i < righe; i++){
		hasse[i] = new int[righe];
	}
	// initialize the graph with no arcs => all 0s
	for (i = 0; i < righe; i++) {
		for (j = 0; j < righe; j++) {
			hasse[i][j] = 0;
		}
	}

//1
 //1a  calcolo diagramma di Hasse. date n specie, costruisco matrice inclusioni nxn dove cella = 1 se esiste arco che collega specie-riga e specie-colonna
 //� la matrice delle inclusioni, in cui non esiste propriet� transitiva (eliminata al passo successivo)
 for(i=0; i<righe; i++){
    for(j=0; j<righe; j++){
        if(i!=j){
            inclusioni[i][j]=sIncluded(matrice, colonne, i, j);
        }
    }
 }
/* cout<<"Inclusioni:"<<endl;
 for(i=0; i<righe; i++){
    for(j=0; j<righe; j++){
        cout<<inclusioni[i][j];
    }
    cout<<endl;
 }*/

 for (i = 0; i < righe; i++) {
		for (j = 0; j < righe; j++) {
			hasse[i][j] = inclusioni[i][j];
		}
	}

 //1b  transitivit�: date s1 s2 s3, se s1<s2<s3 devo avere archi s1-s2 ed s2-s3 ma non s1-s3
 //per fare questo, devo tenere conto del numero di 1 delle specie. Tengo archi tra specie di livelli consecutivi
 //per ogni riga di inclusioni, se esiste pi� di un 1, controllo che le due specie che includono non abbiano inclusione tra loro
 // se disgiunte, tengo entrambi gli 1
 // se inclusione, tengo quella con minor numero di 1 e metto a 0 l'altra
 for(i=0; i<righe; i++){
    n_uni=conta_uni(inclusioni, i, righe);
    if(n_uni<2){
      //  cout<<"riga "<<i<<" con numero di 1 minore di 2"<<endl;
        for(j=0; j<righe; j++) hasse[i][j]=inclusioni[i][j];
    }
    else{
       // cout<<"verifica archi per specie "<<i<<endl;
        for(j=0; j<righe-1; j++){
            if(inclusioni[i][j]==1){
                for(k=j+1; k<righe; k++){
                    if(inclusioni[i][k]==1){
                           if((inclusioni[j][k]==1)|(inclusioni[k][j]==1)){
                            //cout<<"specie "<<j<<" "<<k<<" in inclusione tra loro"<<endl;
                            if(inclusioni[j][k]==1) hasse[i][k]=0;
                            else hasse[i][j]=0;
                        }
                    }
            }
        }
    }
    }
 }
 cout<<"Hasse:"<<endl;
for(i=0; i<righe; i++){
    for(j=0; j<righe; j++){
        cout<<hasse[i][j];
    }
    cout<<endl;
 }

//trovaSorgenti(matrice, hasse, righe, colonne);
trovaPercorsi(matrice, hasse, righe, colonne);


}


void readMatrix(int** matrice, int righe, int colonne) {
  int i,j;

       for (i = 0; i < righe; i++) {
           for (j = 0; j < colonne; j++) {
              fscanf(file, "%d", &(matrice[i][j]));
               }
       }

}

// print a matrix to the standard output
void printMatrix(int ** a, int m, int n) {
  int i, j;
  for (i = 0; i < m; i++) {
    for (j = 0; j < n; j++)
      printf("%d ", a[i][j]);
    putchar('\n');
  }
  putchar('\n');
}

int calcola_componenti(int** matrice, int righe, int colonne){

    int i,j, start;
    int contatore_componente=0;
    int* singolettiRiga;
    int* singolettiColonna;
//cout<<"calcola componenti--------------------"<<endl;
  /*  for(i=0; i<righe; i++){
             for(j=0; j<colonne; j++){
                      cout<<matrice[i][j];
                      }
             cout<<endl;
             }*/

    comp_colonne = new int [colonne];
          for(i=0; i<colonne; i++)
                  comp_colonne[i]=-1;
    comp_righe = new int [righe];
          for(i=0; i<righe; i++)
                  comp_righe[i]=-1;

    singolettiColonna = new int [colonne];
          for(i=0; i<colonne; i++)
                  singolettiColonna[i]=0;
    singolettiRiga = new int [righe];
          for(i=0; i<righe; i++)
                  singolettiRiga[i]=0;

    for(i=0; i<colonne; i++){
        if(colonnaSingoletto(matrice, righe, i)) {
                singolettiColonna[i]=1;
                comp_colonne[i]=-2;  //indico che � un singoletto
        }
    }

    for(i=0; i<righe; i++){
        if(rigaSingoletto(matrice, colonne, i)) {
                singolettiRiga[i]=1;
                comp_righe[i]=-2;
        }
    }
for(i=0; i<colonne; i++) cout<<singolettiColonna[i];
cout<<endl;
for(i=0; i<righe; i++) cout<<singolettiRiga[i];
cout<<endl;

    //verifico se sono tutti singoletti
    if(soloSingoletti(comp_colonne, colonne)) return -2;

    //determino prima colonna che non � un singoletto e da l� parto a calcolare le componenti
    for(i=0; i<colonne; i++){
        if(comp_colonne[i]!=-2) {
            start=i;
            break;
        }
    }

    comp_colonne[start]=0;
    i=start;
    //calcola componente connessa al primo carattere
    componenti_colonna(i, matrice, righe, colonne);

    // controllo se tutti i caratteri appartengono gi� ad una componente
    for (i=0; i<colonne; i++){
        if (comp_colonne[i] == -1) { // questo carattere non appartiene a nessuna componente
        comp_colonne[i]= contatore_componente+1;
        contatore_componente++;
        //calcolo componente connessa a questo carattere
        componenti_colonna(i, matrice, righe, colonne);
                          }
    }

    //riscrivo gli 1 e 2 nella matrice iniziale
    for (i=0; i<righe; i++){
        for (j=0; j<colonne; j++){
            if (matrice[i][j]==3) matrice[i][j]=1;
            if (matrice[i][j]==4) matrice[i][j]=2;
    }
    }

    componenti_matrice=contatore_componente;
   // if(contatore_componente>0) realizza_indotte(matrice, righe, colonne, contatore_componente);
  /* for(i=0; i<colonne; i++) cout<<comp_colonne[i]<<" ";
   cout<<endl;
   for(i=0; i<righe; i++) cout<<comp_righe[i]<<" ";
   cout<<endl;*/

    return componenti_matrice;
}

bool soloSingoletti(int* componenti, int colonne){
    int i;
    for(i=0; i<colonne; i++){
        if(componenti[i]!=-2) return false;
    }
    return true;
}

bool colonnaSingoletto(int** matrice,int righe,int colonna){
    int i;
    for(i=0; i<righe; i++)  {
            if (matrice[i][colonna]!=0) return 0;
    }
    return 1;
}

bool rigaSingoletto(int** matrice,int colonne,int riga){
    int i;
    for(i=0; i<colonne; i++)  {
            if (matrice[riga][i]!=0) return 0;
    }
    return 1;
}

//dato un carattere, controlla dove � a 1 nella colonna e richiama la fx che scorre la riga corrispondente
int componenti_colonna(int car, int** matrice, int ri, int co){
    int i;
   // cout << "cerco riga a 1 per carattere "  << car << "\n";
    for (i=0;i<ri; i++){
        if (matrice[i][car]==1) {
                             comp_righe[i]=comp_colonne[car];
                             matrice[i][car]=3; //altrimenti va in loop. alla fine riscrivo gli 1
                            // cout << "riga " << i << "componente "<<comp_righe[i]<<"\n";
                        componenti_riga(i,car, matrice, ri, co);
                        }
        else if (matrice[i][car]==2) {
                             comp_righe[i]=comp_colonne[car];
                             matrice[i][car]=4; //altrimenti va in loop. alla fine riscrivo gli 1
                           //  cout << "riga " << i << "\n";
                        componenti_riga(i,car, matrice, ri, co);
                        }
    }
    return 0;
}

int componenti_riga(int r, int c_iniziale, int** matrice, int ri, int co){
    int i;
   // cout  <<"cerco carattere a 1 in riga " << r << "\n";
    for (i=0;i<co; i++){
        if (matrice[r][i]==1){
                             matrice[r][i]=3; //altrimenti va in loop. alla fine riscrivo gli 1
                            // cout << "carattere " << i<<" ";
                             comp_colonne[i]=comp_colonne[c_iniziale];
                            // cout<<" componente "<<comp_colonne[i]<<endl;
                             componenti_colonna(i, matrice, ri, co);  //ricorsione??
                             }
        if (matrice[r][i]==2){
                             matrice[r][i]=4; //altrimenti va in loop. alla fine riscrivo gli 1
                             //cout << "carattere " << i << "\n";
                             comp_colonne[i]=comp_colonne[c_iniziale];
                             componenti_colonna(i, matrice, ri, co);
                             }
    }
    return 0;
}

void calcola_massimali(int colonne, int* caratteri_universali, int** matrice, int righe, int* specie_realizzate){
     int i, j, cont;
     int* universali;

     massimali = (int *) calloc(colonne, sizeof(int));
     for (i = 0; i < colonne; i++)
         massimali[i] = 0;

     for(i=0; i<colonne; i++){
            if(caratteri_universali[i]==0){ //il carattere deve ancora essere realizzato
             // cout<<"verifico se carattere "<<i<<" massimale_______________________________________________"<<endl;

                              // cout<<i<<" deve ancora essere realizzato"<<endl;
                               //il carattere non deve essere incluso in uno degli altri caratteri non ancora resi universali
                               //oppure deve essere l'ultimo da rendere universale
                               if((!(incluso(i, caratteri_universali, righe, specie_realizzate, matrice, colonne))) || (ultimo_carattere(i, colonne, caratteri_universali))){
                                    // cout<<i<<" massimale"<<endl;
                                     massimali[i]=1;
                               }
              }
           //  else cout<<"carattere gi� realizzato"<<endl;
     }
     if(no_massimali(massimali, colonne)){ //non ho trovato massimali con il blocco precedente ma ci sono ancora caratteri da realizzare
       // i rimanenti caratteri sono tutti massimali - dovrebbero essere uguali tra loro
            for(i=0; i<colonne; i++){
            if(caratteri_universali[i]==0) massimali[i]=1;
            }
     }
     cout<<"Insieme di massimali: ";
     for(i=0; i<colonne; i++) {
              cout<<massimali[i];

     }
     cout<<endl;
}

//stabilisco se il carattere c � incluso in un altro carattere
int incluso(int c, int* caratteri_universali, int righe, int* specie_realizzate, int** matrice, int colonne){
    int i,j;
    int incluso; //se 0: non incluso; se >0, incluso

    incluso=0; //ipotizzo inizialmente non sia incluso
    // cout<<"verifico se carattere "<<c<<" incluso in altri caratteri"<<endl;
    for(i=0; i<colonne; i++){
           // if((i!=c)&(caratteri_universali[i]==0)){
            if((i!=c)){
                    if(caratteri_universali[i]==0){
                      incluso=incluso + inclusioneCaratteri(c,i, matrice, righe);
                    }
            }
    }
    cout<<"numero inclusioni di carattere "<<c<<": "<<incluso<<endl;
if (incluso==0) return 0;
else return 1;
}

//verifico se carattere c1 incluso in carattere c2
int inclusioneCaratteri(int c1, int c2, int** matrice, int righe){
    int i;

    for(i=0; i<righe; i++){
        if((matrice[i][c1]==1) & (matrice[i][c2]==0)) {
               cout<<c1<<" NON incluso in "<<c2<<endl;
                return 0;
        }
    }
cout<<c1<<" incluso in "<<c2<<endl;
return 1;

}


int ultimo_carattere(int c, int colonne, int* caratteri_universali){
    int i,j;

    for(i=0; i<colonne; i++){
             if(i!=c){
                if (caratteri_universali[i]==0)
                                             return 0;
             }
    }
   // cout<<"ultimo carattere da rendere universale"<<endl;
    return 1;
}

int no_massimali(int* massimali, int colonne){
    int i;
    for(i=0; i<colonne; i++){
        if(massimali[i]==1) return 0; //ho trovato un massimale
    }
    return 1;
}

void indotta_cc(int componente){
    int* caratteri;
    int* specie;
    int i,j;
    int righe_indotta=0;
    int colonne_indotta=0;
    int i_cc, j_cc;

    righe_cc=0;
    colonne_cc=0;

    caratteri = (int *) calloc(colonneO, sizeof(int));
    for (i = 0; i < colonneO; i++) caratteri[i] = 0;

    specie = (int *) calloc(righeO, sizeof(int));
    for (i = 0; i < righeO; i++) specie[i] = 0;

cout<<"componenti colonne:";
for(i=0; i<colonneO; i++) cout<<comp_colonne[i];
cout<<endl;
cout<<"componenti righe:";
for(i=0; i<righeO; i++) cout<<comp_righe[i];
cout<<endl;

    //caratteri della componente
    for(i=0; i<colonneO; i++){
        if(comp_colonne[i]==componente) caratteri[i]=1;
    }

    //specie della componente
   for(i=0; i<righeO; i++){
        if(comp_righe[i]==componente) specie[i]=1;
    }

    //numero righe indotta
    for(i=0; i<righeO; i++){
        if(specie[i]==1) righe_cc++;
    }
    //numero colonne indotta
    for(i=0; i<colonneO; i++){
        if(caratteri[i]==1) colonne_cc++;
    }
cout<<righe_cc<<colonne_cc<<endl;

corrispondenza_cc=new int [colonne_cc];
    //riempio matrice indotta
    matrice_cc=new int*[righe_cc];
    for(i=0; i<righe_cc; i++){
             matrice_cc[i]=new int [colonne_cc];
             }
    i_cc=0;
    j_cc=0;
    for(i=0;i<righeO; i++){
        if(specie[i]==1){
            for(j=0; j<colonneO; j++){
                if(caratteri[j]==1){
                    matrice_cc[i_cc][j_cc]=GRB[i][j];
                    corrispondenza_cc[j_cc]=j; //associo alla colonna indotta l'indice della colonna originale per tenere traccia dell'etichetta
                    //aggiorno indici matrice indotta
                    if(j_cc!=colonne_cc) j_cc++;
                    if((j_cc==colonne_cc)&(i_cc<righe_cc)){
                        i_cc++;
                        j_cc=0;
                    }
                }
            }
        }
    }
    cout<<"Matrice indotta componente "<<componente<<endl;
    for(i=0; i<righe_cc; i++){
        for(j=0; j<colonne_cc; j++){
            cout<<matrice_cc[i][j];
        }
        cout<<endl;
    }
}

void indotta_massimali(int**matrice, int righe, int colonne, int* massimali){
    int* caratteri;
    int* specie;
    int i,j;
    int i_indotta, j_indotta;

    righe_indotta=0;
    colonne_indotta=0;

    caratteri = (int *) calloc(colonne, sizeof(int));
    for (i = 0; i < colonne; i++) caratteri[i] = 0;

    specie = (int *) calloc(righe, sizeof(int));
    for (i = 0; i < righe; i++) specie[i] = 0;

    //caratteri della matrice indotta
    for(i=0; i<colonne; i++){
        if(massimali[i]==1) caratteri[i]=1;
    }
    //specie della matrice indotta
    for(i=0; i<colonne; i++){
            if(caratteri[i]==1){
              for(j=0; j<righe; j++){
                if(matrice[j][i]==1) specie[j]=1;
              }
             }
    }

    //numero righe indotta
    for(i=0; i<righe; i++){
        if(specie[i]==1) righe_indotta++;
    }
    //numero colonne indotta
    for(i=0; i<colonne; i++){
        if(caratteri[i]==1) colonne_indotta++;
    }

    corrispondenzaMC=new int [colonne_indotta];

    //riempio matrice indotta
    matrice_indotta=new int*[righe_indotta];
    for(i=0; i<righe_indotta; i++){
             matrice_indotta[i]=new int [colonne_indotta];
             }
    i_indotta=0;
    j_indotta=0;
    for(i=0;i<righe; i++){
        if(specie[i]==1){
            for(j=0; j<colonne; j++){
                if(caratteri[j]==1){
                    matrice_indotta[i_indotta][j_indotta]=matrice[i][j];
                    corrispondenzaMC[j_indotta]=corrispondenza_cc[j];
                    //aggiorno indici matrice indotta
                    if(j_indotta!=colonne_indotta) j_indotta++;
                    if((j_indotta==colonne_indotta)&(i_indotta<righe_indotta)){
                        i_indotta++;
                        j_indotta=0;
                    }
                }
            }
        }
    }
cout<<"matrice indotta da massimali:"<<endl;
    for(i=0; i<righe_indotta; i++){
        for(j=0; j<colonne_indotta; j++){
            cout<<matrice_indotta[i][j];
        }
        cout<<endl;
    }
}

//verifico se s1 incluso in s2 (s1<s2)
int sIncluded(int** matrice, int colonne, int s1, int s2) {
     int i, k;
    // cout<<"calcolo inclusione tra specie "<<s1<<" e specie "<<s2<<endl;

     for (i=0; i<colonne; i++){
         if ((matrice[s1][i]==1) & (matrice[s2][i]==0)){
         //   cout<<"non incluso "<<endl;
            return 0;
            }
            }
   //  cout<<"incluso "<<endl;
   //  cout<<endl;
     return 1;
}
//conta il numero di 1 in una specie (riga)
int conta_uni(int** m, int riga, int colonne){
    int i,n;
    n=0;
    for(i=0; i<colonne; i++){
        if(m[riga][i]==1) n++;
    }
   return n;
}

int trova_sorgenti(int colonna, int righe){
    int i;
    if(specieRealizzate[colonna]==1) return 0;
    if(sorgenti_potenziali[colonna]==0) return 0;
    //verifico che specie non abbia archi entranti
    for (i=0; i<righe; i++){
        if(hasse[i][colonna]==1) return 0;
    }
    return 1;
}

int trova_sink(int riga, int righe){
    int i;
    //cout<<"trovo sink"<<endl;
    //se la specie � gi� stata realizzata
    if (specieRealizzate[riga]==1) return 0;
    for (i=0; i<righe; i++){
        if(hasse[riga][i]==1) return 0;
    }
    return 1;
}
//calcolo in-degree di un nodo (specie colonna in matrice Hesse)
int inDeg(int colonna, int righe){
    int i, deg;
    deg=0;
    for (i=0; i<righe; i++){
        if(hasse[i][colonna]==1) deg++;
    }
    return deg;
}

//calcolo out-degree di un nodo (specie riga in matrice Hesse)
int outDeg(int riga, int righe){
    int i, deg;
    deg=0;
    for (i=0; i<righe; i++){
        if(hasse[riga][i]==1) deg++;
    }
    return deg;
}

void aggiungi_nodo(int s, int righe){
    int i;
    if(indice_path<righe) percorso[indice_path]=s;
    indice_path++;
}


int realizza_percorso(int** matrice, int righe, int colonne, int* percorso){
int i,j, e, ii, cont_neri, cont_specie, cont, n_componenti,f,g,sp,en,si,so,p,car;
    int** grb;
    int* c_universali;
    int* s_realizzate;
    int* c_attivi;
    int ammissibile;
    int* soluzione_massimali;
//cout<<"dentro la realizzazione del percorso"<<endl;
     grb = (int **)calloc(righe, sizeof(int *));
                   for (i = 0; i < righe; i++){
                       grb[i] = (int *)calloc(colonne, sizeof(int));
                   }

                   for (i = 0; i < righe; i++) {
                       for (j = 0; j < colonne; j++) {
                        //  grb[i][j]=GRB[i][j];
                        grb[i][j]=matrice[i][j];

                           }
                   }

                   c_universali=new int[colonne];
                 for(i=0; i<colonne; i++){
                          cont_neri=0;
                          for(j=0; j<righe; j++){
                                   if(grb[j][i]==1) {
                                      cont_neri++;
                                   }
                          }
                          if(cont_neri>0) c_universali[i]=0;
                          if(cont_neri==0) c_universali[i]=1;
                          }
                     //specie realizzate: nessun arco entrante.
                      s_realizzate=new int[righe];
                     for(i=0; i<righe; i++){
                              cont_specie=0;
                              for(j=0; j<colonne; j++){
                                       if(grb[i][j]!=0) {
                                          cont_specie++;
                                       }
                              }
                              if(cont_specie!=0) s_realizzate[i]=0;
                              if(cont_specie==0) s_realizzate[i]=1;
                              }

                         //caratteri attivi: attivo se reso universale e NON connesso con archi rossi a tutte le specie della sua componente
                         c_attivi=new int[colonne];
                         for(i=0; i<colonne; i++) c_attivi[i]=0;
                         for(i=0; i<colonne; i++){
                              if (c_universali[i]==1){
                                  cont=0;
                                  for(j=0; j<righe; j++){
                                           if(grb[j][i]==2) cont++;
                                                      }
                                  if(cont==righe) c_attivi[i]=0;
                                  if(cont<righe) c_attivi[i]=1;
                              }
                        /*soluzione_massimali=new int[colonne];
                         for(i=0; i<colonne; i++) soluzione_massimali[i]=-1;*/
                         }
    //leggo percorso da fine a inizio
    for(i=righe-1; i>-1; i--){
    //per ogni nodo del percorso, se nodo !=-1
    if(percorso[i]!=-1){
        //realizza la specie
     //   cout<<"realizzo la specie "<<percorso[i]<<endl;
        //realizza_specie(matrice,righe,colonne,percorso[i],s_realizzate, c_universali, c_attivi, grb, soluzione_massimali);
        realizza_specie(grb,righe,colonne,percorso[i],s_realizzate, c_universali, c_attivi);
       /*   cout<<"grb subito dopo realizzazione"<<endl;
              for(f=0; f<righe; f++){
                for(g=0; g<colonne; g++){
                    cout<<grb[f][g];
                }
                cout<<endl;
              }*/


    }
    }
   ammissibile=1;
cout<<"vvvvvvvvvvvvvvvvvvvvvvv"<<endl;
// verifico ammissibilit� sink (percorso[0])
    n_componenti=calcola_componenti(grb,righe,colonne);
    if(n_componenti=-2) return 1; //grb formato da soli singoletti --> nessun conflitto!

    ConflictGraph cg (grb,righe,colonne);
    cout<<"qui"<<endl;
    cg.print_graph();

  /*  for(j=0; j<colonne; j++){
        if(matrice[percorso[0]][j]==1){ //j:carattere del sink
            //verifico che non ci siano conflitti tra i caratteri del sink
            for(e=0; e<cg.edges; e++){
                if((archi_gabry[e][0]==j)|(archi_gabry[e][1]==j)){
                    cout<<"carattere "<<j<<"  ancora in conflitto"<<endl;
                    ammissibile=0;

                }
            }
        }
    }*/
    //per ogni coflitto, verifico se la coppia di caratteri appartiene al sink. In tal caso, sink non ammissibile
    for(e=0; e<cg.edges; e++){
            if(ammissibile==1){
            if(matrice[percorso[0]][archi_gabry[e][0]]==1){
                if(matrice[percorso[0]][archi_gabry[e][1]]==1){
                   // cout<<"conflitto tra i caratteri "<<archi_gabry[e][0]<<archi_gabry[e][1]<<endl;
                    ammissibile=0;
                }
            }
            }
    }
    if(ammissibile==1){
        //    cout<<"Percorso ammissibile!"<<endl;

   /*    //copia grb in GRB generale
       for(f=0; f<righe; f++){
                for(g=0; g<colonne; g++){
                    GRB[f][g]=grb[f][g];
                }
              }
        for(car=0; car<colonne; car++){
          if(c_universali[car]==1)  CarUniversali[car]=c_universali[car];
        }
        //for(sp=0; sp<righe; sp++) S_Realizzate[sp]=s_realizzate[sp];
        for(sp=0; sp<righe; sp++){
          if(s_realizzate[sp]==1)  specieRealizzate[sp]=s_realizzate[sp];
        }

       cout<<"........................pezzo di soluzione da copiare: ";
       for(car=0; car<colonne; car++){
           cout<<soluzione_massimali[car];
       }
       cout<<endl;
       for(car=0; car<colonne; car++){
          if(soluzione_massimali[car]!=-1)  {
              //  soluzioneMassimali[contatore]=soluzione_massimali[car]; DA REINTRODURRE E SISTEMARE
                //copio vettori caratteri attivi
                for(i=0; i<colonne; i++){
                    soluzioneCarAttivi[car][i]=soluzionecarattivi[car][i];
                }
                contatore++;
          }
        }*/


        return 1;
    }
    else if (ammissibile==0) {
      //  cout<<"Percorso NON ammissibile!"<<endl;
            return 0;
    }
}

void realizza_specie(int** Grb, int righe, int colonne, int specie, int* specie_realizzate, int* caratteri_universali, int* caratteri_attivi){
     int car,i,j;
     int flag_realizzata;
     //per ogni carattere della specie non ancora universale, rend universale
//cout<<"........................"<<endl;
     if(specie_realizzate[specie]==0){
           flag_realizzata=0;
        //   cout<<"Realizzo la specie "<<specie<<endl;
          /* cout<<"Caratteri universali: ";
           for(i=0; i<colonne; i++) cout<<caratteri_universali[i];
           cout<<endl;*/

         /*  for(i=0; i<righe;i++){
            for(j=0; j<colonne;j++){
                cout<<Grb[i][j];
            }
            cout<<endl;
           }*/

          //per ogni carattere a 1 nella specie, se il carattere non � ancora stato reso universale rendilo universale

              while(flag_realizzata==0){
               //while(!(specie_realizzata(n_matrice_avvalorata,riga,colonna,specie))){
                 // cout<<"specie non ancora realizzata"<<endl;
              //    c_max=massimale(matrice,righe,colonne,specie,caratteri_universali);
                 // cout<<"carattere massimale da realizzare: "<<c_max<<endl;
                // for(i=0; i<colonne; i++) cout<<caratteri_universali[i];
                 //cout<<endl;
                // for(i=0; i<colonne; i++) cout<<Grb[specie][i];
                // cout<<endl;
                 for(i=0; i<colonne; i++){
                   if((Grb[specie][i]==1)&(caratteri_universali[i]==0)){
//cout<<"devo rendere universale "<<i<<endl;
/*cout<<"prima di realizzare:"<<endl;
for(i=0; i<righe;i++){
            for(j=0; j<colonne;j++){
                cout<<Grb[i][j];
            }
            cout<<endl;
           }*/
                rendi_universale(Grb, righe, colonne, i,  caratteri_universali, caratteri_attivi, specie_realizzate);  //i: carattere da rendere universale
               //cout<<"��������������������������������������������������������qqqqqqq"<<endl;
              //  estendi(i, Grb);
//cout<<"dopo aver realizzato:"<<endl;
//for(i=0; i<righe;i++){
 //           for(j=0; j<colonne;j++){
   //             cout<<Grb[i][j];
     //       }
       //     cout<<endl;
         //  }
              //  aggiorna_soluzione(c_max, n_matrice_avvalorata, riga, colonna, etichette); //soluzione con etichette
               // cout<<"aggiornata"<<endl;
                //DA RIVEDERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            /*    cout<<"car universali dentro ciclo: "<<endl;
     for(i=0; i<colonne; i++) cout<<caratteri_universali[i];
     cout<<endl;/*
                 cout<<"specie realizzate dentro ciclo: "<<endl;
     for(i=0; i<righe; i++) cout<<specie_realizzate[i];
     cout<<endl*/

                 if(specie_realizzata(Grb,righe,colonne,specie,caratteri_universali)) {
                    //    cout<<"specie realizzata"<<endl;
                        flag_realizzata=1;
                 }
               //  else cout<<"cerco altrO CARATTERe da realizzare"<<endl;
              // }
          //}
         }
     //     cout<<"specie realizzata"<<endl;

      specie_realizzate[specie]=1;
     }
    // cout<<"specie realizzate: "<<endl;
    // for(i=0; i<righe; i++) cout<<specie_realizzate[i];
    // cout<<endl;
     }
    }
}


void rendi_universale(int** Grb, int righe, int colonne, int i, int* caratteri_universali, int* caratteri_attivi, int* specie_realizzate){  //i: carattere da rendere universale
  int r,j, k,kk,ii,s;
  int* tspecie;
  int* tcarattere;
//cout<<righe<<colonne<<endl;
  tspecie=new int[righe];
  tcarattere=new int[colonne];
  cout<<"matrice dentro rendi universale"<<endl;
  printMatrix(Grb, righe, colonne);

  printf("--**--Rendo universale il carattere %d\n", i);
/*cout<<"caratteri attivi prima: ";
    for(kk=0; kk<colonne; kk++){
     cout<<caratteri_attivi[kk];
                }
     cout<<endl;*/
  //il carattere da rendere universale diventa attivo
  caratteri_attivi[i]=1;

/*)  cout<<"caratteri attivi dopo: ";
    for(kk=0; kk<colonne; kk++){
     cout<<caratteri_attivi[kk];
                }
     cout<<endl;*/

  caratteri_universali[i]=1;

 // soluzione_massimali[q]=i;
 // q++;

 /* printf("Caratteri attivi: ");
  for(j=0; j<column; j++) printf("%d", caratteri_attivi[j]);
  printf("\n");*/
  // calcolo componente connessa del carattere
  for(j=0; j<righe; j++) tspecie[j]=0;
  for(j=0; j<colonne; j++) tcarattere[j]=0;

  valuta_colonna(Grb, righe, colonne, i, tspecie, tcarattere);

/*cout<<"componente connessa di "<<i<<": ";
for(j=0; j<righe; j++) cout<<tspecie[j];
cout<<endl;*/

  for (j=0; j<righe; j++){ //scorro le specie per vedere a cosa connettere il carattere
       // cout<<j<<matrice[j][i]<<endl;
    if (Grb[j][i]==1){
       // cout<<j<<endl;
        Grb[j][i]=0; //cancello archi neri
       // cout<<Grb[j][i]<<endl;
    }
    else if (Grb[j][i]==0) {
    //verifica che la specie appartiene alla componente di cui fa parte il carattere. Se appartiene metti un arco rosso

       if ((tspecie[j]==1) & (specie_realizzate[j]==0)){
                  Grb[j][i]=2;    //arco rosso tra carattere e specie della componente connessa se specie non ancora realizzata
                           // rossi[j][i]=1;
                            }
    }

}

/*
cout<<"grb dentro rendi universale....."<<endl;
for(r=0; r<righe; r++){
        for(j=0; j<colonne; j++){
            cout<<Grb[r][j];
        }
        cout<<endl;
    }
    cout<<endl;*/


//estendi(i, Grb);

/*for(r=0; r<righe; r++){
        for(j=0; j<2*colonne; j++){
            cout<<estesa[r][j];
        }
        cout<<endl;
    }
    cout<<endl;*/
//cout<<"prima di aggiornare car attivi"<<endl;
// aggiorna_caratteri_attivi(matrice, righe, colonne, caratteri_attivi, Grb, specie_realizzate);
  aggiorna_caratteri_attivi(Grb, righe, colonne, caratteri_attivi, specie_realizzate);
//  cout<<"$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ CAR ATTIVI dopo aver reso universale "<<i<<": ";
 // for(ii=0; ii<colonne; ii++)
   // cout<<caratteri_attivi[ii];
 // cout<<endl;
 // cout<<"finito di rendere universale"<<endl;
/*cout<<",,,,,,,,,,,,,,,,,,,,,,,,grb dopo aggiornamento"<<endl;
for(r=0; r<righe; r++){
        for(j=0; j<colonne; j++){
            cout<<Grb[r][j];
        }
        cout<<endl;
    }
    cout<<endl;*/
//cout<<"q: "<<q<<endl;
//cout<<"qui"<<endl;
  //  for(i=0; i<colonne; i++) soluzionecarattivi[q][i]=caratteri_attivi[i];
    //cout<<"qui"<<endl;
  //  cout<<"zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz"<<endl;
//q++;

//cout<<"qui"<<endl;
}

int valuta_colonna(int** Grb, int righe, int colonne, int carattere, int* tspecie, int* tcarattere){
    int i;
    for (i=0; i<righe; i++){
        //se il carattere � collegato alla specie con un arco nero o un arco rosso allora � connesso alla specie
        if ((Grb[i][carattere]==1) | (Grb[i][carattere]==2)){
                                    if (tspecie[i]==0) {
                                                       tspecie[i]=1;
                                                       //printf("specie connessa al carattere %d: %d", carattere, i);
                                                       valuta_riga(Grb, righe, colonne, i, tspecie, tcarattere);
                                    }
        }
    }
}

int valuta_riga(int** Grb, int righe, int colonne, int specie, int* tspecie, int* tcarattere){
    int j;
    //printf("inizio di valuta riga\n");
    //costruzione grafo connesso di "carattere"

    for (j=0; j<colonne; j++){
        if ((Grb[specie][j]==1) | (Grb[specie][j]==2)){
                                    if (tcarattere[j]==0) {
                                                       tcarattere[j]=1;
                                                       valuta_colonna(Grb, righe, colonne, j, tspecie, tcarattere);
                                                       }
                                    }
    }


}

void aggiorna_caratteri_attivi(int** Grb, int righe, int colonne, int* caratteri_attivi, int* specie_realizzate){
     int i,j, specie,k,r;
     int flag;
     int* tspecie;
     int* tcarattere;

     tspecie=new int[righe];
     tcarattere=new int[colonne];
    // cout<<"aggiorno caratteri attivi"<<endl;

     //per ogni carattere attivo
     for(i=0; i<colonne; i++){
              if(caratteri_attivi[i]==1){
                                         flag=0;
                                         //cout<<"stabilisco se disattivare carattere "<<i<<endl;
                                         // calcolo componente connessa del carattere
                                            for(j=0; j<righe; j++) tspecie[j]=0;
                                            for(j=0; j<colonne; j++) tcarattere[j]=0;
                                            valuta_colonna(Grb, righe, colonne, i, tspecie, tcarattere);

                                         //se carattere connesso con archi rossi a tutte le specie della sua componente, posso spegnerlo

                                         for(k=0; k<righe; k++){
                                                  if((tspecie[k]==1) & (Grb[k][i]!=2)) flag=flag+1;
                                                  }
                                         if (flag>0){
                                                      caratteri_attivi[i]=1;
                                                      //cout<<"non disattivo"<<endl;
                                                      }
                                         // se disattivo un carattere, devo controllare le specie realizzate
                                         if (flag==0) {
                                                      //cout<<"disattivo carattere "<<i<<" colonna "<<colonna<<endl;
                                                    //  cout<<"disattivo carattere "<<i<<endl;
                                                      caratteri_attivi[i]=0;
                                                      //elimino carattere e relativi archi da grafo rb
                                                      for(specie=0; specie<righe; specie++) Grb[specie][i]=0;

                                                      aggiorna_specie_realizzate(Grb, righe, colonne, specie_realizzate);

                                                      aggiorna_caratteri_attivi(Grb, righe, colonne, caratteri_attivi, specie_realizzate);
                                                      }
                                         }
              }
}

void aggiorna_specie_realizzate(int** Grb, int righe, int colonne, int* specie_realizzate){
     int k,i,j;
     int archi;

     for(i=0; i<righe; i++){ //confronto ogni specie con il vettore di caratteri attivi relativi alla stessa componente della specie.

              if(specie_realizzate[i]==1); //se la specie � gi� stata realizzata, vai avanti
              if(specie_realizzate[i]==0){
                 archi=0;
                 //controlla se ci sono archi entranti
                 for(j=0; j<colonne; j++){
                          if(Grb[i][j]!=0) archi=archi+1;
                          }
                 if (archi==0) {
                        cout<<"specie "<<i<<" realizzata"<<endl;
                        specie_realizzate[i]=1;
                        specieRealizzate[i]=1;
                 }
              }
    }
 }


int valuta_colonna(int** matrice, int righe, int colonne, int carattere, int** Grb, int* tspecie, int* tcarattere){
    int i;
    for (i=0; i<righe; i++){
        //se il carattere � collegato alla specie con un arco nero o un arco rosso allora � connesso alla specie
        if ((Grb[i][carattere]==1) | (Grb[i][carattere]==2)){
                                    if (tspecie[i]==0) {
                                                       tspecie[i]=1;
                                                       //printf("specie connessa al carattere %d: %d", carattere, i);
                                                       valuta_riga(matrice, righe, colonne, i, Grb, tspecie, tcarattere);
                                    }
        }
    }
}

int valuta_riga(int** matrice, int righe, int colonne, int specie, int** Grb, int* tspecie, int* tcarattere){
    int j;
    //printf("inizio di valuta riga\n");
    //costruzione grafo connesso di "carattere"

    for (j=0; j<colonne; j++){
        if ((Grb[specie][j]==1) | (Grb[specie][j]==2)){
                                    if (tcarattere[j]==0) {
                                                       tcarattere[j]=1;
                                                       valuta_colonna(matrice, righe, colonne, j, Grb, tspecie, tcarattere);
                                                       }
                                    }
    }


}


bool specie_realizzata(int** matrice,int righe,int colonne,int specie, int* caratteri_universali){
    int i;
  /*  cout<<"dentro specie realizzata"<<endl;
    for(i=0; i<colonne; i++) cout<<caratteri_universali[i];
   cout<<endl;
   cout<<"matrice:"<<endl;
   printMatrix(matrice, righe, colonne);*/

    for(i=0; i<colonne; i++){
           // cout<<i<<endl;
        if((matrice[specie][i]==1)&(caratteri_universali[i]==0)) {
              //  cout<<"car "<<i<<"ancora da realizzare"<<endl;
                return false;
        }
    }
  //  cout<<"realizzato tutti i caratteri"<<endl;
    return true;
}
/*
//aggiorno sorgenti potenziai
void aggiorna_sorgenti_potenziali(int specie){
    int i;
    sorgenti_potenziali[specie]=0;
    for(i=0; i<righe; i++){
        if(included(i,specie)) sorgenti_potenziali[i]=0;
    }
}*/

//verifico se s1<s2
int included(int** matrice, int righe, int colonne, int s1, int s2){
    int i;
    for(i=0; i<colonne; i++){
        if((matrice[s1][i]==1)&(matrice[s2][i]==0)) return 0;
    }
    return 1;
}

void estendi(int carattere, int** Grb){

  int i,j;
cout<<"_______________________________________________________________estendo il carattere "<<carattere<<endl;
cout<<"righe e colonne :"<<righeO<<colonneO<<endl;
printMatrix(Grb,righeO, colonneO);
  for (i=0; i<righeO; i++){
       // for (j=0; j<colonne; j++){
           // if(CarUniversali[j]==1){ //estendi solo i caratteri che sono gi� stati resi universali
              if (matriceO[i][carattere]==1) {
            	estesa[i][2*carattere]=1;
            	estesa[i][2*carattere+1]=0;
              }

            /*  if (matrice[i][carattere]==3) {
            	estesa[i][2*carattere]=0;
            	estesa[i][2*carattere+1]=0;
              }*/

              if (matriceO[i][carattere]==0) {
                	if (Grb[i][carattere]==0) {
                	  estesa[i][2*carattere]=0;
                	  estesa[i][2*carattere+1]=0;
                	}

                	if (Grb[i][carattere]==2) {
                	  estesa[i][2*carattere]=1;
                	  estesa[i][2*carattere+1]=1;
                	}
              }
      //  }
       // }
  }
  cout<<"mmm"<<endl;
  printMatrix(estesa, righeO, colonneO*2);
}

//Inverto ordine nodi percorso. es in:123-1   out: 321-1
int* riordina_percorso(int* percorso, int righe){
    int i, nodi;
    int* ordinato;

    ordinato=new int[righe];
    for(i=0; i<righe; i++) ordinato[i]=-1;
    nodi=0;
    for(i=0; i<righe; i++){
        if(percorso[i]!=-1) nodi++;
    }
    for(i=0; i<nodi; i++) ordinato[i]=percorso[nodi-1-i];
    cout<<"percorso ordinato: ";
    for(i=0; i<righe; i++) cout<<ordinato[i];
    cout<<endl;
    return ordinato;
}


void aggiornaSoluzione(int minimale){
cout<<"inserisco minimale "<<minimale<<endl;
//1- trovo specie pi� vicina alla radice: � quella che nella matrice estesa ha la riga con minor numero di 1
//cio� il minor numero di operazione di attivazione e spegnimento
    //- per ogni specie che contiene il minimale
    //calcola il punteggio
    //prendi il punteggio minimo
//2- dopo attivazione di ultimo carattere della specie trovata, inserisco minimale
}

// verifico se il carattere "carattere" appartiene alla specie "specie"
int appartiene(int carattere, int specie){
    int i;
    if (matriceO[specie][carattere]==1) return 1;
    return 0;
}

//verifica se c  con un carattere A gi� nella soluzione induce le configurazioni (0,1) (1,0) (1,1) nella matrice M
int conflitto(int c){
    int i,k, flag1, flag2, flag3;

   // cout<<"verfico se "<<c<<" in conflitto"<<endl;
    for(i=0; i<colonneO; i++){
        if(inserito(i)==1){
        // cout<<"confronto con colonna "<<i<<endl;
         flag1=0;
         flag2=0;
         flag3=0;
         for (k = 0; k < righeO; k++) {
            if ((matriceO[k][i] == 1 ) & (matriceO[k][c] == 1)) flag1 = 1;
    		if ((matriceO[k][i] == 0) & (matriceO[k][c] == 1)) flag2 = 1;
    		if (((matriceO[k][i] == 1)) & (matriceO[k][c] == 0)) flag3 = 1;
    		}
    		if (flag1 & flag2 & flag3)  return 1;
		}
	}
    return 0;
}

//verifica se un carattere � gi� stato inserito nella soluzione
int inserito(int c){
    int i;

    for(i=0; i<colonneO; i++){
        if(soluzione[i]==c) return 1;
    }
   // cout<<c<<" non ancora inserito"<<endl;
    return 0;
}

//elimina eventuali righe uguali
void compattaIndottaMassimali(){
    int i,j, nr, i_matrice;
    int* copia;
    copia=new int[righe_indotta];
    for(i=0; i<righe_indotta; i++) copia[i]=-1;
    nr=0;

  /*  cout<<"matrice indotta da massimali:"<<endl;
    for(i=0; i<righe_indotta; i++){
            for(j=0; j<colonne_indotta; j++){
                cout<<matrice_indotta[i][j];
            }
            cout<<endl;
        }
        cout<<endl;*/

    //calcola le righe diverse
    copia[0]=0;
    nr++;
    for(i=1; i<righe_indotta; i++){
           // cout<<"riga "<<i<<endl;
        if(specieDiversa(i)==1){
           // cout<<"diversa!"<<endl;
            copia[i]=0;
            nr++;
        }
    }
   // cout<<"numero di specie diverse: "<<nr<<endl;
    righeMC=nr;

    colonneMC=colonne_indotta;

    matriceMC=new int*[righeMC];
        for(i=0; i<righeMC; i++){
             matriceMC[i]=new int [colonneMC];
             }
    //riempio la matrice
    i_matrice=0;
    for(i=0; i<righe_indotta; i++){
            if(copia[i]==0){
             for(j=0; j<colonneMC; j++){
               matriceMC[i_matrice][j]=matrice_indotta[i][j];
        }i_matrice++;
    }

    }

    cout<<"matrice ripulita indotta da massimali:"<<endl;
    for(i=0; i<righeMC; i++){
            for(j=0; j<colonneMC; j++){
                cout<<matriceMC[i][j];
            }
            cout<<endl;
        }
        cout<<endl;


  }

//verifica se la specie s � diversa da tutte le specie che la precedono
int specieDiversa(int s){
    int i,j;
    int cont; //conta le uguaglianze
    cont=0;
    for(i=0; i<s; i++){ //per ogni specie i che precede la specie s
        if(specieUguali(i,s)) cont++;
    }
   if(cont>0) return 0;
   else return 1;
}

int specieUguali(int s1, int s2){
    int i;
    for(i=0; i<colonne_indotta; i++)
        if(matrice_indotta[s1][i]!=matrice_indotta[s2][i]) return 0;
    //cout<<s1<<s2<<"sono uguali"<<endl;
    return 1;
}

 int* costruiscoPercorso(int** matrice, int** hasse, int righe, int colonne, int source, int sink){
    int flag,i;
    int* percorso;

    percorso=new int[righe];
    for(i=0; i<righe; i++) percorso[i]=-1;

    i=0;
    percorso[i]=source;

    while(percorso[i]!=sink){
        i++;
    percorso[i]=trovaSuccessivo(matrice, hasse, righe, colonne, percorso[i-1], sink);
    //cout<<"nodo successivo: "<<percorso[i]<<endl;
    }

    for(i=0; i<righe; i++) cout<<percorso[i];
         cout<<endl;
    return percorso;
 }


int trovaSuccessivo(int** matrice, int** hasse, int righe, int colonne, int nodo, int sink){
int i;

for(i=0; i<righe; i++){
    if(hasse[nodo][i]==1){ //se specie i � collegata al nodo precedente
        if(included(matrice, righe, colonne, i, sink)) //se specie i � inclusa nel sink
            return i;
    }
}
}
//prende una matrice, ne calcola le c.c. e su ogni componente richiama la procedura per risolvere la singola componente
void riduciMatrice(int** GRB, int righe, int colonne){
int n_componenti, i, j,ii;
int cont;
int* universali;
cout<<"GRB in riduci matrice"<<endl;
printMatrix(GRB, righeO, colonneO);
     // calcola le componenti connesse del grafo rosso nero
    n_componenti=calcola_componenti(GRB,righe,colonne);
    cout<<"numero di componenti: "<<n_componenti+1<<endl;

   // per ogni componente GRB connessa
   for(contatore_componente=0; contatore_componente<=n_componenti; contatore_componente++){
        cout<<"componente "<<contatore_componente<<endl;


        // calcola matrice indotta dalla componente connessa di Grb
        indotta_cc(contatore_componente);
      //  matrice_con_minimali=0;  //flag per indicare se ci sono minimali nella matrice considerata
      cout<<"matrice indotta dalla c.c.:"<<endl;
      printMatrix(matrice_cc, righe_cc, colonne_cc);

        universali = (int *) calloc(colonne_cc, sizeof(int));
     for (i = 0; i < colonne_cc; i++)
         universali[i] = 0;

     for(i=0; i<colonne_cc; i++){
        cont=0;
        for(j=0; j<righe_cc; j++){
            if(matrice_cc[j][i]==2) cont++;
        }
     if(cont>0) universali[i]=1;
     }
     cout<<"car universali matrice indotta: ";
     for(i=0; i<colonne_cc; i++) cout<<universali[i];
     cout<<endl;

        // calcola i caratteri massimali
        //calcola_massimali(colonne_cc, car_universali, matrice_cc, righe_cc, specie_realizzate);
        calcola_massimali(colonne_cc, universali, matrice_cc, righe_cc, specie_realizzate);
        // Calcolo la matrice indotta da soli massimali
        indotta_massimali(matrice_cc,righe_cc,colonne_cc,massimali);

        compattaIndottaMassimali();
//cout<<"GRB in riduci matrice dopo compattamento"<<endl;
//printMatrix(GRB, righeO, colonneO);
      //  calcolaSoluzione(matrice_cc, righe_cc, colonne_cc); //passo il grb di questa matrice
    /*  cout<<"INDICI DI CORRISPONDENZA: ";
for(i=0; i<colonneMC; i++) cout<<corrispondenzaMC[i];
cout<<endl;*/
      cout<<"matrice indotta dai soli massimali:"<<endl;
      printMatrix(matriceMC, righeMC, colonneMC);
      calcolaSoluzione(matriceMC, righeMC, colonneMC);
   }
   //controllo se tutti i caratteri sono stati resi universali. Se non � cos�, richiamo l'algoritmo ricorsivamente
   for(i=0; i<colonneO; i++) cout<<car_universali[i];
   cout<<endl;
   int somma=0;
   for(i=0; i<colonneO; i++) somma=somma+car_universali[i];
   if(somma==colonneO) {
        cout<<"Ho realizzato tutti i caratteri!"<<endl;
        cout<<"SOLUZIONE: ";
        for(ii=0; ii<colonneO; ii++) cout<<soluzione[ii];
        cout<<endl;
   }
   else{
    cout<<"Ricorsione!!!"<<endl;
    riduciMatrice(GRB, righeO, colonneO);
   }

 }

 void trovaPercorsi(int** matrice, int** hasse, int righe, int colonne){ //matrice di soli massimali
  int** percorsi;
  int* safe;
  int* percorsoOrdinato;
  int numeroPercorso;
  int i,j,k,c, nSo, nSi, ii, jj;
  int* c_universali;
  int* s_realizzate;
  int* c_attivi;
  int* soluzione_massimali;
  int cont_neri, cont_specie, cont;
//cout<<"NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN"<<endl;
  nSo=0;
  nSi=0;
  sorgenti=new int[righe];
  sorgenti_potenziali=new int[righe];
  sink=new int[righe];
  in_deg=new int[righe];
  specieRealizzate=new int[righe];
  for(i=0; i<righe; i++) sorgenti_potenziali[i]=1; //inizialmente suppongo che qualsiasi nodo possa essere sorgente
  for(i=0; i<righe; i++) specieRealizzate[i]=0;
//SISTEMARE CONDIZIONE ITERAZIONE: CONTROLLO SU GC MASSIMALI
 //while((flag_hasse_vuoto==0)&(flag_ammissibile==1)){

  for(i=0; i<righe; i++){
        sorgenti[i]=trova_sorgenti(i,righe);
        sink[i]=trova_sink(i,righe);
        in_deg[i]=inDeg(i,righe);
 }
 for(i=0; i<righe; i++){
    if (sorgenti[i]==1){
     //   cout<<i<<" sorgente"<<endl;
        nSo++;
    }
 }

 for(i=0; i<righe; i++){
    if (sink[i]==1) {
          //  cout<<i<<" sink"<<endl;
            nSi++;
    }
 }
 /*cout<<"in-deg nodi:";
 for(i=0; i<righe; i++){
    cout<<in_deg[i];
 }
 cout<<endl;*/

 //inizializzo matrice percorsi: al max numero percorsi= numero sources*numero sink
 percorsi=new int*[nSo*nSi];
    for(i=0; i<(nSo*nSi); i++){
             percorsi[i]=new int [righe]; //ogni percorso � al massimo la lista di tutte le specie della matrice
             }
for(i=0; i<nSo*nSi; i++){
    for(j=0; j<righe; j++)
        percorsi[i][j]=-1;
}

numeroPercorso=0;

 for(i=0; i<righe; i++){
    if(sorgenti[i]==1){
        for(j=0; j<righe; j++){
            if(sink[j]==1){
                if(included(matrice, righe, colonne, i,j)){
                   // cout<<"costruisco percorso tra "<<i<<j<<endl;
                    percorsi[numeroPercorso] = costruiscoPercorso(matrice, hasse, righe, colonne, i, j);
                    numeroPercorso++;
                }
            }
        }
    }
 }

 //determino quali percorsi sono safe
safe=new int[nSo*nSi];
percorsoOrdinato=new int[righe];
for(i=0; i<righe; i++)percorsoOrdinato[i]=-1;
for(i=0; i<nSo*nSi; i++) safe[i]=-1;

for(i=0; i<nSo*nSi; i++){
    if(percorsi[i][0]!=-1){
        cout<<"..................determino se percorso "<<i<<" sia safe"<<endl;
        percorsoOrdinato=riordina_percorso(percorsi[i], righe); //inverto ordine dei nodi
        safe[i]=realizza_percorso(matrice, righe, colonne, percorsoOrdinato);
        cout<<"safe[i].......................: "<<safe[i]<<endl;
        if(safe[i]==1){
          //1- prendo la sorgente del percorso
      //      cout<<"realizzo la sorgente safe "<<percorsi[i][0]<<endl;
          //2- copio nella soluzione generale la sequenza di caratteri per realizzare la sorgente
            //determino indice da cui iniziare a copiare
                k=determinaIndice(soluzione);
                //copio caratteri, in base alla tabella di associazione originale-indotto
                copiaSoluzione(soluzione,k,percorsi[i][0], matrice, colonne);
                cout<<"soluzione: ";
                for(c=0; c<colonneO; c++) cout<<soluzione[c];
                cout<<endl;

          //3- realizzo i caratteri della sorgente nel grb della soluzione1 o della soluzione2
             //realizza_specie()

             /*             cout<<"GRB prima:"<<endl;
             for(ii=0; ii<righeO; ii++){
                for(jj=0; jj<colonneO; jj++){
                    cout<<GRB[ii][jj];
                   //cout<<ii<<jj;
                }
                cout<<endl;
             }
             cout<<endl;*/
            // cout<<"GRB prima di realizzare la specie"<<endl;
           //  printMatrix(GRB, righeO, colonneO);

              //     c_universali=new int[colonneO];
                 for(ii=0; ii<colonneO; ii++){
                       // cout<<"sssssssssssssssss"<<endl;

                          cont_neri=0;
                          for(jj=0; jj<righeO; jj++){
                                   if(GRB[jj][ii]==1) {
                                      cont_neri++;
                                   }
                          }

                          if(cont_neri>0) car_universali[ii]=0;
                          if(cont_neri==0) car_universali[ii]=1;
                          }

                     //specie realizzate: nessun arco entrante.
                    //  s_realizzate=new int[righeO];
                     for(ii=0; ii<righeO; ii++){
                              cont_specie=0;
                              for(jj=0; jj<colonneO; jj++){
                                       if(GRB[ii][jj]!=0) {
                                          cont_specie++;
                                       }
                              }
                              if(cont_specie!=0) specie_realizzate[ii]=0;
                              if(cont_specie==0) specie_realizzate[ii]=1;
                              }
//cout<<"sssssssssssssssss"<<endl;

                         //caratteri attivi: attivo se reso universale e NON connesso con archi rossi a tutte le specie della sua componente
                      //   c_attivi=new int[colonneO];
                       //  for(ii=0; ii<colonneO; ii++) car_attivi[ii]=0;
                         for(ii=0; ii<colonneO; ii++){
                              if (car_universali[ii]==1){
                                  cont=0;
                                  for(jj=0; jj<righeO; jj++){
                                           if(GRB[jj][ii]==2) cont++;
                                                      }
                                  if(cont==righeO) car_attivi[ii]=0;
                                  if(cont<righeO) car_attivi[ii]=1;
                              }
                    /*    soluzione_massimali=new int[colonneO];
                         for(ii=0; ii<colonneO; ii++) soluzione_massimali[ii]=-1;*/
                         }
//cout<<"ppppppp"<<endl;
            // realizza_specie(GRB,righeO,colonneO,percorsi[i][0],specie_realizzate, car_universali, car_attivi);
             aggiornaGRB(GRB,righeO,colonneO, k,specie_realizzate, car_universali, car_attivi);
             cout<<"GRB dopo aver realizzato i car della specie "<<percorsi[i][0]<<" della matrice indotta dai massimali:"<<endl;
             for(i=0; i<righeO; i++){
                for(j=0; j<colonneO; j++){
                    cout<<GRB[i][j];
                }
                cout<<endl;
             }
             cout<<endl;

          //4- richiamo l'algoritmo sul grb generale 1 o 2
        //  riduciMatrice(GRB, righeO, colonneO);
          //if(albero=1) riduciMatrice(grb1, righeO, colonneO);
          //if(albero=2) riduciMatrice(grb2, righeO, colonneO);
        }
    }
}
 }

 //determino l'indice da dove iniziare a copiare l'ordine di caratteri della soluzione
 int determinaIndice(int* soluzione){
     int i;
     for(i=0; i<colonneO; i++){
        if(soluzione[i]==-1) return i;
     }
 }

 //aggiorna la soluzione con i caratteri della sorgente da realizzare
 void copiaSoluzione(int* soluzione,int indice,int specie, int** matrice, int colonne){
   int i;
   for(i=0; i<colonne; i++){
    if(matrice[specie][i]==1) soluzione[indice]=corrispondenzaMC[i];
    indice++;
   }
 }

 void aggiornaGRB(int** Grb,int righeO,int colonneO,int k,int* specie_realizzate,int* car_universali, int* car_attivi){
      int car,i,j;
     int flag_realizzata;
     //per ogni carattere della specie non ancora universale, rend universale
         /*  for(i=0; i<righe;i++){
            for(j=0; j<colonne;j++){
                cout<<Grb[i][j];
            }
            cout<<endl;
           }*/
           cout<<"matrice dentro aggiornaGRB"<<endl;
           printMatrix(Grb, righeO, colonneO);
           for(i=k; i<colonneO; i++){
                if(soluzione[i]!=-1){
                        cout<<"REALIZZO IL CARATTERE "<<soluzione[i]<<endl;
                rendi_universale(Grb, righeO, colonneO, soluzione[i], car_universali, car_attivi, specie_realizzate);  //i: carattere da rendere universale
                estendi(i, Grb);

                }
    }
 }

 // verifica se tra le due colonne esiste la sottomatrice proibita
// restituisce 0 se esiste la sottomatrice proibita, 1 se non esiste

int sottomatriceProibita(int ** a, int c1, int c2) {

  int i;

  int flag1, flag2, flag3;
  flag1 = flag2 = flag3 = 0;

  //printf("\nPRIMA DEL FOR Flag 1: %d \t flag2: %d \t flag3: %d\n", flag1, flag2, flag3);

  for (i = 0; i < righeO; i++) {
    //flag1 = flag2 = flag3 = 0;
    if ((a[i][c1] == 0 ) & (a[i][c2] == 1)) {
      flag1 = 1;
      //printf("\n C'e' 0 1 tra la colonna %d e la colonna %d alla riga %d\n", c1, c2, i);
    }
    if ((a[i][c1] == 1 ) & (a[i][c2] == 0)) {
      flag2 = 1;
      //printf("\n C'e' 1 0 tra la colonna %d e la colonna %d alla riga %d\n", c1, c2, i);
    }
    if ((a[i][c1] == 1 ) & (a[i][c2] == 1)) {
      flag3 = 1;
      //printf("\n C'e' 1 1 tra la colonna %d e la colonna %d alla riga %d\n", c1, c2, i);
    }
  }
  //printf("\nFlag 1: %d \t flag2: %d \t flag3: %d\n", flag1, flag2, flag3);
  if (flag1 & flag2 & flag3) return 1; //esiste sottomatrice proibita
  else
    return 0;  //non esiste sottomatrice proibita
}
