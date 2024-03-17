/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "receive.h"

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
      if(header->icmp_type == 11){
         header = (struct icmp*)((void*)header + 28);
      }

      if(header->icmp_hun.ih_idseq.icd_seq != ttl){
         return "0";
      }

      return sender_ip_str;
}

void print_path(int ttl, char sender_ip[3][IP4_MAX_LENGTH], int received){
   printf("%d. ", ttl);
   if(ttl < 10) { printf(" "); }
   printf("%s ", sender_ip[0]);
   if(received > 1){
      if (strcmp(sender_ip[0], sender_ip[1]) != 0) {printf("%s ", sender_ip[1]);}
   }
   if(received > 2){
      if (strcmp(sender_ip[0], sender_ip[2]) != 0 && strcmp(sender_ip[1], sender_ip[2]) != 0){
         printf("%s ", sender_ip[2]);
      }
      printf(" %.3f ms\n", compute_average_difference(start_avg, end_avg)); 
   }
}

bool end_of_route(const char* ip_str, char sender_ip[3][IP4_MAX_LENGTH]){
      if (strcmp(sender_ip[0], ip_str) == 0
      || strcmp(sender_ip[1], ip_str) == 0
      || strcmp(sender_ip[2], ip_str) == 0) { return true; }
      return false; 
}