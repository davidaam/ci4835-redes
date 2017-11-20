#include <stdint.h>

typedef struct {
  int atm_id;
  int event_code;
  char* event;
  time_t timestamp;
} atm_message;

/**
 * Leer un mensaje de la entrada estándar y devolverlo en el formato del struct
 * @param message:       Apuntador al atm_message donde se colocara el mensaje
 *                       leido. Tambien es retornado.
 * @return atm_message*  Mensaje leído
 */
atm_message* read_atm_message(atm_message* message);

/**
 * Enviar mensaje
 *
 * @param  message  Mensaje a ser enviado
 * @param  socket   File descriptor del socket
 * @return int      Código de respuesta
 */
int send_atm_message(atm_message* message, int socket);

/**
 * Establecer conexión con el SVR
 *
 * @param  ip    IP del SVR
 * @param  port  Puerto del SVR
 * @return int   File descriptor del socket
 */
int connect_to_svr(int socket, char* ip, uint16_t port);

/**
 * Establecer puerto local en el cliente
 *
 * @param  socket  File descriptor del socket
 * @param  port    Puerto del cliente
 * @return int     -1 si hubo error, 1 en caso de que sea exitoso.
 */
int bind_local_port(int socket_descr, uint16_t port);