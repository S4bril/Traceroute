#include "utils.h"

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

double compute_average_difference(struct timespec start_times[NUM_PAIRS], struct timespec end_times[NUM_PAIRS]) {
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
