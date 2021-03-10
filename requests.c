#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

// compunearea mesajului de tip get
char * compute_get_request(char * host, char * url, char * query_params,
  char ** cookies, char ** token) {
  char * message = calloc(BUFLEN, sizeof(char));
  char * line = calloc(LINELEN, sizeof(char));

  if (query_params != NULL) {
    sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "GET %s HTTP/1.1", url);
  }

  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);
    // daca avem token il adaugam in mesaj
  if (token != NULL) {
    sprintf(line, "Authorization: Bearer %s", token[0]);
    compute_message(message, line);
  }
    // daca avem cookie il adaugam in mesaj
  if (cookies != NULL) {
    sprintf(line, "Cookie: %s", cookies[0]);
    compute_message(message, line);
  }

  compute_message(message, "");
  return message;
}

 //compune mesaj de tip delete
char * compute_delete_request(char * host, char * url, char * query_params,
  char ** cookies, char ** token) {
  char * message = calloc(BUFLEN, sizeof(char));
  char * line = calloc(LINELEN, sizeof(char));

  if (query_params != NULL) {
    sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
  } else {
    sprintf(line, "DELETE %s HTTP/1.1", url);
  }

  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);
    // daca avem token il adaugam in mesaj
  if (token != NULL) {
    sprintf(line, "Authorization: Bearer %s", token[0]);
    compute_message(message, line);
  }
    // daca avem cookie il adaugam in mesaj
  if (cookies != NULL) {
    sprintf(line, "Cookie: %s", cookies[0]);
    compute_message(message, line);
  }

  compute_message(message, "");
  return message;
}

    // compune un mesaj de tip post
char * compute_post_request(char * host, char * url, char * content_type, char ** body_data,
  int body_data_fields_count, char ** cookies, char ** token) {
  char * message = calloc(BUFLEN, sizeof(char));
  char * line = calloc(LINELEN, sizeof(char));
  char * body_data_buffer = calloc(LINELEN, sizeof(char));

  sprintf(line, "POST %s HTTP/1.1", url);
  compute_message(message, line);

  sprintf(line, "Host: %s", host);
  compute_message(message, line);

    // adauga payload daca avem 
  compute_message(message, "Content-Type: application/x-www-form-urlencoded");
  for (int i = 0; i < body_data_fields_count; i++) {
    strcat(body_data_buffer, body_data[i]);
    if (i != body_data_fields_count - 1) {
      strcat(body_data_buffer, "&");
    }
  }

  sprintf(line, "Content-Length: %ld", strlen(body_data_buffer));
  compute_message(message, line);

    // daca avem token il adaugam in mesaj
  if (token != NULL) {
    sprintf(line, "Authorization: Bearer %s", token[0]);
    compute_message(message, line);
  }

    // daca avem cookie il adaugam in mesaj
  if (cookies != NULL) {
    sprintf(line, "Cookie: %s", cookies[0]);
    compute_message(message, line);
  }

  compute_message(message, "");

  memset(line, 0, LINELEN);
  compute_message(message, body_data_buffer);

  free(line);
  return message;
}
