# REPORT

## General consideration:
the task seem to be again memory bound, indeed, if we use the inplace merge sort instead of the merge sort we gain some time

## Versions

### FastFlow
descrizione concettuale dello splitting di come dovrebbe essere
descrizione della prima versinoe naive 
descrizione dei 2 problemi che presenta questa versione
soluzione al primo (togliere il blocco)
soluzione al secondo (usare attivamente l'emitter (solo efficienza??)) (queue size decided with very few test on personal computer)
#### considerazioni
struttura del task
struttura del codice
step di ottimizzazione della memoria:
inplace ms
eliminare task
#### Todo
- [x] far lavorare l'emitter (fai la cosa che ha detto Andrea)
- [x] emettere quando pronti (non ha funzionato molto bene, e l'implementazion risultava molto complessa quindi il progetto e' stato abbandonato. Magari funziona meglio quando stiamo su altre dimensioni con i numeri o con i thread. In ogni caso fare un po' di test per confermare)
- [x] recicle memory
- [ ] leggere da un file??
- [x] cambiare definizione del numero di thread

### MPI
Scegliere il problema, vogliamo partire da un nodo? vogliamo partire da un file?
di sicuro se il problema e' scrivere su un file o avere l'array ordinato da qualche parte non importa perche' comunque la fase di merge richiede avere l'array in un solo nodo (cercare paper che confuta questa ipotesi)

discutere il fatto che sia possibile anche fare una versione in cui la comunicazione non e' logaritmica
2 versioni:
- che non passa il payload
- che passa il payload

#### Todo
- [ ] leggere da un file
- [ ] passare sempre un payload, poi per non passarlo basta lasciarlo vuoto ed e' come aver passato solo i puntatori 

## TEST
massimo un nodo e lo tieni buono per gli altri
spiegare perche' solo un piload di uno e' stato usato nella versione ff
### Fast flow
strong scaling
weak scaling
efficiency

### MPI

lost stability (results may be 'different')
strong scaling (2 dimensioni diverse)
comparison con 16 thread (che sembra il numero migliore in media)
comparison sequenziale vs sequenziale

ricreazione della farm ogni volta

double buffering has a fatal flow, if we do multiple round we'll end up with multiple arrays to merge and the merge can be quite costly

provare con piu' nodi (job) (non sembra andare)
(siamo limitati dalla rete)
altra parte lenta il merge finale che e' poco parallelizzabile

## Future improvements
more test are needed to check the oversubription of the processors in ff with blocking queue
thread pinning, can be checked although no significant performance improvements is expected, given how ff heuristic

possibile miglioramento per MPI mettendo i merge dei pezzetti prima (mentre riecevi gli altri)

Il  double buffering forse non va perche' il successivo merge richiede di fare piu' lavoro (un'altra scan dei dati) che non c'e' se unsiamo solamente un buffer

## Referece to docs to motivate
[Finalize](https://www.mpich.org/static/docs/v3.1/www3/MPI_Finalize.html)

[Receive](https://docs.open-mpi.org/en/main/man-openmpi/man3/MPI_Recv.3.html)

[Scatter](https://www.open-mpi.org/doc/v3.1/man3/MPI_Scatter.3.php)

[Quelli che ce l'hanno fatta](https://arxiv.org/pdf/2003.01216)