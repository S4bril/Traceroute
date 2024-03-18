/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/
//#ifndef SENT_H
//#define SENT_H

#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <strings.h>

u_int16_t compute_icmp_checksum (const void *buff, int length);

void prepare_ICMP_header(struct icmp* header);

void prepare_adressing(struct sockaddr_in* recipient, const char* ip_str);

//#endif