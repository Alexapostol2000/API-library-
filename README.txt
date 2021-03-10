Nume: Apostol Alexandru
Grupa: 324CD



            ===================================================================
            ======================= TEMA 3 ====================================
            =============== PROTOCOALE DE COMUNCATII ==========================
            ===================================================================


1.Request.c:
    -> fisierul contine 3 functii (compute_get_request ; compute_delete_request ; compute_post_request)
    -> compute_get_request(char * host, char * url, char * query_params,char ** cookies, char ** token)
        * compune un  mesaj de get_request pentru server la care adauga parametrii primiti
        * host(adresa serverului) ; url(destinatia din server unde sa se faca cererea);
          cookie (cookie-ul de sesiune primit la login) ; token (tokenul returnat in urma cereri de acces)
    -> compute_delete_request(char * host, char * url, char * query_params,char ** cookies, char ** token)
        * compune un mesaj de delete_request pentru server la care adauga parametrii primiti
        * host(adresa serverului) ; url(destinatia din server unde sa se faca cererea);
          cookie (cookie-ul de sesiune primit la login) ; token (tokenul returnat in urma cereri de acces)
    -> char * compute_post_request(char * host, char * url, char * content_type, char ** body_data,
    int body_data_fields_count, char ** cookies, char ** token)
        * compune un mesaj de post_request pentru server la care adauga parametrii primiti    
        * host(adresa serverului) ; url(destinatia din server unde sa se faca cererea);
          cookie (cookie-ul de sesiune primit la login) ; token (tokenul returnat in urma cereri de acces);
          body_data payloadul care trebuie adaugat in mesaj ; body_data_fields_count din cate "bucati" 
          e alcatuit payloadul;


2.Client.c:
    -> inaintea de while(1) fac un request dns pentru a afla adresa ip a serverului , pe care o retin pentru a deschide
    conexiunea cu serverul cu ajutorul ei;
    -> fisierul contine un ciclu infinit din care se primeste un input de la tastatura ;
    -> in functie de comanda primita (register,login,enter_library,get_books,get_book,delete_book,add_book,logout,exit)
        
        *register =  primeste de la tastatura un user si o parola compune payloadul (user = user ;  parola = parola)
        si trimite requestul catre server , intoarce un mesaj de succes sau mesajul json de eroare ; 
        
        *login = primeste de la tastatura un user si o parola compune payloadul (user = user ;  parola = parola)
        si trimite requestulde catre server, intoarce un mesaj de la server care contine un cookie de sesiune, 
        descmpun mesajul cu strtok pentru a lua cookie-ul pe care il salvez intr-o variabila data_cookie[0].
        Acesta va fi utilizat pana la comanda logout;
        
        *enter_library =  compune un mesaj de request pentru server , comanda returneaza un mesaj json cu un token in 
        cazul in care utilizatorul este conectat corect cu un cookie valid, cu ajutorul strtok iau token-ul pe care
        il retin intr-o variabila data_token[0]. Acesta ramane valabil pana la comanda logout cand este sters;

        *get_books = compune un mesaj de request pentru server , comanda se poate executa doar daca suntem conectati
        la biblioteca cu un token jwt. In cazul in care nu exista carti se afiseaza un mesaj specific, daca exista se 
        afiseaza o lista cu acestea. In caz de eroare este afisat mesajul json primit , sau un mesaj specific ca 
        utilizatorul nu este logat;

        *get_book = compune un mesaj de request pentru server , comanda se poate executa doar daca suntem conectati
        la biblioteca cu un token jwt. Se afiseaza mesajul json primit de la server, care poate fi ori eroare sau 
        un mesaj specific cand cartea nu exista, ori detaliile cartii respective;

        *delete_book = compune un mesaj de request pentru server , comanda se poate executa doar daca suntem conectati
        la biblioteca cu un token jwt. Se afiseaza mesajul json primit de la server, care poate fi ori eroare sau 
        un mesaj specific cand cartea a fost stearsa;

        *add_book = compune un mesaj de request pentru server cu un index primit de la tastatura, comanda se poate 
        executa doar daca suntem conectati la biblioteca cu un token jwt. In cazul in care cartea a fost stearsa
        mesajul json este null, in caz contrar se afiseza mesajul json;

        *logout = compune un mesaj de request pentru server de logout, se initilizeaza loginul si acees cu 0,
        daca raspunsul json de la server este null a fost delogat cu succes ster cookie-ul si tokenul de sesiune, 
        altfel se afiseaza mesajul json;

        *exit = inchide clientul ;
    -> la sfarsitul ciclului eliberez toata memoria alocata;


Concluzii : 
    *Implementarea temei a fost una destul de usoara, insa interactiunea cu un server extern este mult mai 
    complicata. Tratarea cazurilor de eroare este foarte complicata, fiind foarte multe posibilitati prin care aplicatia
    sa ajunga intr-o stare nedefinita;
    *Lucrul cu memoria a fos un alt factor delicat in rezolvarea temei, aparand foarte multe seg foult din cauza 
    lucrului deficitar cu memoria;
    *Tratarea erorilor a fost facuta cu ajutorul mesajelor json de la server dar si cu mesaje proprii;
    *Comenzie au un format bine stabilit, respecta cerinta;
    *Cerinta nu a fost foarte explicita lasand loc de interpretari in anumite cazuri;
    *Tema contine implementarea tuturor functionalitatilor din enunt
    *A fost o tema super interesanta la fel ca si celalte teme de la aceasta materie;
    *Tema ar fi aratat mult mai bine daca primeam ca argument numele serverului insa nu am facut asta deoarece nu stiam 
    daca am voie;
