# define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX 100

/* ---------------------- */
/* --- STRUTTURE DATI --- */
/* ---------------------- */
/* Utente */
typedef struct user {
	int id;
	char *nome;
	char *cognome;
	struct relationship *friends;
	int flag;
} User;

/* Amicizia */
typedef struct relationship {
	int year;
	int id;
	struct relationship *next;
} Relationship;

/* Grafo di utenti */
typedef struct face {
	struct user **users;
	int nusers;
} Face;

typedef struct node {
	int info;
	struct node *next;
} Node;

typedef struct queue {
	struct node *head;
	struct node *tail;
	int len;
} Queue;

typedef struct node_queue {
	struct queue *q;
	struct node_queue *next;
} Node_queue;

typedef struct queue_queues {
	struct node_queue *head;
	struct node_queue *tail;
	int len;
} Queue_queues;

/* -------------------------- */
/* --- PROTOTIPI FUNZIONI --- */
/* -------------------------- */
Face * new_graph(int nusers);
void del_graph(Face *g);
void print_graph(Face *g);

User * new_user(int id, char *n, char *c);
User * get_user(Face *g, int id);
void del_user(User *u);

void del_friends_list(Relationship *head);
Relationship * add_friend (Relationship *head, int year, int id);
int are_friends(Face *g, int u1, int u2);

Queue * new_queue();
Queue_queues * new_queue_queues();
Queue * append_queue(Queue *nl, int n);
int pop_queue(Queue *nl);
void del_queue(Queue *nl);
Queue_queues * append_queue_queues(Queue_queues *lnl, Queue *nl);
void del_queue_queues (Queue_queues *qq);

Queue * year_bfs (Face *f, int id, int year);
Queue_queues * groups (Face *f, int year);
float weakness(Face *f, int u1, int u2);

int extract_min(int *q, float *dist, int count);
int * weak_dijkstra(Face *f, int src, int dest, int count);
void coeff(Face *f, int u1, int u2);

