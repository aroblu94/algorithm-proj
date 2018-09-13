# "Facebook" - 20 febbraio 2016
Aronne Brivio, 831154

## Introduzione
Il progetto richiede di implementare in C una rete di utenti che possono instaurare fra di loro una relazione di amicizia e accetta i seguenti comandi:

+ `add <nome> <cognome> <id>`: aggiunge l'utente `nome cognome`
alla rete assegnandogli l'identificativo `id`.
+ `find <id>`: cerca nella rete l'utente con id = `id`.
+ `mkfriend <id1> <id2>`: stringe un'amicizia fra `id1` e `id2`.
+ `groups`: estrae i gruppi di amici, vale a dire tutte le 
componenti del grafo (che risulta essere poi una _foresta_) connesse
tra loro.
+ `data_groups <anno>`: estrae i gruppi di amici selezionando solo le 
relazioni nate _dopo_ l'`anno` specificato.
+ `weakness`: calcola la debolezza dell'amicizia tra due utenti della rete.
+ `coeff <id1> <id2>`: calcola il coefficiente d'amicizia tra i due
utenti `id1` e `id2`, estraendo la catena di amicizie più breve
che collega i due utenti.
+ `print`: stampa la rete di utenti.

## Strutture dati scelte per l'implementazione
Per rappresentare la rete di utenti è stato utilizzato un _grafo non orientato_, con un nodo per ogni utente e un lato per rappresentare ogni relazione d'amicizia (`Relationship` - struttura dati che mantiene come informazioni l'`id` dell'amico e l'`anno` in cui è stata stretta l'amicizia).

In questo modo è possibile raccogliere le informazioni richieste in un tempo dell'ordine di `O(|V|*|E|)`, con `E` numero di amicizie (_lati_) nel grafo e `V` numero di utenti (_nodi_).

Il grafo è stato implementato attraverso _liste di adiacenza_: un array di _puntatori_ a `User` (`struct user`) di dimensione fissata `|V|` (il numero massimo di utenti che è possibile inserire nella rete), ognuna contenente una _reference_ ad una lista di amicizie:

```
    /* Grafo di utenti */
    typedef struct face {
        struct user ** users;
        int nusers;
    } Face;

    /* Utente */
    typedef struct user {
        int id;
        char * nome;
        char * cognome;
        struct relationship * friends;
        int flag;
    } User;

    /* Amicizia */
    typedef struct relationship {
        int year;
        int id;
        struct relationship * next;
    } Relationship;

```
L'inserimento all'interno della lista avviene _in testa_, quindi in tempo costante, `O(1)`.

Per l'implementazione di un algoritmo di _visita in ampiezza_ dei nodi utente, necessario per l'estrazione dei gruppi, sono state utilizzate altre strutture dati: nodi di interi `Node`, code `Queue` e una coda di code `Queue_queues`.   
(`Node_queue` viene utilizzata per mantenere nella coda di code un riferimento alla coda `Queue`).

```
    typedef struct node {
        int info;
        struct node * next;
    } Node;

    typedef struct queue {
        struct node * head;
        struct node * tail;
        int len;
    } Queue;

    typedef struct node_queue {
        struct queue * q;
        struct node_queue * next;
    } Node_queue;

    typedef struct queue_queues {
        struct node_queue * head;
        struct node_queue * tail;
        int len;
    } Queue_queues;
```
\newpage

## Funzionalità e operazioni disponibili

### Creazione del grafo 
La creazione della rete di utenti avviene attraverso la funzione `new_graph(int nusers)`, che restituisce un grafo implementato come specificato precedentemente.

### Aggiunta di un utente alla rete
L'aggiunta di un nuovo utente alla rete avviene attraverso il comando `add` che chiama la funzione `new_user(int id, char * n, char * c)` restituendo una struttura `User`, la cui _reference_ viene poi aggiunta all'array `f->users[]` (con `f` indichiamo il _grafo_).

Vista l'implementazione, la _reference_ alla struttua `User` viene aggiunta in posizione `id`:

        f->users[id] = new_user(id, nome, cognome);

In questo modo l'accesso a `f->users[id]` e la creazione dell'utente avvengono in _tempo costante_, indipendentemente dal numero di vertici e archi.   
L'aggiunta di un nuovo utente avviene quindi in tempo `O(1)`.

