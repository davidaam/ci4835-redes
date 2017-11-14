/**
 * Ejecutar el servidor que escuchará un cierto puerto y escribirá los log en el
 * archivo al que hace referencia el file descriptor fd. Cada conexión será
 * delegada a un hilo que la procesará
 *
 * @param  port  Puerto en el que el servidor escuchará
 * @param  fd    File descriptor del archivo donde se escribiran los logs
 * @return int   Código de salida
 */
int listen_svr(int port, int fd);

/**
 * Función que maneja cada conexión
 *
 * @param socket  File descriptor del socket. Debe ser casteado
 */
void* connection_handler(void* socket);
