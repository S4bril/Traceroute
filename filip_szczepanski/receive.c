/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "receive.h"

#define ICMP_TIME_EXCEEDED_OFFSET 28

char* receive_ip_from_mesage(int sockfd, int ttl)
{
      struct sockaddr_in sender;
      socklen_t sender_len = sizeof(sender);
      u_int8_t buffer[IP_MAXPACKET];

      ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
      if (packet_len < 0) {
         return "-1";
      }

      char *sender_ip_str = malloc(INET_ADDRSTRLEN);
      if (sender_ip_str == NULL) {
         return "-1";
      }
      inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, INET_ADDRSTRLEN);

      struct ip* ip_header = (struct ip*) buffer;

      ssize_t	ip_header_len = 4 * (ssize_t)(ip_header->ip_hl);

      struct icmp* header = (struct icmp*)((uint8_t*)ip_header + ip_header_len);
      if(header->icmp_type == ICMP_TIME_EXCEEDED){
         header = (struct icmp*)((void*)header + ICMP_TIME_EXCEEDED_OFFSET);
      }

      if(header->icmp_hun.ih_idseq.icd_seq != ttl){
         return "0";
      }

      return sender_ip_str;
}

bool end_of_route(const char* ip_str, char sender_ip[3][20]){
      if (strcmp(sender_ip[0], ip_str) == 0
      || strcmp(sender_ip[1], ip_str) == 0
      || strcmp(sender_ip[2], ip_str) == 0) { return true; }
      return false; 
}