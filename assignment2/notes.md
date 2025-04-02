# Cose provate
loop unrolling
fare una tabella in cui salvo alcuni risultati

# cose che non sono state misurate
Con il modo naive se i range sono molto piccoli gli ultimi thread non fanno nulla nella versione statica. Nella versione dinamica il problema non si pone.

Nella versione statica la policy migliore dipende da come ci aspettiamo che arrivino i dati. Provare versione con:
- un range per ogni thread
- provare con diverse blocksize per range piccoli
- assegnamento fatto bene (dovrebbe essere il migliore ma aggiunge un po' di overhead)
- in alteranativa al sopra uno che usa il modulo num thread del numero non del thread (un po' piu' randomica)

Provare anche ad usare una threadpool