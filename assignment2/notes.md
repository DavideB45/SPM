# Analisi teorica
grafichino carino fatto con le mie mani (vedi disegnino)
poi fai qualche test per vedere quanti stepz richiede in un range fissato, puo' essere quello dell'esempio del prof per semplicita':
- in media per un range
- salvare anche il numero piu' grande perche' da il tempo massimo e l'upperbound
- salvare anche il totale numero di step che non so perche' ma puo' servire
- il tempo medio per uno step, tipo fai andare con quei range, hai il totale e il tempo totale pui fare una stima per capire quanto tempo dovrebbe richiedere se parallelizzato

# Analisi pratica

## sequenziale
nulla da dire, e' sequenziale
loop unrolling non ha migliorato

## multithread generico
per ogni versione aggiungere un'analisi dello scalamento forte

## static sceduling
Con il modo naive se i range sono molto piccoli gli ultimi thread non fanno nulla nella versione statica. Nella versione dinamica il problema non si pone.

bello, si possono portare miglioramenti, in quali casi aiutano questi miglioramenti?
sarebbero:
- lock free (forse da fare)
- miglior partizione (vedi Andrea) (complesso, molto situazionale, dovremmo sapere che tipo di split arrivano, perche' se come in questo caso non si sa allora dobbiamo usare lo cheduling dinamico)
- cosa che fa Angelo dicendo che l'ho vista da un amico e che non la rifaro' per non usare un lock
- se ad esempio avessimo sempre range piccoli potrebbe essere piu' conveniente dare un range ad ogni thread ad un certo punto

## dynamic
### dyn con lock
bello, migliorabile? il bottlneck sembrava la parte con le lock, quindi ho tolto le lock
### dyn lock free
figo, a volte vince, a volte perde
### possibili altre implementazioni
threadpool che permette di fare le cose facendo fare meno calcoli al nostro worker

## overall
qui devo fornire un confronto tra i vari algoritmi dicendo quando ciascuno e' preferibile agli altri analizzando i vari casi
- quanto costa spawnare i thread?
- tabella in cui per i 3 diversi task do i tempi migliori di ciascuna perhche' non ha senso confrontare tutti allo stesso modo
- 

# Mixed analysis
confrontare se la teoria aveva senso, non posso saperlo al momento
fare un calcolo teorico tipo con un numero particolare di thread per semplicita' usando la versione migliore che ho creato (non so quale sia)