/* ------------ */
/* --- MAIN --- */
/* ------------ */
int main() {
	int id, n, count;
	size_t lsel;
	char *sel, *name, *surname;
	Face *G;
	User *u;
	Node *nd;
	Node_queue *nq;
	Queue_queues *qq;

	/* Inizializzo le variabili per il comando */
	sel = calloc(32, sizeof(char));
	lsel = 32;

	G = new_graph(MAX);
	/* Inserimento dei primi utenti */
	printf("Numero iniziale di utenti: \n");
	scanf("%d", &n);
	for(count = 0; count<n; count++) {
		name = calloc(32, sizeof(char));
		surname = calloc(32, sizeof(char));
		scanf("%s %s %d", name, surname, &id);
		if(G->users[id]!=NULL) {
			printf("L'utente #%d esiste gia'.\n", id);
			scanf("%s %s %d", name, surname, &id);
		}
		G->users[id] = new_user(id, name, surname);
		free(name);
		free(surname);
	}

	/* Ciclo sull'istruzione ricevuta e salvata in sel */
	while(getline(&sel, &lsel, stdin)>0) {
		if(strncmp(sel,"add",strlen("add"))==0) {
			name = calloc(32, sizeof(char));
			surname = calloc(32, sizeof(char));
			sscanf(sel, " %*s %s %s %d", name, surname, &id);
			if(G->users[id]!=NULL)
				printf("L'utente #%d esiste gia'.\n", id);
			else
				G->users[id] = new_user(id, name, surname);
			free(name);
			free(surname);
		}
		else if(strncmp(sel,"find",strlen("find"))==0) {
			sscanf(sel, " %*s %d", &id);
			if (id>MAX || G->users[id] == NULL)
				printf("L'utente #%d non esiste.\n", id);
			else {
				u = get_user(G, id);
				printf("\t%s %s (%d)\n", u->nome, u->cognome, u->id);
				free(u);
			}
		}
		else if(strncmp(sel,"mkfriend",strlen("mkfriend"))==0) {
			int u1, u2, year;
			sscanf(sel, " %*s %d %d %d", &u1, &u2, &year);
			if (u1>=MAX || G->users[u1] == NULL)
				printf("L'utente #%d non esiste.\n", u1);
			else if (u2>=MAX || G->users[u2] == NULL)
				printf("L'utente #%d non esiste.\n", u2);
			else if ((u1>=MAX || G->users[u1] == NULL) && (u2>=MAX || G->users[u2] == NULL))
				printf("Gli utenti #%d e #%d non esistono.\n", u1, u2);
			else if (are_friends(G, u1, u2))
				printf("Gli utenti #%d e #%d sono gia' amici.\n", u1, u2);
			else {
				G->users[u1]->friends = add_friend(G->users[u1]->friends, year, u2);
				G->users[u2]->friends = add_friend(G->users[u2]->friends, year, u1);
			}
		}
		else if(strncmp(sel,"groups",strlen("groups"))==0) {
			int i;
			qq = groups(G,-1);
			i=1;
			for(nq = qq->head; nq != NULL; nq = nq->next) {
				printf("Gruppo #%d\n", i);
				for(nd = nq->q->head; nd != NULL; nd = nd->next) {
					u = get_user(G, nd->info);
					printf("\t%s %s %d\n", u->nome, u->cognome, u->id);
					free(u);
				}
				i++;
			}
			free(qq);
		}
		else if(strncmp(sel,"data_groups",strlen("data_groups"))==0) {
			int i, year;
			sscanf(sel, " %*s %d", &year);
			qq = groups(G,year);
			i=1;
			for(nq = qq->head; nq != NULL; nq = nq->next) {
				printf("Gruppo #%d - dall'anno %d\n", i, year);
				for(nd = nq->q->head; nd != NULL; nd = nd->next) {
					u = get_user(G, nd->info);
					printf("\t%s %s %d\n", u->nome, u->cognome, u->id);
					free(u);
				}
				i++;
			}
			free(qq);
		}
		else if(strncmp(sel,"weakness",strlen("weakness"))==0) {
			for(n=0; n<G->nusers; n++) {
				if(G->users[n]!=NULL) {
					for(count=0; count<G->nusers; count++) {
						if(G->users[count]!=NULL && n!=count) {
							printf("\t%d - %d ------ %f\n", n, count, weakness(G, n, count));
						}
					}                     
				}
			}            
		}
		else if(strncmp(sel,"coeff",strlen("coeff"))==0) {
			int u1, u2;
			sscanf(sel, " %*s %d %d", &u1, &u2);
			if (u1>=MAX || G->users[u1] == NULL)
				printf("L'utente #%d non esiste.\n", u1);
			else if (u2>=MAX || G->users[u2] == NULL)
				printf("L'utente #%d non esiste.\n", u2);
			else if ((u1>=MAX || G->users[u1] == NULL) && (u2>=MAX || G->users[u2] == NULL))
				printf("Gli utenti #%d e #%d non esistono.\n", u1, u2);
			else
				coeff(G, u1, u2);
		}
		else if(strncmp(sel,"print",strlen("print"))==0)
			print_graph(G);
		else if(strncmp(sel,"exit",strlen("exit"))==0)
			break;
	}
	free(sel);
	del_graph(G);
	return 0;
}

/* ---------------- */
/* --- FUNZIONI --- */
/* ---------------- */
/*
 * Creazione di un nuovo grafo di utenti
 * allocando spazio necessario per nusers
 * (numero massimo di utenti possibili)
 * utenti (passato come argomento)
 * e restituendo il puntatore al grafo
 */
Face * new_graph(int nusers) {
	Face *g = calloc(1, sizeof(Face));
	g->nusers = nusers;
	g->users = calloc(nusers, sizeof(User *));
	return g;
}

/*
 * Stampa del grafo passato come argomento.
 * Per ogni utente stampa le sue generalità
 * e cicla la sua lista di amicizie.
 */
void print_graph(Face *g) {
	int i;
	User *u;
	Relationship *r;
	for(i=0; i<g->nusers; i++) {
		if(g->users[i]!=NULL) {
			/* Utente facebook */
			printf("%d %s %s\n", g->users[i]->id, g->users[i]->nome, g->users[i]->cognome);
			/* Lista amici dell'utente */
			printf("[\n");
			for(r = g->users[i]->friends; r!=NULL; r=r->next) {
				u = get_user(g, r->id);
				printf("\t%d %s %s - dal %d\n", r->id, u->nome, u->cognome, r->year);
			}
			printf("]\n\n");
		}
	}
}

/*
 * Elimina il grafo passato come
 * argomento liberando la memoria
 * occupata da tale struttura rimuovendo
 * tutti gli utenti attraverso del_user()
 */
