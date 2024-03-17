/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "utils.h"

char* receive_ip_from_mesage(int sockfd, int ttl);

void print_path(int ttl, char sender_ip[3][IP4_MAX_LENGTH], int received);

bool end_of_route(const char* ip_str, char sender_ip[3][IP4_MAX_LENGTH]);