### Ricerca di un utente nella rete
Con `find <id>` è possibile ricevere informazioni sull'utente con l'identificativo `id`.   

Dato che l'`id` corrisponde con la posizione dell'utente in `f->users[]`, l'accesso all'utente avviene in un _tempo costante_ `O(1)` attraverso la funzione `get_user(int id)`.

### Determinare gruppi di amici
La ricerca di un gruppo di amici coincide con la ricerca dei nodi connessi in un grafo ed è stata implementata tramite _visita in ampiezza_.   
Nel caso in cui si vogliano estrarre i gruppi senza considerare l'anno, quindi attraverso il comando `groups`, come anno verrà passato alla funzione `groups (Face * f, int year)` il valore `-1`.   
In questo modo non è stato necessario scrivere due funzioni diverse per i due casi, semplicemente nella funzione `year_bfs (Face * f, int id, int year)` il controllo sull'anno tiene conto anche di questa variante, considerando tutti i nodi connessi se `year=-1`.

Per tenere conto dei nodi già visitati è stato usato il campo `flag` dell'utente, che ha valore `1` se il nodo è stato visitato, `0` altrimenti. Ad ogni chiamata di `groups()` il primo passo sarà quindi quello di resettare questo campo e riportarlo a `0` per tutti gli utenti.

La funzione di visita in ampiezza restituisce una coda `Queue` di `Node`, vale a dire nodi di interi che mantengono come informazione l'`id` del nodo utente visitato. Tale coda viene poi aggiunta alla coda di code (`Queue_queues`) in `groups()`, utilizzata per la raccolta dei nodi connessi, attraverso `append_queue_queues(Queue_queues * lnl, Queue * nl)`.

Per la visita in ampiezza vengono utilizzate due code di supporto, `nl` e `visited`. Entrambe vengono caricate con i nodi visitati attraverso `append_queue(Queue * nl, int n)`.   
All'inizio di ogni ciclo viene estratto il primo elemento di `nl` attraverso `pop_queue(Queue *nl)`, nodo che viene utilizzato per cercare i suoi adiacenti. Questa operazione viene ripetuta finchè `nl` non è vuota.

L'implementazione della visita in ampiezza, `year_bfs()`, ha un costo del tipo `O(|E|)`.   
La funzione `groups()` itera sui vertici del grafo (al più `|V|` volte), applicando `year_bfs()` su ciascuno.

Possiamo stimare che l'intera procedura di estrazione dei gruppi abbia quindi un costo pari a `O(|V|*|E|)`.

### Determinare la debolezza dell'amicizia
Con il comando `weakness` vengono calcolate e stampate le debolezze delle amicizie tra i vari utenti della rete. La formula per il calcolo della debolezza è

        weakness = 1 - (numero amici in comune/
                            numero amici diretti totali)

Il comando richiama la funzione `weakness(Face *f, int u1, int u2)` per tutti gli utenti (quindi `|V|` volte), la quale cicla le amicizie dell'utente `u1` e dell'utente `u2` (vale a dire gli elementi delle liste `f->users[u1]->friends` e `f->users[u2]->friends`), tenendo conto delle amicizie totali e di quelle in comune.

\newpage

### Determinare il coefficiente di amicizia [^0]
Con il comando `coeff <u1> <u2>` viene calcolato il coefficiente di amicizia tra gli utenti `u1` e `u2`. Questo è dato da

        coeff = 1/(somma delle debolezze della catena + 1)

Per calcolare la catena di amicizie che lega i due utenti viene utilizzato l'algoritmo di _Dijkstra_, che è implementato nella funzione `weak_dijkstra(Face *f, int src, int dest)`.   
Tale algoritmo permette di ricavare il percorso più breve tra due nodi del grafo (SP -- _shortest path_) considerando come _peso_ degli archi la debolezza dell'amicizia tra i due nodi.

Per trovare il nodo a distanza minima dal nodo sorgente (_parent_) viene richiamata la funzione `extract_min(int *q, float *dist, int count)`.

