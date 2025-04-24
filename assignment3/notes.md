# Sources
[video di un tizio](https://www.youtube.com/watch?v=8SLRMa50tmc) dice che la decompressione non puo' essere parallelizzata, io non so se si riferisce a pzip in particolare o se si riferisce alla possibilita' generica di decomprimere in parallelo (spero sia la seconda)

[PIGZ](https://zlib.net/pigz/) (pronunciato 'pig-zee') e' la versione parallela fatta da qualcuno di forte, potrei confrontarla con la mia se fosse presente su cluster

[Source](https://github.com/madler/pigz/blob/master/pigz.c), line 282 the explanation of how this thing works

[Deflate](https://zlib.net/feldspar.html). almeno per sapere cosa stiamo facendo

# studio del problema (file grossi/piccoli)
## File piccoli
se sono tanti la parallelizzazione e' abbastanza facile, basta darne uno per thread, se sono pochi (meno dei thread il programma e' probabilmente veloce) parallelizzare potrebbe risultare in solo un'introduzione di overhead
## File grossi
se abbiamo un solo file grosso le tecniche implementate per file piccoli sono 100 100 inutili, bisogna adottare un nuovo approccio, (link a pgzip e spiegazione della fattibilita' del problema ma lo scopo e' studiare la parallelizzabilita' del problema, inteserrante ma troppo complesso). Come lui divido in blocchi pero' poi uso un header personalizzato e un'estensione nuova per distinguarli a tempo di decompressione. ogni blocco e' compresso indipendentemente
## Problemi
come disinguere i file quando vogliamo decomprimerli?
beh possiamo usare bzip che sta per big zip

# sequenziale
poche modifiche. Parlare del fatto che adesso usa i blocchi

# parallelo (con task)
una descrizione di cosa ho fatto

tied untied nella generazione dei task non sembra fare differenza, ma s un thread dovesse morire meglio averlo untied

provando varie posizioni per creare i file nella cosetta ricorsiva ho visto che il migliore e' metterlo sono al momento in cui fai il task al 100%

per i file grossi la scelta e' di fare prima le cose e poi unirle, un'alternativa potrebbe essere usare del nested parallelism (parallel for) e fargli scrivere nel file in modo ordinato. Non e' stato fatto perche' sarebbe difficile decidere il numero di thread da utilizzare nel for e poi ci sarebbe sicuramente dell'overhead, quindi non e' immediato dire che quella soluzione sarebbe la migliore. Poi non voglio gestire il nested parallelism.

# furder tests
prove con il parallel for
Questo viene fatto perche' se abbiamo molti thread il bottleneck potrebbe essere lo spawnatore di task, inoltre i task sono piu' lenti dei parallel for a detta del prof, quindi questo potrebbe migliorare le performance e la parallelizzabilita'.
Questo ha senso solo se abbiamo thanti piccoli file, per migliorare la gestione dei grossi file dobbiamo gestire le cose in modo piu' complicato, e forse a quel punto non ha senso l'utilizzo di OMP ma ha piu' senso una gestione personalizzata delle code oppure fastflow

[Tabella con tempi in base a scheduling statico dinamico, 1/2 selezioni di blocchi]

prove con thread pinning
Perche' stiamo gestendo file di dimensioni rilevanti, quindi avere tutto su un singolo socket, oppure due, aumentando le risorse totali, ma anche l'overhead per accedere alla memoria. La decisione non e' ovvia e richiede test per vedere se ha senso starci a perdere tempo.

[Tabella con tempi in base a posizionamento (usando i flag precucinati da OMP)]

questi testo sono stai fatti per vedere cosa influiva di piu' sulle performance, e decidere quale delle due strade debba essere seguita in caso di sviluppi futuri

# TODO:
- [x] implement flag to change num thread
- [x] distinguere file grossi da piccoli quando sono compressi (usare il primo valore letto)
- [x] mettere nel sequenziale la cosa a blocchi (basta a questo punto copiare la compressione senza pragma)
- [x] vedere se effettivamente un file compresso e decompresso rimane uguale
- [] gestire la r di ricorsione (cioe' diattivarla se va disattivata)
- [] fixare quella cosina...
- [x] gestire la D e la C se vogliamo rimuovere (era gia' nel codice del prof)
- [x] gestire file grossi da spezzettare
- [x] definire benchmark
- [x] fare misurazioni con numero di thread progressivo
- [] fare il report
- [x] fare prima creazione dei task poi gestione (deve semplicemente essere una lista di stringhe)
- [x] studio un po' fatto a caso sul pinnare i thread, potrebbe migliorare i file grossi?
- [] what about some efficiency plots?


# Cose migliorabili che non ho fatto

provare a pinnare i thread ma faere piu' test

gestire in modo migliore la compresisone di file grossi nel senso che il parallel for funzionera' solo per file singoli perche' non c'e' tempo
inoltre bisognerebbe creare un sacco di task, e quindi il porcesso non e' del tutto immediato,

una soluzione che potrebbe essere una via di mezzo e' gestire in coda i file grossi nello stesso modo in cui sono gestiti ora

un'altra cosa che potrebbe toccare l'analisi e' come si compota con pochi file 

la chunksize per splittare i file grossi e' stata scelta a occhio, era buona attorno a 1/2 MB