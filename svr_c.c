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

int connect_to_svr(int socket, char* ip, uint16_t port) {
  struct sockaddr_in socket_dir;
  socket_dir.sin_addr.s_addr = inet_addr(ip);
  socket_dir.sin_family = AF_INET;
  socket_dir.sin_port = htons( (intptr_t)port ); // se castea a este tipo porque si no, da un warning

  if (connect(socket , (struct sockaddr *)&socket_dir , sizeof(socket_dir)) < 0)
    {
        perror("Conexión con el servidor fallida. \n \n");
        return 0;
    }
     
    puts("Conexión con el servidor exitosa.\n");
    return 1;
}

int main(int argc, char *argv[]) {
  char *dir; // dirección ip del servidor a conectarse
  int port; // puerto del servidor a conectarse
  int port_client; // puerto del cliente
  int option = 0; // variable utilizada por getopt
  int port_flag = 0; // banderas utilizadas para saber si se introdujo la cantidad correcta de argumentos
  int dir_flag = 0;
  char server_reply[2000];

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

  if (port_flag && dir_flag){} // continue
  else {
    printf("Argumentos insuficientes.\n");
    return 1;
  }
  

  // Creación de la conexión del cliente
  int socket_descr;

  socket_descr = socket(AF_INET , SOCK_STREAM , 0);
  
  if (socket_descr == -1)
  {
      printf("Error creando el socket.");
  }
  printf("Socket creado correctamente.\n\n" );
  
  // Si se quiere probar sin necesidad de un servidor, 
  // colocar if (1) {...} en vez de if(connect_to_...){...}
  if (connect_to_svr(socket_descr, dir, port))
  {
    while (1) {
      atm_message message;
      read_atm_message(&message);
      printf("ATM ID: %d\n", message.atm_id);
      printf("TIMESTAMP: %ld\n", message.timestamp);
      printf("EVENT: %s\n", message.event);

      //send_atm_message(&message, socket_descr); // Falta implementar

      
      // Enviar información al servidor
      if( send(socket_descr, message.event , strlen(message.event) , 0) < 0)
      {
          puts("Falló el envío del mensaje al servidor.");
          return 1;
      }
       
      // Recibir información del servidor
      if( recv(socket_descr, server_reply, 2000 , 0) < 0)
      {
          puts("Fallo al recibir respuesta del servidor.");
          break;
      }
    }
  }


  close(socket_descr);

  return 0;
}
