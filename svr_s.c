#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include "svr_s.h"

int main(int argc, char *argv[]) {
  char *dir; // dirección ip del servidor a conectarse
  int port; // puerto del servidor a conectarse
  int option = 0; // variable utilizada por getopt
  int log_flag = 0; // banderas utilizadas para saber si se introdujo la cantidad correcta de argumentos
  int dir_flag = 0;

  while ((option = getopt(argc, argv,"l:b:")) != -1) {
    switch (option) {
       case 'l' :
          dir_flag = 1;
          port =  atoi (optarg);
          break;
       case 'b' :
          log_flag = 1;
          dir = optarg;
          break;
       default:
           break;
    }
  }

  if (!log_flag || !dir_flag) {
    printf("Argumentos insuficientes.\n");
    return 1;
  }
  
  // Ejecutar el servidor, si este termina con un codigo de error, es devuelto
  return listen_svr(port, dir);
}

int listen_svr(int port, char* fn) {

  // Apertura del archivo en modo append para que se mantengan los logs de ejecuciones anteriores
  FILE *f = fopen(fn, "a");
  // Hace que sea unbuffered, asi cuando se escriba se hace de inmediato
  setbuf(f, NULL);

  if (f == NULL) {
    printf("Error abriendo el archivo especificado.\n");
    exit(1);
  }

  // Creación de la conexión del cliente
  int socket_desc , client_sock , c , *new_sock;
  struct sockaddr_in server , client;
   
  socket_desc = socket(AF_INET , SOCK_STREAM , 0);
  if (socket_desc == -1)
  {
      printf("Error creando el socket.");
  }
  puts("Socket creado correctamente.\n");
   
  // Se llena la estructura con la información correspondiente
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( (intptr_t)port );
   
  // Binding
  if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
  {
      perror("Error al crear la asociación");
      return 1;
  }
  puts("Asociación creada exitosamente.");
   
  // Escucha las peticiones de los clientes
  listen(socket_desc , MAX_CONNECTIONS);
   
  // Aceptar conexiones entrantes
  puts("Esperando por conexiones entrantes...");
  c = sizeof(struct sockaddr_in);
   
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
  {
      puts("Conexión establecida");
    
      pthread_t sniffer_thread;
      
      argument args;

      args.socket = client_sock; 
      args.f = f;


      if( pthread_create( &sniffer_thread , NULL ,  connection_handler , &args) < 0)
      {
          perror("Error creando el hilo");
          return 1;
      }
       
      // Se hace un join de los hilos para evitar que la ejecución del programa termine antes que
      // la de los hilos
      //pthread_join( sniffer_thread , NULL);
      puts("Hilo asignado correctamente");
  }
   
  if (client_sock < 0)
  {
      perror("Conexión aceptada correctamente");
      return 1;
  }

  return 0;
}

/*
 * Manejador de la conexión con el cliente
 * */
void *connection_handler(void *argumento)
{
    // Descriptor del socket
    argument *args  = (argument*) argumento;
    int sock = args->socket;
    FILE* f = args->f;
    int read_size;
    char *message , client_message[2000];

    // Envía notificación de que el servidor espera un mensaje
    message = "Servidor en espera de mensaje...\n";
    write(sock , message , strlen(message));

    // Recibir un mensaje del cliente
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        // Envía mensaje de confirmación al cliente
        write(sock , "Mensaje recibido.", strlen("Mensaje recibido."));
        fprintf(f, "Raw message: %s\n", client_message); // Escribir en el log cuando se reciba el mensaje
        fflush(f);
    }
     
    if(read_size == 0)
    {
        puts("Cliente desconectado.");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
     
    return 0;
}
