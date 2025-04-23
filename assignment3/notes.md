# Sources
[video di un tizio](https://www.youtube.com/watch?v=8SLRMa50tmc) dice che la decompressione non puo' essere parallelizzata, io non so se si riferisce a pzip in particolare o se si riferisce alla possibilita' generica di decomprimere in parallelo (spero sia la seconda)

[PIGZ](https://zlib.net/pigz/) (pronunciato 'pig-zee') e' la versione parallela fatta da qualcuno di forte, potrei confrontarla con la mia se fosse presente su cluster

[Source](https://github.com/madler/pigz/blob/master/pigz.c), line 282 the explanation of how this thing works

[Deflate](https://zlib.net/feldspar.html). almeno per saere cosa stiamo facendo

# studio del problema (file grossi/piccoli)
## File piccoli
se sono tanti la parallelizzazione e' abbastanza facile, basta darne uno per thread, se sono pochi (meno dei thread il programma e' probabilmente veloce) parallelizzare potrebbe risultare in solo un'introduzione di overhead
## File grossi
se abbiamo un solo file grosso le tecniche implementate per file piccoli sono 100 100 inutili, bisogna adottare un nuovo approccio, (link a pgzip e spiegazione della fattibilita' del problema ma lo scopo e' studiare la parallelizzabilita' del problema, inteserrante ma troppo complesso). Come lui divido in blocchi pero' poi uso un header personalizzato e un'estensione nuova per distinguarli a tempo di decompressione. ogni blocco e' compresso indipendentemente
## Problemi
come disinguere i file quando vogliamo decomprimerli?
beh possiamo usare bzip che sta per big zip

# Stuff in general

tied untied nella generazione dei task non sembra fare differenza, ma s un thread dovesse morire meglio averlo untied

provando varie posizioni per creare i file nella cosetta ricorsiva ho visto che il migliore e' metterlo sono al momento in cui fai il task al 100%

# TODO:
- [x] implement flag to change num thread
- [] distinguere file grossi da piccoli quando sono compressi (usare bzip e zip)
- [] vedere se effettivamente un file compresso e decompresso rimane uguale
- [] gestire la r di ricorsione (cioe' diattivarla se va disattivata)
- [] gestire file grossi da spezzettare
- [] definire benchmark
- [] fare misurazioni con numero di thread progressivo
- [] fare il report
- [] fare prima creazione dei task poi gestione (deve semplicemente essere una lista di stringhe)
- [] studio un po' fatto a caso sul pinnare i thread, potrebbe migliorare i file grossi?

# prima creare task poi gestirli
Questo viene fatto perche' se abbiamo molti thread il bottleneck potrebbe essere lo spawnatore di task, inoltre i task sono piu' lenti dei parallel for a detta del prof, quindi questo potrebbe migliorare le performance e la parallelizzabilita'.

Questo ha senso solo se abbiamo thanti piccoli file, per migliorare la gestione dei grossi file dobbiamo 


# Cose migliorabili che non ho fatto

provare a pinnare i thread ma faere piu' test

gestire in modo migliore la compresisone di file grossi nel senso che il parallel for funzionera' solo per file singoli perche' non c'e' tempo
richiederebbe una nuova analisi intera, quindi e' fatto solo per i file piccoli

provare a non usare sottogruppi di thread, perche' oversubscribe i processori e quindi a volte non va bene, (vale solo se i task sono gestiti in quel modo)