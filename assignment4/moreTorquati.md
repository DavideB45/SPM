# Double buffering farm
Farm seleton ->
intuitively we use more memory to overlap computation and comunication.
Emitter sends on demand, the worker meanwhile works on other stuff. Many buffer overall, but need to be carefull since we can't use buffer when sending/receiving.

Quando usi i thread non puoi usare semplicemente MPI_init ma devi usare **MIP_Init_thread** . This way MPI runtime knows that there are thread and lock are used or other stuff.

## Implementazione
- Il worker va avanti finche' non riceve un end of stream. Se riceve qualcosa, richiede un altro task con `send_redy`; controlli quanti byte hai ricevuto (l'ultimo messaggio potrebbe avere meno cose), estrai i taske li esegui uno alla volta e quando ne finisci uno copi nel send buffer pero' per farlo devi essere sicuro che la **send precedente sia finita** (questo viene fatto qualche riga prima, fuori dal `for`). Dopo il for swappi i buffer cosi' usi cio' che e' gia stato finito => abbiamo contemporaneamente send/receive/computation
- the collector has 2 buffer in input to 
aspetta un messaggio e posta una receive sullo stesso buffer, gathera quello ceh ha ricevuto, controlla se ha finito. Essenzialmente riceve da qualsiasi fonte che abbia un *result tag*, riposta una receive facendo qualcosa locally
- scheduler, e' un po' piu' compicato. MOlta extra memoria, 2 buffer per worker (non fa nulla, quindi non c'e' nulla da overlappare, pero' e' un esempio...) Always clean the buffer after aver finito per bene

Few things to notice, the MPI program is quite complicated, for this reason it is nice to have some precooked pattern. This is the reason to promote parallel pattern way of programming.

# Something more about MPI
Collectives comunication
MPI provides many of this, that are efficiently implemented on different platform/networks. Many have different implementation based on the architecture of the network, for this it is always a good idea to try to use them.

What is a collective? all the processes of the group should call the same MPI function. The most straightforward one is the MPI_Barrier, there is also a non blocking one. 

### Broadcast
mandi un messaggio a tutti quelli nel comunicator. Anche esso stesso lo riceve, mentre con il multicast non lo riceve se stesso. (ovviamente se stesso e' fake). Il multicast non e' disponibile, devi fare un comunicator, that contains a subset of processes. The message may cost a lot.

### Reduction
sappiamo cos'e' una riduzione. Tutti la devono chiamare, atrimenti non tremina. ci sono delle operazioni predefinite classiche che si possono usare, sono abbastanza standard, poi ci sono minlock e maxloc che va su coppie, puoi tipo trovare massimo. maxloc.cpp trova il massimo di un array in un grosso array.
Particolare, solo uno la ha all'inizio se la vuoi per tutti devi fare una chiamata diversa

### Scatter & Gather
mandi i pezzetti ai fratm. ScatterV i dati possono essere splittati arbitrariamente. The source is splitting the data

La gather riprende le cose indietro (il receive buffer deve avere abbastanza spazio)

quelle che finiscono con *V* sono diverse.
poi c'e' anche la versione asincrona che e' quella con la $I$

ci sono anche le versioni con all che poi ti fanno avere tutto a tutti con allgather ed allgatherv. It is equivalent to gahter + broadcast, ma e' un errore, perhe' e' meno efficiente.
All reduce, stessa cosa, logicamente recuce + broadcast

## Example
poca comunicazione ma usa tanta memoria

tanta comunicazione ma poca memoria

devi scegliere in base a quello che hai