void del_graph (Face *f) {
	int i;
	for (i = 1; i <= f->nusers; ++i) {
		if (f->users[i] != NULL)
			del_user(f->users[i]);
	}
	free(f->users);
	free(f);
}

/*
 * Creazione di un nuovo utente con id, nome e cognome
 * passati come argomento della funzione.
 * Alloca spazio per una struttura User, la popola
 * con i dati passati come argomento creando anche
 * la sua lista di amicizie attraverso new_friends_list()
 */
User * new_user(int id, char *n, char *c) {
	User *u = calloc(1, sizeof(User));
	char *nome = calloc(32, sizeof(char));
	char *cognome = calloc(32, sizeof(char));
	u->id = id;
	strcpy(nome, n);
	strcpy(cognome, c);
	u->nome = nome;
	u->cognome = cognome;
	u->flag = 0;
	u->friends = NULL;
	return u;
}

/*
 * Restituisce l'utente associato
 * all'id ricevuto come parametro.
 */
User * get_user(Face *g, int id) {
	int i;
	User *u = calloc(1, sizeof(User));
	for(i=0; i<g->nusers; i++) {
		if(g->users[i]!=NULL && g->users[i]->id == id) {
			u->id = id;
			u->nome = g->users[i]->nome;
			u->cognome = g->users[i]->cognome;
			u->friends = g->users[i]->friends;
			u->flag = 0;
			break;
		}
	}
	return u;
}

/*
 * Elimina l'utente ricevuto come parametro
 * liberando lo spazio precedentemente
 * allocato per l'utente stesso.
 * Attraverso la chiamata di del_friends_list()
 * elimina anche la lista di amici dell'utente.
 */
void del_user(User *u) {
	free(u->nome);
	free(u->cognome);
	del_friends_list(u->friends);
	free(u);
}

/*
 * Aggiunge un'amicizia alla lista di amici
 * ricevuta come parametro insieme all'id
 * dell'utente con cui viene stretta l'amicizia
 * e l'anno.
 * L'inserimento avviene in testa alla lista
 * e viene restituito il puntatore alla testa
 * della lista stessa.
 */
Relationship * add_friend(Relationship *head, int year, int id) {
	Relationship *frnd = calloc(1, sizeof(Relationship));
	frnd->id = id;
	frnd->year = year;
	frnd->next = head;
	return frnd;
}

/*
 * Elimina la lista di amici ricevendo
 * come parametro il puntatore alla testa 
 * della stessa.
 */
void del_friends_list(Relationship *head) {
	Relationship *p;
	Relationship *q;
	if (head == NULL) {
		return;
	}
	for (p = head->next, q = head; p != NULL;) {
		free(q);
		q = p;
		p = p->next;
	}
}

/*
 * Verifica se due utenti, i cui id sono ricevuti
 * come parametri, sono amici tra loro.
 * Per farlo scorre la lista di amici del primo
 * utente, se trova il secondo restituisce 1,
 * altrimenti restituisce 0.
 */
int are_friends(Face *g, int u1, int u2) {
	Relationship *r = g->users[u1]->friends;
	while (r!=NULL) {
		if (r->id == u2)
			return 1;
		r=r->next;
	}
	return 0;
}

/*
 * Crea una nuova coda di Node,
 * impostando la lunghezza a 0
 * e restituendo il puntatore alla
 * struttura.
 */
Queue * new_queue() {
	Queue *q = calloc(1, sizeof(Queue));
	q->len=0;
	return q;
}

/*
 * Crea una coda di code allocando lo spazio
 * necessario, impostando la lunghezza a 0
 * e restituendo il puntatore all'indirizzo
 * di memoria allocato.
 */
Queue_queues * new_queue_queues() {
	Queue_queues *qq = calloc(1, sizeof(Queue_queues));
	qq->len=0;
	return qq;
}

/*
 * Inserisce in fondo alla coda ricevuta
 * come parametro un nuovo nodo utilizzando
 * come dato l'intero ricevuto come parametro
 * e aumentandone la lunghezza.
 * Restituisce poi il puntatore alla coda stessa.
 */
Queue * append_queue(Queue *nl, int n) {
	Node *nd = calloc(1, sizeof(Node));
	nd->info = n;
	if (nl->len == 0) {
		nl->head = nl->tail = nd;
	}
	else {
		nl->tail->next = nd;
		nl->tail = nd;
	}
	nl->len += 1;
	return nl;
}

