/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

//#ifndef RECEIVE_H
//#define RECEIVE_H

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <time.h>
#include <stdbool.h>

char* receive_ip_from_mesage(int sockfd, int ttl, int id);

bool end_of_route(const char* ip_str, char sender_ip[3][20]);

//#endif