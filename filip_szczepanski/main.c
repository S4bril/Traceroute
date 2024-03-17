/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "utils.h"
#include "sent.h"
#include "receive.h"

int compute_time_diff(struct timespec start, struct timespec end){
   return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
}

int main(int argc, char **argv) {
   const char *ip_str = get_input(argc, argv);
   if(strcmp(ip_str, "") == 0){ return EXIT_FAILURE; }

   // -------------------- program ---------------------------
   int ttl = MIN_TTL;

   // declare socket
   int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
   if(sockfd < 0){
      fprintf(stderr, "socket error: %s\n", strerror(errno));
		return EXIT_FAILURE;
   }

   // prepare ICMP header
   struct icmp header;
   prepare_ICMP_header(&header);

   // addressing
   struct sockaddr_in recipient;
   prepare_adressing(&recipient, ip_str);

   // send packages
   while(ttl <= MAX_TTL){
      // change socket settings
      if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) != 0){
         fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
         release_resources(sockfd);
		   return EXIT_FAILURE;
      }

      // update sequence number
      header.icmp_hun.ih_idseq.icd_seq = ttl;

      // update cksum
      header.icmp_cksum = 0;
      header.icmp_cksum = compute_icmp_checksum (
         (u_int16_t*)&header, sizeof(header));

      // send package 3 times
      for(int i = 0; i < 3; i++){
         clock_gettime(CLOCK_MONOTONIC, &start_avg[i]);
         ssize_t bytes_sent = sendto (
            sockfd,
            &header,
            sizeof(header),
            0,
            (struct sockaddr*)&recipient,
            sizeof(recipient)
            );
         if (bytes_sent < 0){
            fprintf(stderr, "setsockopt error: %s\n", strerror(errno));
            release_resources(sockfd);
		      return EXIT_FAILURE;
         }
      }

      // receive packages
      struct pollfd ps;
      ps.fd = sockfd;
      ps.events = POLLIN;
      ps.revents = 0;
      int ready = 0;

      struct timespec start, end;
      clock_gettime(CLOCK_MONOTONIC, &start);
      int received = 0;

      while(compute_time_diff(start, end) < 1000 && received < 3){
         ready = poll(&ps, 1, 1000);
         if(ready < 0){
            fprintf(stderr, "Poll error: %s\n", strerror(errno));
            release_resources(sockfd);
            return EXIT_FAILURE;
         }
         while(ready > 0){
            if (ps.revents & POLLIN){
               clock_gettime(CLOCK_MONOTONIC, &end_avg[received]);
               char* caught  = receive_ip_from_mesage(sockfd, ttl);
               if(strcmp(caught, "-1") == 0) { 
                  fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
                  release_resources(sockfd); 
                  return EXIT_FAILURE; 
               }
               else if(strcmp(caught, "0") != 0){
                  strcpy(sender_ip[received], caught);
                  received++;
               }
               ready--;
            }
         }
      clock_gettime(CLOCK_MONOTONIC, &end);
      }

         if (received == 0){
            if(ttl < 10) { printf("%d.  *\n", ttl);}
            else { printf("%d. *\n", ttl); }
         }
         else if (received < 3){
            print_path(ttl, sender_ip, received);
            printf(" ???\n");
         }
         else{
            print_path(ttl, sender_ip, received);
         }

         if(end_of_route(ip_str, sender_ip)){
            release_resources(sockfd);
            return EXIT_SUCCESS;
         }

         // increase TTL value
         ttl++;
      }

   // end program
   release_resources(sockfd);
	return EXIT_SUCCESS;
}
