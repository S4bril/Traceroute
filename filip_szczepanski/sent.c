/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "sent.h"

u_int16_t compute_icmp_checksum (const void *buff, int length)
{
    const u_int16_t* ptr = buff;
    u_int32_t sum = 0;
    assert (length % 2 == 0);
    for (; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16U) + (sum & 0xffffU);
    return (u_int16_t)(~(sum + (sum >> 16U)));
}

void prepare_ICMP_header(struct icmp* header){
   pid_t pid = getpid() & 0xFFFF;
   header->icmp_type = ICMP_ECHO;
   header->icmp_code = 0;
   header->icmp_hun.ih_idseq.icd_id = pid;
   header->icmp_cksum = 0;
}

void prepare_adressing(struct sockaddr_in* recipient, const char* ip_str){
   bzero (recipient, sizeof(*recipient));
   recipient->sin_family = AF_INET;
   inet_pton(AF_INET, ip_str, &recipient->sin_addr);
}