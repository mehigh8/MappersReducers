<b>Rosu Mihai Cosmin 333CA</b>

<b>main:</b>
- Programul incepe prin preluarea din linia de comanda a numarului de mapperi,
numarului de reduceri si fisierului ce contine caile fisierelor ce trebuie
procesate.
- Din fisierul cu cai se retin caile si numarul de fisiere ce trebuie procesate.
- Apoi, programul aloca memorie pentru datele ce vor fi necesare thread-urilor,
creeaza o bariera cu dimensiune egala cu numarul de mapperi plus numarul de
reduceri si un mutex, iar apoi creeaza thread-urile.
- Fiecare thread primeste functia corespunzatoare dupa tipul acestuia (mapperii
primeste mapper_func, iar reducerii primesc reducer_func).
- Dupa aceea, programul apeleaza functia de join pentru a astepta terminarea
tuturor thread-urilor.
- In final se distrug bariera si mutex-ul si este eliberata memoria.

<b>mapper:</b>
- Fiecare mapper incepe prin a prelua datele primite ca argument in functie.
- Apoi, fiecare trebuie sa aleaga cate un fisier din cele neprocesate. Acest
lucru are loc prin preluarea fisierului curent (dupa indice) din array-ul de
fisiere, si incrementarea indicelui fisierului curent pentru urmatorul mapper.
Aceasta preluare are loc intr-o zona critica, delimitata de mutex-ul
"pick_file", si de asemenea se afla intr-un while, intrucat pot fi mai multe
fisiere decat mapperi. Acest mod de alegere a fisierelor este optim deoarece
un fisier mai mic va avea nevoie de un timp mai scurt de procesare, iar un
fisier mai larg va avea nevoie de un timp mai lung. Asadar, un mapper care isi
termina de procesat fisierul, va alege altul. Astfel, cantitatea de lucru este
impartita optim mapperilor.
- Odata ce mapper-ul are un fisier ales, incepe sa verifice numerele. Pentru
fiecare putere verifica daca numarul curent este putere perfecta. Pentru a
verifica daca un numar este putere perfecta am cautat binar daca exista o
baza b, astfel incat b la puterea p sa fie n, unde n este numarul ce este in
procesul de verificare si p este puterea pentru care se verifica. Fiecare
element ce este putere perfecta este retinut in lista corespunzatoare din
mappers_lists. mappers_lists este o lista de liste de liste, deoarece avem o
lista pentru fiecare mapper, fiecare mapper are o lista de puteri, iar fiecare
putere are o lista de numere.
- Dupa ce mapper-ul termina cu fisierul ales, incearca sa aleaga alt fisier.
Daca nu mai sunt fisiere de procesat, acesta asteapta la bariera inainte sa 
termine executia, pentru ca reducerii sa poate incepe cand termina toti
mapperii.

<b>reducer:</b>
- Fiecare reducer incepe prin a prelua datele primite ca argument in functie si
creeaza fisierul in care va scrie raspunsul.
- Apoi, asteapta la bariera pana cand termina toti mapperii.
- Dupa ce termina mapperii, reducerii pot incepe procesarea listelor. Fiecare
reducer corespunde unei puteri. Asadar, incepe prin a numara cate numere au
fost retinute in lista corespunzatoare puterii lui, in listele mapperilor, dupa
care copiaza numerele intr-un array. Odata ce are array-ul reducer-ul mai are
doar de eliminat duplicatele. Pentru a face asta, reducer-ul sorteaza array-ul
deoarece, odata sortat, array-ul va avea duplicatele grupate unul langa
celalalt. Dupa sortare, array-ul este parcurs, si pentru fiecare element care
este diferit de vecini incrementez numarul final ce trebuie scris in fisier.
- In final numarul gasit este scris, se elibereaza memoria array-ului si
reducer-ul isi incheie executia.
