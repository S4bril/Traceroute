/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/

#include "sent.h"
#include "receive.h"

#define IP4_MAX_LENGTH 20
#define MIN_TTL 1
#define MAX_TTL 30
#define NUM_PAIRS 3
#define BILLION 1000000000L

struct timespec start_avg[3];
struct timespec end_avg[3];
char sender_ip[3][IP4_MAX_LENGTH];

const char* get_input(int argc, char **argv){
   if (argc == 1){
      printf("You forgot to pass IP address\n");
      return "";
   }
   else if (argc == 2){
      char ip_address[INET_ADDRSTRLEN];
      const char *ip_str = argv[1];
      
      if (inet_pton(AF_INET, ip_str, &ip_address) != 1) {
         printf("The IP address \"%s\" is not valid.\n", ip_str);
         return "";
      }
      return ip_str;
   }
   else{
      printf("Too many arguments. Pass just IP address.\n");
      return "";
   }
}

double compute_average_difference(struct timespec start_times[], struct timespec end_times[]) {
    double total_difference_ms = 0.0;

    for (int i = 0; i < NUM_PAIRS; i++) {
        double start_ns = (double)(start_times[i].tv_sec * BILLION + start_times[i].tv_nsec);
        double end_ns = (double)(end_times[i].tv_sec * BILLION + end_times[i].tv_nsec);
        double difference_ns = end_ns - start_ns;
        total_difference_ms += difference_ns / 1000000.0;
    }

    double average_difference_ms = total_difference_ms / NUM_PAIRS;
    return average_difference_ms;
}

void release_resources(int sockfd)
{
   close(sockfd);
}

int compute_time_diff(struct timespec start, struct timespec end){
   return (end.tv_sec - start.tv_sec) * 1000 + (end.tv_nsec - start.tv_nsec) / 1000000;
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
               char* caught  = receive_ip_from_mesage(sockfd, ttl, getpid() & 0xFFFF);
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
