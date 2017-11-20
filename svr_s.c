#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/syscall.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<regex.h>
#include "svr_s.h"

char email[255]; // almacena la direccion de email a donde se enviaran las notificaciones

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

	// Obtiene la direccion de email a ser utilizada
	get_email_address();

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

  // Se llena la estructura del servidor con la información correspondiente
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons( (intptr_t)port );

  // Se establece la información correspondiente al timeout de la conexión
  struct timeval tv;
  tv.tv_sec = 300; // 5 minutos de Timeout (60 segs * 5)
  tv.tv_usec = 0;
  setsockopt(socket_desc, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv,sizeof(struct timeval));

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
		char cmd[255];

    // Envía notificación de que el servidor espera un mensaje
    message = "Servidor en espera de mensaje...\n";
    write(sock , message , strlen(message));

    //Para obtener fecha
    time_t tiempo = time(NULL);
    struct tm *structTiempo = localtime(&tiempo);
    char fecha[128];
    strftime(fecha,128,"%d/%m/%y %H:%M:%S",structTiempo); //Guarda la fecha como string

    //Obtener identificacion del ATM
    pid_t idATM = syscall(SYS_gettid);

    // Recibir un mensaje del cliente
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        // Envía mensaje de confirmación al cliente
        write(sock , "Mensaje recibido.", strlen("Mensaje recibido."));
        //Obtener codigo y patron del evento
        int codigo = 0;
        char *patron;

        if (strstr(client_message, "Communication Offline"))
        {
          patron = "Communication Offline";
          codigo = 1;
        }
        else if (strstr(client_message, "Communication error"))
        {
          patron = "Communication error";
          codigo = 2;
        }
        else if (strstr(client_message, "Low cash alert"))
        {
          patron = "Low cash alert";
          codigo = 3;
        }
        else if (strstr(client_message, "Running Out of notes is casette"))
        {
          patron = "Running Out of notes is casette";
          codigo = 4;
        }
        else if (strstr(client_message, "empty"))
        {
          patron = "empty";
          codigo = 5;
        }
        else if (strstr(client_message, "Service mode entered"))
        {
          patron = "Service mode entered";
          codigo = 6;
        }
        else if (strstr(client_message, "Service mode left"))
        {
          patron = "Service mode left";
          codigo = 7;
        }
        else if (strstr(client_message, "device did not answer as expected"))
        {
          patron = "device did not answer as expected";
          codigo = 8;
        }
        else if (strstr(client_message, "The protocol was cancelled"))
        {
          patron = "The protocol was cancelled";
          codigo = 9;
        }
        else if (strstr(client_message, "Low Paper warning"))
        {
          patron = "Low Paper warning";
          codigo = 10;
        }
        else if (strstr(client_message, "Printer Error"))
        {
          patron = "Printer Error";
          codigo = 11;
        }
        else if (strstr(client_message, "Paper-out condition"))
        {
          patron = "Paper-out condition";
          codigo = 12;
        }
        else
        {
          patron = " ";
        }

        if (strcmp (patron, " ") == 0) {
          fprintf(args->f, "%s %d %s \n", fecha, idATM, client_message);
					sprintf(cmd,"echo '%s %d %s \n' | mail -s 'Reporte de operaciones' %s", fecha, idATM, client_message, email);
					system(cmd);
        }
        else {
          fprintf(args->f, "%s %d %d %s %s \n", fecha, idATM, codigo, patron, client_message);
					sprintf(cmd,"echo '%s %d %d %s %s \n' | mail -s 'Reporte de operaciones' %s", fecha, idATM, codigo, patron, client_message, email);
					system(cmd);
        }

        fflush(args->f);
    }

    if(read_size == 0)
    {
        puts("Cliente desconectado.");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        fprintf(f, "Raw message: Tiempo limite excedido\n"); // Escribir en el log cuando no se recibe un mensaje en el tiempo limite
        fflush(f);
    }

    return 0;
}


void get_email_address() {
	FILE *fp; // file descriptor para abrir el archivo que contiene el correo electronico
	fp = fopen("agenda.txt","r");

	if( fp == NULL )
	{
		perror("Error abriendo el archivo con la dirección de correo electrónico.\n");
		exit(EXIT_FAILURE);
	}

	while(fgets(email, 255, (FILE*) fp)) {} // Lee el contenido del archivo que contiene la dirección de email

	// Si no hay ninguna dirección de correo en agenda, se coloca una por defecto
	if(strcmp(email, "\n")) {
		sprintf(email, "prueba@gmail.com");
	}

	fclose(fp);
}
