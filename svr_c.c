#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "svr_c.h"
#include <sys/socket.h>    //socket
#include <arpa/inet.h> //inet_addr

atm_message* read_atm_message(atm_message* message) {
  printf("\n");
  printf("Ingrese la información del evento: (0dia:mes:año:hora:minuto:segundoID Mensaje)\n");
  int atm_id, event_code;
  char event[1000], event_time[21];
  struct tm tm;
  scanf("0%21s%4d %1000[^\n]s", event_time, &atm_id, event);

  message->atm_id = atm_id;
  message->event = malloc(strlen(event));
  strcpy(message->event, event);
  // En el formato del enunciado no se especifica donde va el codigo del evento
  message->event_code = 0;
  if (strptime(event_time, "%d:%m:%Y:%T", &tm) != NULL) {
    message->timestamp = mktime(&tm);
  }
  else {
    return NULL;
  }
  return message;
}

int main(int argc, char *argv[]) {
    
  char *dir;
  int port;
  int port_client;
  int option = 0;
  int port_flag = 0;
  int dir_flag = 0;

  while ((option = getopt(argc, argv,"d:p:l:")) != -1) {
    switch (option) {
       case 'd' : 
          dir_flag = 1;
          dir =  optarg;
          break;
       case 'p' : 
          port_flag = 1;
          port = atoi (optarg);
          break;
       case 'l' : 
          port_client = atoi (optarg); 
          break;
       default:  
           break;
    }
  }

  if (port_flag && dir_flag)
  {}
  else {
    printf("Argumentos insuficientes.\n");
    return 1;
  }
    

  atm_message message;
  read_atm_message(&message);
  printf("ATM ID: %d\n", message.atm_id);
  printf("TIMESTAMP: %ld\n", message.timestamp);
  printf("EVENT: %s\n", message.event);


  // Creación de la conexión del cliente
  int socket_descr;
  struct sockaddr_in socket_dir;

  socket_descr = socket(AF_INET , SOCK_STREAM , 0);
  
  if (socket_descr == -1)
  {
      printf("Error creando el socket.");
  }
  printf("Socket creado correctamente.\n\n" );

  socket_dir.sin_addr.s_addr = inet_addr(dir);
  socket_dir.sin_family = AF_INET;
  socket_dir.sin_port = htons( (intptr_t)port ); // se castea a este tipo porque si no, da un warning

  if (connect(socket_descr , (struct sockaddr *)&socket_dir , sizeof(socket_dir)) < 0)
    {
        perror("Conexión con el servidor fallida. \n \n");
        return 1;
    }
     
    puts("Conexión con el servidor exitosa.\n");

  return 0;
}
