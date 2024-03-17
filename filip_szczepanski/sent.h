/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "utils.h"
u_int16_t compute_icmp_checksum (const void *buff, int length);
void prepare_ICMP_header(struct icmp* header);
void prepare_adressing(struct sockaddr_in* recipient, const char* ip_str);