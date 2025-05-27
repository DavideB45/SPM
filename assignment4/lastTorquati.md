# Last part that is 

A comunicator is a mix of a gorup and a context. To define a new comunicator we aaa

we can define groups with api

the comunicator communicate only with the programs of the groups; teh numbers are local, so a local root different from the global root, can select whatever, the first 10, the odd one.
quelli che usano lo stesso colore fanno parte dello stesso colore. The new communicator can be used 

## Derived data types
in complex applicatino we can have complex structures. The problem is how to send data that can be non continuos, and also avoid send useless data (maybe just part of the data). We can define a new datatype with MPI and we can use that in standard MPI; the hard stuff is left to MPI, that will ensure the efficiency.

MPI_Datatype t_name;

MPI_Type_\[create]_\[contiguos|vector|struct]();

MPI_Type_commit();

MPI_Type_?delete?();


**Structure**

A volte il compilatore assegna piu' cose di quelle che servono, quindi devi fare attenzione, soprattutto per la portabilita', definire il tipo.
L'esempio non e' perfetto perche' anche il primo potrebbe non partire da zero. E' un sacco difficile devi usare delle funzioni di MPI per capire dove iniziano e finiscono i pezzi della struct


**SUMMA**

Esempio di come si fa la moltiplicazione tra matrici, e' molto difficile, pero' se usi i communicators e' un po' piu' facile.

## Hybrid programming

A farm of farm. Peak of parallelization (spmcode12)

di deafult non e' thread safe, provides 4 livelli di safety:
- base usato fin'ora, quando comunichi i thread devono fermarsi
- thread funneled: puoi usare chiamate solo dal master thread
- serialized multiple thread ma uno alla volta
- multple (most overhead) can do whatever

i dati non arrivano ai thread ma solo ad un nodo in generale

morto (il puntatore)

Eh, edvi controllare se si puo' fare