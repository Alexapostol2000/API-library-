#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

int main(int argc, char * argv[]) {
  char * message;
  char * response;
  char * payload;
  int sockfd, login = 0, acces = 0;
  char * host = "ec2-3-8-116-10.eu-west-2.compute.amazonaws.com";
  char buffer[BUFLEN];
  char ** cookies_data = malloc(2 * sizeof(char * ));
  cookies_data[0] = malloc(100 * sizeof(char));
  char ** token_data = malloc(2 * sizeof(char * ));
  token_data[0] = malloc(100 * sizeof(char));
  char ** data = malloc(2 * sizeof(char * ));
  data[0] = malloc(100 * sizeof(char));
  data[1] = malloc(100 * sizeof(char));
  char* ip_adrr = malloc(100*sizeof(char));

// Request dns pentru a obtine ip serverului
struct addrinfo *result;

	getaddrinfo(host, NULL, NULL, &result);
	
	while (result != NULL) {
		if (result->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *) (result->ai_addr);
			strcpy(ip_adrr,inet_ntoa(ipv4->sin_addr));
		} 
		result = result->ai_next;
	}
		freeaddrinfo(result);

  while (1) {
    // deschid o conexiune cu serverul
    sockfd = open_connection(ip_adrr, 8080, AF_INET, SOCK_STREAM, 0);
    // citesc comanda de la tastatura
    memset(buffer, 0, BUFLEN);
    fgets(buffer, BUFLEN - 1, stdin);

    // register command
    if (strncmp(buffer, "register", 8) == 0) {
      // daca sunt logat nu permit inregistrarea
      if (login == 1) {
        printf("Delogheazate inainte de a face un nou cont\n");
        printf("\n");
        continue;
      }
      // citesc datele si pregatesc payloadul pentru mesaj
      char ** data2 = malloc(2 * sizeof(char * ));
      data2[0] = malloc(100 * sizeof(char));
      data2[1] = malloc(100 * sizeof(char));
      char * username = malloc(100 * sizeof(char));
      char * password = malloc(100 * sizeof(char));
      printf("Introduceti username-ul: ");
      scanf("%s", username);
      printf("Introduceti parola: ");
      scanf("%s", password);
      sprintf(data2[0], "username=%s", username);
      sprintf(data2[1], "password=%s", password);
      // compun mesajul cu ajutorul functiilor din request.c
      message = compute_post_request(host, "/api/v1/tema/auth/register", NULL, data2, 2, NULL, NULL);
      // trimit mesajul catre server
      send_to_server(sockfd, message);
      // primesc raspunsul de la server
      response = receive_from_server(sockfd);
      // extrag din rasouns doar mesajul json
      payload = basic_extract_json_response(response);
      // daca mesajul e gol registerul sa efecuat cu succes
      if (payload == NULL) {
        printf("Ai creat un cont nou cu succes\n");
      } else {
        printf("%s\n", payload);
      }

      free(username);
      free(password);
      free(data2[0]);
      free(data2[1]);
      free(data2);
      printf("\n");
      continue;
    }

    // daca e comanda de login
    if (strncmp(buffer, "login", 5) == 0) {
      // daca e deja logat nu i permite sa se mai logheze odata
      if (login == 1) {
        printf("Esti deja logat\n");
        printf("\n");
        continue;
      }
      //compun mesajul citesc de la tastatura credentialele
      char ** data3 = malloc(2 * sizeof(char * ));
      data3[0] = malloc(100 * sizeof(char));
      data3[1] = malloc(100 * sizeof(char));
      char * username = malloc(100 * sizeof(char));
      char * password = malloc(100 * sizeof(char));
      printf("Introduceti username-ul: ");
      scanf("%s", username);
      printf("Introduceti parola: ");
      scanf("%s", password);
      sprintf(data3[0], "username=%s", username);
      sprintf(data3[1], "password=%s", password);
      //compun mesajul pentru server
      message = compute_post_request(host, "/api/v1/tema/auth/login", NULL, data3, 2, NULL, NULL);
      //trimit mesajul catre server
      send_to_server(sockfd, message);
      //primesc raspunsul de la server
      response = receive_from_server(sockfd);
      // extrag raspusul json din raspuns
      payload = basic_extract_json_response(response);
      //daca paylodul este nul logarea a avut succes
      if (payload == NULL) {
        printf("Ai fost logat cu succes\n");
        // extrag cookie-ul din raspunsul de la server
        char * token = strtok(response, "\n");
        char * word = "Cookie";
        while (token) {
          if (strstr(token, word) != NULL) {
            break;
          }
          token = strtok(NULL, "\n");
        }
        char * token2;
        token2 = strtok(token, " ");
        token2 = strtok(NULL, " ");
        // salvez cookie-ul intr-o variabila pentru al utiliza mai tarziu
        strcpy(cookies_data[0], token2);
        login = 1;
      } 
      // eroare afisez mesajul json
      else {
        printf("%s\n", payload);
      }
      free(username);
      free(password);
      free(data3[0]);
      free(data3[1]);
      free(data3);
      printf("\n");
      continue;
      
    }

    // functia care da acces la biblioteca
    if (strncmp(buffer, "enter_library", 13) == 0) {
      //verific daca am deja acces
      if (acces == 0) {
          //verific daca sunt logat
        if (login == 1) {
            // fac cerere de autorizare
          message = compute_get_request(host, "/api/v1/tema/library/access", NULL, cookies_data, NULL);
          send_to_server(sockfd, message);
          response = receive_from_server(sockfd);
          payload = basic_extract_json_response(response);
          char * token;
          //daca mesajul json de la server e gol am primit autorizare
          if (payload != NULL) {
            printf("Ai primit acces la biblioteca\n");
            token = strtok(payload, ":");
            token = strtok(NULL, "\"");
          }
          strcpy(token_data[0], token);
        } else {
          printf("Cerere refuzata nu esti logat\n");
        }
      } else {
        printf("Ai deja acces in biblioteca");
      }
      printf("\n");
      continue;
    }

    // functie care extrage toate cartile din biblioteca
    if (strncmp(buffer, "get_books", 9) == 0) {
      //verific daca sunt logat
      if (login == 1) {
          //trimit cerere catre server
        message = compute_get_request(host, "/api/v1/tema/library/books", NULL, cookies_data, token_data);
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
        payload = basic_extract_json_response(response);
        // daca mesajul json e gol nu avem carti in biblioteca altfel afiseaza cartile sub forma de lista
        if (payload == NULL) {
          printf("Nu se afla nici o carte in biblioteca\n");
        } else {
          printf("%s\n", payload);
        }
      } else {
        printf("Nu esti logat\n");
      }
      printf("\n");
      continue;
    }

        // functie care returneaza o carte in biblioteca dupa id
    if (strncmp(buffer, "get_book", 8) == 0) {
      if (login == 1) {
        // primesc id cartii si pregatesc mesajul cu id respectiv
        int index;
        printf("Introduceti id cartii: ");
        scanf("%d", & index);
        char * url;
        url = malloc(100 * sizeof(char));
        sprintf(url, "/api/v1/tema/library/books/%d", index);
        message = compute_get_request(host, url, NULL, cookies_data, token_data);
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
        payload = basic_extract_json_response(response);
        // afisez mesajul json care poate fi eroare sau cartea respectiva
        printf("%s\n", payload);
        free(url);
      } else {
        printf("Nu esti logat\n");
      }
      printf("\n");
      continue;
    }

    // functie de stergere a unei carti dupa id
    if (strncmp(buffer, "delete_book", 11) == 0) {
      // verific daca sunt logat
      if (login == 1) {
        // citesc indexul si pregatesc mesajul pentru server
        int index;
        printf("Introduceti id cartii: ");
        scanf("%d", & index);
        char * url;
        url = malloc(100 * sizeof(char));
        sprintf(url, "/api/v1/tema/library/books/%d", index);
        message = compute_delete_request(host, url, NULL, cookies_data, token_data);
        send_to_server(sockfd, message);
        response = receive_from_server(sockfd);
        payload = basic_extract_json_response(response);
        // daca payloadul e null cartea a fost stearsa cu succes altfel afiseaza mesajul json
        if (payload != NULL) {
          printf("%s\n", payload);
        } else {
          printf("Carte stearsa cu succes\n");
        }
        free(url);
      } else {
        printf("Nu esti logat\n");
      }
      printf("\n");
      continue;
    }

    // functie de adaugare a unei carti
    if (strncmp(buffer, "add_book", 8) == 0) {
        // verific daca sunt logat
      if (login == 0) {
        printf("Nu esti logat\n");
        printf("\n");
        continue;
      }
      // citesc informatiile necesare
      char * title = malloc(100 * sizeof(char));
      char * author = malloc(100 * sizeof(char));
      char * genre = malloc(100 * sizeof(char));
      int page_count;
      char * publisher = malloc(100 * sizeof(char));
      printf("Introduceti titlul: ");
      scanf("%s", title);
      printf("Introduceti autorul: ");
      scanf("%s", author);
      printf("Introduceti genul: ");
      scanf("%s", genre);
      printf("Introduceti numarul de pagini: ");
      scanf("%d", & page_count);
      printf("Introduceti publicatia: ");
      scanf("%s", publisher);

      char ** data1 = malloc(5 * sizeof(char * ));
      data1[0] = malloc(100 * sizeof(char));
      data1[1] = malloc(100 * sizeof(char));
      data1[2] = malloc(100 * sizeof(char));
      data1[3] = malloc(100 * sizeof(char));
      data1[4] = malloc(100 * sizeof(char));
      // creez mesajul cu continutl pentru adaugarea unei carti
      sprintf(data1[0], "title=%s", title);
      sprintf(data1[1], "author=%s", author);
      sprintf(data1[2], "genre=%s", genre);
      sprintf(data1[3], "page_count=%d", page_count);
      sprintf(data1[4], "publisher=%s", publisher);
        // compun mesaj pentru server
      message = compute_post_request(host, "/api/v1/tema/library/books", NULL, data1, 5, cookies_data, token_data);
      send_to_server(sockfd, message);
      response = receive_from_server(sockfd);
      payload = basic_extract_json_response(response);
      // daca raspunsul json este null a fost adaugata cu succes altfel afiseaza raspunsul json
      if (payload != NULL) {
        printf("%s\n", payload);
      } else {
        printf("Adaugata cu succes\n");
      }

      free(title);
      free(author);
      free(genre);
      free(publisher);
      free(data1[0]);
      free(data1[1]);
      free(data1[2]);
      free(data1[3]);
      free(data1[4]);
      free(data1);
      printf("\n");
      continue;
    }

    // functie de logout
    if (strncmp(buffer, "logout", 6) == 0) {
      // compune mesaj de logout pentru server
      message = compute_get_request(host, "/api/v1/tema/auth/logout", NULL, cookies_data, NULL);
      send_to_server(sockfd, message);
      response = receive_from_server(sockfd);
      login = 0;
      acces = 0;
      payload = basic_extract_json_response(response);
      // daca raspunsul json e null delogarea a avut succes ster cookie-ul si token-ul de la autentificare anterioara
      if (payload == NULL) {
        printf("Te-ai delogat cu succes\n");
        cookies_data[0][0] = '\0';
        token_data[0][0] = '\0';
      } else {
        printf("%s\n", payload);
      }
      printf("\n");
      continue;
    }

    // Daca primeste exit de la tastatura inchide clientulu
    if (strncmp(buffer, "exit", 4) == 0) {
      printf("Clientul sa inchis\n");
      return 0;
    }
  }
  // eliberez memoria utilizata in program
  free(cookies_data[0]);
  free(cookies_data);
  free(token_data[0]);
  free(token_data);
  free(data[0]);
  free(data[1]);
  free(data);
  free(ip_adrr);
}
