#define _XOPEN_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "svr_c.h"

atm_message* read_atm_message(atm_message* message) {
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

int main() {
  atm_message message;
  read_atm_message(&message);
  printf("ATM ID: %d\n", message.atm_id);
  printf("TIMESTAMP: %ld\n", message.timestamp);
  printf("EVENT: %s\n", message.event);
  return 0;
}
