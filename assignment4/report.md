# REPORT

## General consideration:
the task seem to be again memory bound, indeed, if we use the inplace merge sort instead of the merge sort we gain some time

## Versions

### FastFlow
#### considerazioni
struttura del task
struttura del codice
step di ottimizzazione della memoria:
inplace ms
eliminare task
#### Todo
- [ ] far lavorare l'emitter (fai la cosa che ha detto Andrea)
- [x] emettere quando pronti (non ha funzionato molto bene, e l'implementazion risultava molto complessa quindi il progetto e' stato abbandonato. Magari funziona meglio quando stiamo su altre dimensioni con i numeri o con i thread. In ogni caso fare un po' di test per confermare)
- [ ] recicle memory
- [ ] leggere da un file
- [ ] cambiare definizione del numero di thread

### MPI
Scegliere il problema, vogliamo partire da un nodo? vogliamo partire da un file?

#### Todo
- [ ] leggere da un file
- [ ] passare sempre un payload, poi per non passarlo basta lasciarlo vuoto ed e' come aver passato solo i puntatori 

## TEST
massimo un nodo e lo tieni buono per gli altri