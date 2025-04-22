# Sources
[video di un tizio](https://www.youtube.com/watch?v=8SLRMa50tmc) dice che la decompressione non puo' essere parallelizzata, io non so se si riferisce a pzip in particolare o se si riferisce alla possibilita' generica di decomprimere in parallelo (spero sia la seconda)

[PIGZ](https://zlib.net/pigz/) (pronunciato 'pig-zee') e' la versione parallela fatta da qualcuno di forte, potrei confrontarla con la mia se fosse presente su cluster

[Source](https://github.com/madler/pigz/blob/master/pigz.c), line 282 the explanation of how this thing works

[Deflate](https://zlib.net/feldspar.html). almeno per saere cosa stiamo facendo

# Stuff in general

tied untied nella generazione dei task non sembra fare differenza, ma s un thread dovesse morire meglio averlo untied

provando varie posizioni per creare i file nella cosetta ricorsiva ho visto che il migliore e' metterlo sono al momento in cui fai il task al 100%