Per comodità sono stati utilizzati degli _array_ numerici, di _int_ per `*q` e `*precedente` che memorizzano l'`id` dei nodi e di _float_ per `*dist`, che memorizza invece le distanze tra il nodo padre e il nodo in posizione `dist[i]`. Per questo motivo occorre un'inizializzazione di questi array con i nodi effettivamente esistenti nel grafo (se `f->users[i]!=NULL`, quindi l'operazione viene ripetuta `|V|` volte).

Inizialmente la distanza di tutti i nodi è impostata a `1` eccetto il nodo `src` che, dovendo essere valutato per primo, si trova a distanza `0` da se stesso.

Dopo aver estratto il nodo con distanza minima vengono quindi valutati i suoi nodi adiacenti. Per questo viene sfruttata la relazione di amicizia, attraverso un ciclo sulla lista di `Relationship` relativa all'utente considerato.   
Per ogni nodo adiacente viene calcolata la debolezza tra questo e il padre, viene poi sommata alla distanza dal nodo `src` e utilizzata come nuova `dist[i]`, dove `q[i]` rappresenta il nodo che si sta valutando.

Il costo per questa operazione si può stimare come `O(|E| + |V^2|)`.

\newpage

## Conclusioni
La funzione che controlla se due utenti sono amici (`are_friends(Face *f, int u1, int u2)`), così come è stata implementata non è particolarmente efficiente. Questo perchè di fatto risulta in una _scansione completa_ della lista di amicizie di un utente.

Una soluzione più efficiente sarebbe quella di mantenere la lista di amicizie, `f->users[i]->friends`, ordinata.   
Facendo questo si perderebbe però l'inserimento di un'amicizia in tempo `O(1)`, anche se risulterebbe comunque più efficiente nel caso in cui si dovesse avere a che fare con una rete di utenti con molte relazioni di amicizia tra loro.

La scelta di utilizzare l'`id` dell'utente come posizione dello stesso nell'array, sebbene abbia lo svantaggio di dover controllare in diverse situazioni che `f->users[i] != NULL`, di portare a riordinare gli array di supporto utilizzati nel calcolo del coefficiente di amicizia (`coeff()`) e non sia efficiente a livello di memoria occupata, permette però l'estrazione dell'utente in tempo `0(1)`.   
Considerando che questa operazione viene richiesta numerose volte durante l'esecuzione del programma, come in fase di inserimento di un utente o di una relazione di amicizia per verificare che gli `id` inseriti esistano, piuttosto che in tutte le funzioni che richiedono una scansione del grafo, risulta essere comunque una scelta implementativa efficiente.

\newpage

## Sessione di esempio

Input:

```
    4
    Aronne Brivio 97
    Marco Brivio 98
    Mario Rossi 99
    Nome Cognome 88

    mkfriend 97 98 1994
    mkfriend 97 99 2010

    add Foo Bar 0
    add Luca Paccani 1
    mkfriend 0 1 2015

    add Mirko Wart 15
    mkfriend 15 1 1987

    add Federico Almaviva 8
    add Lorenzo Colombo 2
    mkfriend 8 2 2007

    add Anna Cornago 12
    mkfriend 8 12 2013

    add Davide Prete 11
    mkfriend 8 11 2008

    add Valentina Pignatale 9
    mkfriend 8 9 2016

    add Sono Solo 3

    mkfriend 11 98 2000
    mkfriend 12 88 2007

    print
    groups
    data_groups 2008
    weakness
    coeff 2 11
    exit
```

\newpage
Output:

```
    0 Foo Bar
    [
        1 Luca Paccani - dal 2015
    ]

    1 Luca Paccani
    [
        15 Mirko Wart - dal 1987
        0 Foo Bar - dal 2015
    ]

    2 Lorenzo Colombo
    [
        8 Federico Almaviva - dal 2007
    ]

    3 Sono Solo
    [
    ]

    8 Federico Almaviva
    [
        9 Valentina Pignatale - dal 2016
        11 Davide Prete - dal 2008
        12 Anna Cornago - dal 2013
        2 Lorenzo Colombo - dal 2007
    ]

    9 Valentina Pignatale
    [
        8 Federico Almaviva - dal 2016
    ]

    11 Davide Prete
    [
        98 Marco Brivio - dal 2000
        8 Federico Almaviva - dal 2008
    ]

    12 Anna Cornago
    [
        88 Nome Cognome - dal 2007
        8 Federico Almaviva - dal 2013
    ]

    15 Mirko Wart
    [
        1 Luca Paccani - dal 1987
    ]

    88 Nome Cognome
    [
        12 Anna Cornago - dal 2007
    ]

    97 Aronne Brivio
    [
        99 Mario Rossi - dal 2010
        98 Marco Brivio - dal 1994
    ]

    98 Marco Brivio
    [
        11 Davide Prete - dal 2000
        97 Aronne Brivio - dal 1994
    ]

    99 Mario Rossi
    [
        97 Aronne Brivio - dal 2010
    ]

    Gruppo #1
        Foo Bar 0
        Luca Paccani 1
        Mirko Wart 15
    Gruppo #2
        Lorenzo Colombo 2
        Federico Almaviva 8
        Valentina Pignatale 9
        Davide Prete 11
        Anna Cornago 12
        Marco Brivio 98
        Nome Cognome 88
        Aronne Brivio 97
        Mario Rossi 99
    Gruppo #3
        Sono Solo 3
```

\newpage

```
    Gruppo #1 - dall'anno 2008
        Foo Bar 0
        Luca Paccani 1
    Gruppo #2 - dall'anno 2008
        Lorenzo Colombo 2
    Gruppo #3 - dall'anno 2008
        Sono Solo 3
    Gruppo #4 - dall'anno 2008
        Federico Almaviva 8
        Valentina Pignatale 9
        Davide Prete 11
        Anna Cornago 12
    Gruppo #5 - dall'anno 2008
        Mirko Wart 15
    Gruppo #6 - dall'anno 2008
        Nome Cognome 88
    Gruppo #7 - dall'anno 2008
        Aronne Brivio 97
        Mario Rossi 99
    Gruppo #8 - dall'anno 2008
        Marco Brivio 98

    0 - 1 ------ 0.333333
    0 - 2 ------ 1.000000
    0 - 3 ------ 1.000000
    0 - 8 ------ 1.000000
    0 - 9 ------ 1.000000
    0 - 11 ------ 1.000000
    0 - 12 ------ 1.000000
    0 - 15 ------ 0.500000
    0 - 88 ------ 1.000000
    0 - 97 ------ 1.000000
    0 - 98 ------ 1.000000
    0 - 99 ------ 1.000000
    1 - 0 ------ 0.333333
    1 - 2 ------ 1.000000
    1 - 3 ------ 1.000000
    1 - 8 ------ 1.000000
    1 - 9 ------ 1.000000
    1 - 11 ------ 1.000000
    1 - 12 ------ 1.000000
    1 - 15 ------ 0.333333
    1 - 88 ------ 1.000000
    1 - 97 ------ 1.000000
    1 - 98 ------ 1.000000
    1 - 99 ------ 1.000000
    2 - 0 ------ 1.000000
    2 - 1 ------ 1.000000
    2 - 3 ------ 1.000000
    2 - 8 ------ 0.200000
    2 - 9 ------ 0.500000
    2 - 11 ------ 0.666667
    2 - 12 ------ 0.666667
    2 - 15 ------ 1.000000
    2 - 88 ------ 1.000000
    2 - 97 ------ 1.000000
    2 - 98 ------ 1.000000
    2 - 99 ------ 1.000000
    3 - 0 ------ 1.000000
    3 - 1 ------ 1.000000
    3 - 2 ------ 1.000000
    3 - 8 ------ 1.000000
    3 - 9 ------ 1.000000
    3 - 11 ------ 1.000000
    3 - 12 ------ 1.000000
    3 - 15 ------ 1.000000
    3 - 88 ------ 1.000000
    3 - 97 ------ 1.000000
    3 - 98 ------ 1.000000
    3 - 99 ------ 1.000000
    8 - 0 ------ 1.000000
    8 - 1 ------ 1.000000
    8 - 2 ------ 0.200000
    8 - 3 ------ 1.000000
    8 - 9 ------ 0.200000
    8 - 11 ------ 0.166667
    8 - 12 ------ 0.166667
    8 - 15 ------ 1.000000
    8 - 88 ------ 0.800000
    8 - 97 ------ 1.000000
    8 - 98 ------ 0.833333
    8 - 99 ------ 1.000000
    9 - 0 ------ 1.000000
    9 - 1 ------ 1.000000
    9 - 2 ------ 0.500000
    9 - 3 ------ 1.000000
    9 - 8 ------ 0.200000
    9 - 11 ------ 0.666667
    9 - 12 ------ 0.666667
    9 - 15 ------ 1.000000
    9 - 88 ------ 1.000000
    9 - 97 ------ 1.000000
    9 - 98 ------ 1.000000
    9 - 99 ------ 1.000000
    11 - 0 ------ 1.000000
    11 - 1 ------ 1.000000
    11 - 2 ------ 0.666667
    11 - 3 ------ 1.000000
    11 - 8 ------ 0.166667
    11 - 9 ------ 0.666667
    11 - 12 ------ 0.750000
    11 - 15 ------ 1.000000
    11 - 88 ------ 1.000000
    11 - 97 ------ 0.750000
    11 - 98 ------ 0.250000
    11 - 99 ------ 1.000000
    12 - 0 ------ 1.000000
    12 - 1 ------ 1.000000
    12 - 2 ------ 0.666667
    12 - 3 ------ 1.000000
    12 - 8 ------ 0.166667
    12 - 9 ------ 0.666667
    12 - 11 ------ 0.750000
    12 - 15 ------ 1.000000
    12 - 88 ------ 0.333333
    12 - 97 ------ 1.000000
    12 - 98 ------ 1.000000
    12 - 99 ------ 1.000000
    15 - 0 ------ 0.500000
    15 - 1 ------ 0.333333
    15 - 2 ------ 1.000000
    15 - 3 ------ 1.000000
    15 - 8 ------ 1.000000
    15 - 9 ------ 1.000000
    15 - 11 ------ 1.000000
    15 - 12 ------ 1.000000
    15 - 88 ------ 1.000000
    15 - 97 ------ 1.000000
    15 - 98 ------ 1.000000
    15 - 99 ------ 1.000000
    88 - 0 ------ 1.000000
    88 - 1 ------ 1.000000
    88 - 2 ------ 1.000000
    88 - 3 ------ 1.000000
    88 - 8 ------ 0.800000
    88 - 9 ------ 1.000000
    88 - 11 ------ 1.000000
    88 - 12 ------ 0.333333
    88 - 15 ------ 1.000000
    88 - 97 ------ 1.000000
    88 - 98 ------ 1.000000
    88 - 99 ------ 1.000000
    97 - 0 ------ 1.000000
    97 - 1 ------ 1.000000
    97 - 2 ------ 1.000000
    97 - 3 ------ 1.000000
    97 - 8 ------ 1.000000
    97 - 9 ------ 1.000000
    97 - 11 ------ 0.750000
    97 - 12 ------ 1.000000
    97 - 15 ------ 1.000000
    97 - 88 ------ 1.000000
    97 - 98 ------ 0.250000
    97 - 99 ------ 0.333333
    98 - 0 ------ 1.000000
    98 - 1 ------ 1.000000
    98 - 2 ------ 1.000000
    98 - 3 ------ 1.000000
    98 - 8 ------ 0.833333
    98 - 9 ------ 1.000000
    98 - 11 ------ 0.250000
    98 - 12 ------ 1.000000
    98 - 15 ------ 1.000000
    98 - 88 ------ 1.000000
    98 - 97 ------ 0.250000
    98 - 99 ------ 0.666667
    99 - 0 ------ 1.000000
    99 - 1 ------ 1.000000
    99 - 2 ------ 1.000000
    99 - 3 ------ 1.000000
    99 - 8 ------ 1.000000
    99 - 9 ------ 1.000000
    99 - 11 ------ 1.000000
    99 - 12 ------ 1.000000
    99 - 15 ------ 1.000000
    99 - 88 ------ 1.000000
    99 - 97 ------ 0.333333
    99 - 98 ------ 0.666667

    Coefficiente di amicizia tra 2 e 11: 0.731707
    2 - 8 ------ 0.200000
    8 - 11 ------ 0.166667
```

[^0]: L'implementazione di tale algoritmo nel progetto non è completamente funzionante.