/*
 * Estrae dalla coda ricevuta come parametro
 * il primo nodo, diminuendo la lunghezza
 * della coda stessa e restituendo il
 * valore del nodo estratto.
 */
int pop_queue(Queue *nl) {
	int ret;
	Node *next;
	ret = nl->head->info;
	next = nl->head->next;
	free(nl->head);
	nl->head = next;
	nl->len -= 1;
	return ret;
}

/*
 * Elimina la coda ricevuta come parametro
 * rimuovendo in sequenza tutti i nodi
 * attraverso pop_queue().
 */
void del_queue(Queue *nl) {
	if(nl->len > 0) {
		do {
			pop_queue(nl);
		} while(nl->len > 0);
	}
	free(nl);
}

/*
 * Aggiunge una coda in fondo alla coda di code, entrambe
 * ricevute come parametri, aumentando la dimensione della
 * coda di code e restituendo un puntatore alla stessa.
 * Per inserire una coda viene utilizzata una nuova struttura,
 * la Node_queue, che funge da "contenitore" per una coda,
 * in modo che possa mantenere come informazione ->next,
 * ovvero un puntatore alla coda successiva nella coda di code.
 */
Queue_queues * append_queue_queues(Queue_queues *lnl, Queue *nl) {
	Node_queue *l = calloc(1, sizeof(Node_queue));
	l->q = nl;
	if (lnl->len == 0) {
		lnl->head = lnl->tail = l;
	}
	else {
		lnl->tail->next = l;
		lnl->tail = l;
	}
	lnl->len += 1;
	return lnl;
}

/*
 * Rimuove la coda di code, rimuovendo ogni suo nodo
 * attraverso del_queue() e liberando lo spazio
 * allocato alla struttura stessa.
 */
void del_queue_queues (Queue_queues *qq) {
	Node_queue *p = NULL;
	Node_queue *q = NULL;
	if (qq->len == 0) 
		return;
	for (p = qq->head->next, q = qq->head; p != NULL;) {
		del_queue(q->q);
		free(q);
		qq->len -= 1;
		q = p;
		p = p->next;
	}
	del_queue(q->q);
	free(q);
	qq->len -= 1;
	free(qq);
}

/*
 * Implementa una visita in ampiezza (BFS)
 * utilizzando due code di appoggio: nl e visited.
 * Visited contiene i nodi di interi che rappresentano
 * gli utenti nel grafo che sono stati visitati.
 * Per non considerare più di una volta lo stesso nodo
 * viene utilizzato il campo ->flag della struttura User,
 * che viene impostato a 1 una volta visitato.
 * La visita in ampiezza parte dall'utente con identificativo
 * id, ricevuto come parametro e procede valutando i nodi presenti
 * nella sua lista di amicizie, così per ogni elemento della stessa.
 * Il controllo sull'anno permette di valutare solo i gruppi di
 * utenti formatisi in seguito ad un anno ricevuto come parametro
 * (-1 se si vogliono estrarre i gruppi di amici senza considerare
 * l'anno).
 * Restituisce visited, la coda di nodi visitati, ossia il gruppo di amici.
 */
Queue * year_bfs (Face *f, int id, int y) {
	Relationship *r;
	Queue *nl = new_queue();
	Queue *visited = new_queue();
	append_queue(nl, id);
	append_queue(visited, id);
	/* flaggo l'utente */
	f->users[id]->flag = 1;
	while (nl->len > 0) {
		int ppd = pop_queue(nl);
		r = f->users[ppd]->friends;
		for (; r != NULL; r = r->next) { 
			if (f->users[r->id]->flag<1 && (r->year>=y || y==-1)) {
				append_queue(visited, r->id);
				append_queue(nl, r->id);
				f->users[r->id]->flag = 1;
			}
		}
	}
	del_queue(nl);
	return visited;
}

/*
 * Creazione dei gruppi.
 * Riceve come parametri il grafo e l'anno da
 * passare a year_bfs() che crea la coda
 * di nodi in base ad una visita in ampiezza,
 * aggiungendola poi alla coda di code che restituisce.
 */
Queue_queues * groups (Face *f, int year) {
	Queue *q;
	Queue_queues * ret = new_queue_queues();
	int i;

	/* reset user->flag */
	for(i = 0; i < f->nusers; i++) {
		if(f->users[i] != NULL)
			f->users[i]->flag = 0;
	}

	for (i = 0; i < f->nusers; i++) {
		if (f->users[i]!=NULL && f->users[i]->flag<1) {
			q = year_bfs(f, f->users[i]->id, year);
			ret = append_queue_queues(ret, q);
		}
	}
	return ret;
}

