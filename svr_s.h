// Estructura para almacenar la información del hilo
typedef struct argument {
  int socket;
  FILE* f;
} argument;

/**
 * Ejecutar el servidor que escuchará un cierto puerto y escribirá los log en el
 * archivo al que hace referencia el file descriptor fd. Cada conexión será
 * delegada a un hilo que la procesará
 *
 * @param  port  Puerto en el que el servidor escuchará
 * @param  fn    Nombre del archivo donde se escribiran los logs
 * @return int   Código de salida
 */
int listen_svr(int port, char* fn);

/**
 * Función que maneja cada conexión en un hilo
 *
 * @param arguments  Argumentos necesarios (file descriptor del socket y file pointer del log).
 *                   Debe ser casteado a struct argument
 */
void* connection_handler(void* arguments);

/*Función que se encarga de leer una dirección de correo electrónico desde un archivo de texto
con nombre 'agenda.txt'*/
void get_email_address();

const int MAX_CONNECTIONS = 1000;
