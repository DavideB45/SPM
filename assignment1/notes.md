# STUFF TO REMEMBER IN THE REPORT
## Introduzione
Presentare le tre versioni, dire qual'e' lo scopo dell'assignment, spiegare la divisione degli esperimenti (piccoli per vedere se una delle versioni introducesse dell'overhead significativo che perde di importanza con l'aumentare delle dimensioni del softmax)
## Speedup:
- steedy state
    - auto vs plain: 2.75
    - AVX  vs plain: 3.5
    - AVX  vs auto : 1.25
- altre osservazioni:
    - inizialmente è molto più veloce AVX poi perde un po', che sia colpa della chache? solo supposizioni
    - auto nella fase iniziale non è super stabile, nessuno sa il perché però poi si riprende


## Plots:
- small vector unstable, just see that avx is lower and stable
- big vector: insert log log scale plot or normal scale plot

## Code
### AVX
- disclaimer: servono almeno 8 elementi (per il primo loop, alternativa)
- ciclo principale
- gestione somma/massimo finale alla Torquati
- gestione massimo non multiplo di 8
- gestione chiusura con le maschere
- cose che non sono andate nella versione finale:
    - aligned memory
    - due tipi di maschere
### Plain
- credo nulla era tutto fornito dal prof
- spero non andasse ottimizzata, cioè non puoi dividere più velocemente...
### Auto
- provato un po' di flag che non facevano
- la matematica veloce ha risolto quasi tutto
- __restrict ha risolto quello che non andava
- aligned memory mistery

## Considerazioni finali
Ci siamo tutti divertiti un sacco, avevo paura che il mio avx scritto con il sangue andasse più lento che quello automaticamente fatto con il compilatore ma non è il caso.
Altra nota: i valori non sono uguali alla millesima cifra per errori fatti dalla macchina, ma c'è da notare che neanche softmax plain è perfetto, infatti se sommiamo tutti gli elementi contenuti in output vediamo che nessuna delle implementazioni è in grado di raggiungere 1; a volte ci si avvicina più uno a volte l'altro.
AVX è meglio ma è un casino, non mettere un flag che fa andare più veloce è follia.

## TODO
- [x] controllare che autovectorization abbia fatto tutto per davvero
- [x] controllare se allineare la memoria rallenta davvero
- [x] accettare meno di 8 elementi in AVX version
- [x] misurare avx contro auto
- [] Roofline model??