// all
#include <stdio.h>

// inet_pton(...)
#include <arpa/inet.h>

// ICMP
#include <netinet/ip_icmp.h>

// getpid()
#include <unistd.h>

// compute_icmp_checksum
#include <assert.h>
#include <stdlib.h>

//bzero
#include <string.h>

// geting socketfd
#include <errno.h>

// poll (receiving pckages)
#include <poll.h>
#include <time.h>

#define MIN_TTL 1
#define MAX_TTL 30
#define MAX_MESSAGE_LEN 300

void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);	
}

int receive_mesage(int sockfd, int ttl, char* message_buff)
{
      int received_ttls = 0;
      struct sockaddr_in sender;
      socklen_t sender_len = sizeof(sender);
      u_int8_t buffer[IP_MAXPACKET];

      ssize_t packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr*)&sender, &sender_len);
      if (packet_len < 0) {
         return -1;
      }

      char sender_ip_str[20]; 
      inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));

      struct ip* ip_header = (struct ip*) buffer;

      ssize_t	ip_header_len = 4 * (ssize_t)(ip_header->ip_hl);

      struct icmp* header = (struct icmp*)((uint8_t*)ip_header + ip_header_len);
      if(header->icmp_type == 11){
         header = (struct icmp*)((void*)header + 28);
      }

      printf("seq: %d\n", header->icmp_hun.ih_idseq.icd_seq);
      if(header->icmp_hun.ih_idseq.icd_seq != ttl){
         return 0;
      }

      // printf ("Received IP packet with ICMP content from: %s\n", sender_ip_str);

      // printf("IP header: ");
      // print_as_bytes(buffer, ip_header_len);
      // printf("\n");

      // printf("IP data:   ");
      // print_as_bytes(buffer + ip_header_len, packet_len - ip_header_len);
      // printf("\n\n");

      sprintf(message_buff, "Received IP packet with ICMP content from: %s\n", sender_ip_str);
      strcat(message_buff, "IP header: ");
      strcat(message_buff, "\n");
      strcat(message_buff, "IP data:   ");
      strcat(message_buff, "\n\n");

      printf("xdd\n");

      return 1;
}

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
      }
      return ip_str;
   }
   else{
      printf("Too many arguments. Pass just IP address.\n");
      return "";
   }
}

int main(int argc, char **argv) {
   const char *ip_str = get_input(argc, argv);
   if(ip_str == ""){ return EXIT_FAILURE; }

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
   pid_t pid = getpid() & 0xFFFF;
   header.icmp_type = ICMP_ECHO;
   header.icmp_code = 0;
   header.icmp_hun.ih_idseq.icd_id = pid;
   header.icmp_cksum = 0;

   // addressing
   struct sockaddr_in recipient;
   bzero (&recipient, sizeof(recipient));
   recipient.sin_family = AF_INET;
   inet_pton(AF_INET, ip_str, &recipient.sin_addr);

   // prepare buffer for printing
   char message_buffer[MAX_MESSAGE_LEN];

   // send packages
   while(ttl <= MAX_TTL){
      memset(message_buffer, 0, MAX_MESSAGE_LEN);
      printf("ttl: %d \n", ttl);
      // change socket settings
      if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) != 0){
         fprintf(stderr, "setsockopt error: %s\n", strerror(errno)); 
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
         ssize_t bytes_sent = sendto (
            sockfd,
            &header,
            sizeof(header),
            0,
            (struct sockaddr*)&recipient,
            sizeof(recipient)
            );
      }

      // receive packages
      struct pollfd ps;
      ps.fd = sockfd;
      ps.events = POLLIN;
      ps.revents = 0;
      int ready = 0;
      // TO DO: wait max 1 sec or earlier 
      // clock_t start, end;
      // double cpu_time_used;
      ready = poll(&ps, 1, 1000);

      printf("ready: %d\n", ready);

      if(ready < 0){
         fprintf(stderr, "Poll error: %s\n", strerror(errno)); 
		   return EXIT_FAILURE;
      }
      else if(ready == 0){
         printf("*\n");
      }
      else{
            int received = 0;
            
            for(int i = 0; i < ready; i++){
               if (ps.revents & POLLIN){
                  int r = receive_mesage(sockfd, ttl, message_buffer);
                  if(r < 0){
                     fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
                     return EXIT_FAILURE;
                  }
                  received += r;
                  ready--;
               }
               else{ break; }
            }

            if (received == 0){
               printf("*\n");
            }
            else if (received < 3){
               printf("???\n");
            }
            else{
               printf("%s", message_buffer);
            }
      }

      // increase TTL value
      ttl++;
   }

   // end program
   close (sockfd);
	return EXIT_SUCCESS;
}
