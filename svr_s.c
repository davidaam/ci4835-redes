#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
 
//the thread function
void *connection_handler(void *);

// Estructura para almacenar la información del hilo

typedef struct argument {
    int *socket;
    FILE *f;
} argument;
 
int main(int argc, char *argv[]) {
  char *dir; // dirección ip del servidor a conectarse
  int port; // puerto del servidor a conectarse
  int option = 0; // variable utilizada por getopt
  int log_flag = 0; // banderas utilizadas para saber si se introdujo la cantidad correcta de argumentos
  int dir_flag = 0;
  char server_reply[2000];

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

  if (log_flag && dir_flag){} // continue
  else {
    printf("Argumentos insuficientes.\n");
    return 1;
  }


  // Apertura del archivo

  FILE *f = fopen(dir, "w");

  if (f == NULL)
  {
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
  puts("Socket creado correctamente.\n\n");
   
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
  listen(socket_desc , 3);
   
  // Aceptar conexiones entrantes
  puts("Waiting for incoming connections...");
  c = sizeof(struct sockaddr_in);
   
  while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
  {
      puts("Conexión establecida");
    
      pthread_t sniffer_thread;
      new_sock = malloc(1);
      *new_sock = client_sock;
      
      argument args;

      args.socket = new_sock;
      args.f = f;


      if( pthread_create( &sniffer_thread , NULL ,  connection_handler , &args) < 0)
      {
          perror("Error creando el hilo");
          return 1;
      }
       
      //Now join the thread , so that we dont terminate before the thread
      // Esto estaba originalmente comentado
      pthread_join( sniffer_thread , NULL);
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
    int sock = *(int*)args->socket;
    int read_size;
    char *message , client_message[2000];
     
    //const char *text = "Write this to the file";
    //fprintf(f, "Some text: %s\n", text);

    // Envía notificación de que el servidor espera un mensaje
    message = "Servidor en espera de mensaje...\n";
    write(sock , message , strlen(message));

    // Recibir un mensaje del cliente
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        write(sock , "Mensaje recibido.", strlen("Mensaje recibido."));
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
         
    // Libera el apuntador al socket
    free(args->socket);
     
    return 0;
}