/*
 * Determina la debolezza della relazione
 * di amicizia tra l'utente u1 e l'utente u2
 * passati  come parametri insieme al grafo.
 * Restituisce il valore come float.
 */
float weakness(Face *f, int u1, int u2) {
	float wknss;
	int c1, c2, cinter;
	Relationship *i;
	Relationship *j;
	cinter = 0;
	c1 = 0;

	i = f->users[u1]->friends;
	while(i!=NULL) {
		c1++;
		c2=0;
		j = f->users[u2]->friends;
		while(j!=NULL) {
			c2++;
			if(i->id == j->id || i->id == u2 || j->id == u1)
				cinter++;
			j=j->next;
		}
		i=i->next;
	}
	c1 = c1+c2;
	if(c1)
		wknss = 1 - ((float)cinter/(float)c1);
	else
		wknss = 1;

	return wknss;
}

/*
 * Estrae il nodo con distanza minima, ricevendo
 * come parametri l'array di id, l'array delle distanze
 * e la dimensione dei due.
 * Restituisce la posizione del "nodo" (memorizzato come id)
 * di quello con distanza minore.
 */
int extract_min(int *q, float *dist, int count) {
	int i, sid=0;
	float small=999;
	for(i=0; i<count; i++) {
		if(q[i]>-1 && dist[i] <= small) {
			small = dist[i];
			sid = i;
		}
	}
	q[sid] = -1;
	return sid;
}

/*
 * Calcola il percorso minimo tra due nodi src e dest ricevuti
 * come parametri. Riceve inoltre la rete di utenti e
 * il numero di utenti "reali" presenti nella stessa.
 * Restituisce un'array contente in sequenza gli id dei
 * nodi visitati.
 */
int * weak_dijkstra(Face *f, int src, int dest, int count) {
	int *q, *precedente, i, u, t=0;
	float *dist;
	Relationship *r;
	dist = malloc(count * sizeof(int));
	precedente = malloc(count * sizeof(int));
	q = malloc(count * sizeof(int));
	for(i=0; i<MAX; i++) {
		if(f->users[i]!=NULL) {
			q[t] = f->users[i]->id;
			dist[t] = 1;
			precedente[t] = -1;
			t++;
		}
	}

	dist[src] = 0;
	i=0;

	while((u = extract_min(q,dist,count))!=-1) {
		if(u==dest) {
			precedente[i] = u;
			break;
		}
		else if(dist[u]==1)
			break;

		for(r = f->users[u]->friends; r != NULL; r=r->next) {
			int v,alt;
			v = r->id;
			alt = dist[u] + weakness(f,u,v);
			if(alt < dist[v]) {
				dist[v] = alt;
				precedente[i] = u;
			}
		}
		i++;
	}

	free(q);
	free(dist);
	return precedente;
}

/*
 * Stampa il SP calcolato da weak_dijkstra()
 * ricevendo il nodo di partenza e di arrivo
 * come parametri, somma le debolezze
 * degli elementi della catena e le
 * utilizza per calcolare il coefficente di
 * debolezza tra u1 e u2.
 */
void coeff(Face *f, int u1, int u2) {
	float wtot = 0;
	int *sp, *arr, i, j=0, c=0;
	for(i=0; i<MAX; i++) {
		if(f->users[i]!=NULL)
			c++;
	}
	arr = malloc(c * sizeof(int));
	sp = weak_dijkstra(f,u1,u2,c);
	for(i=0; i<c; i++) {
		if(sp[i]!=-1) {
			arr[j] = sp[i];
			j++;
		}
		else
			arr[i] = -1;
	}
	for(i=0; i<c; i++) {
		printf("%d ",arr[i]);
	}
	i=1;
	while(arr[i]!=-1) {
		wtot += weakness(f, arr[i-1], arr[i]);
		if(arr[i]==u2)
			break;
		i++;
	}
	wtot = 1/(wtot+1);
	printf("\nCoefficiente di amicizia tra %d e %d: %f\n", u1, u2, wtot);
	i=1;
	while(arr[i]!=-1) {
		printf("%d - %d ------ %f\n", arr[i-1], arr[i], weakness(f, arr[i-1], arr[i]));
		if(arr[i]==u2)
			break;
		i++;
	}
	free(sp);
	free(arr);
}