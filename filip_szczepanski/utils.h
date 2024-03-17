/*
Imię: Filip
Nazwisko: Szczepanski
Numer indeksu: 333262
*/
#ifndef UTILS_H
#define UTILS_H

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

#define MIN_TTL 1
#define MAX_TTL 30
#define MAX_MESSAGE_LEN 300
#define IP4_MAX_LENGTH 20
#define NUM_PAIRS 3
#define BILLION 1000000000L

struct timespec start_avg[3];
struct timespec end_avg[3];
char sender_ip[3][IP4_MAX_LENGTH];

const char* get_input(int argc, char **argv);
double compute_average_difference(struct timespec start_times[NUM_PAIRS], 
                                  struct timespec end_times[NUM_PAIRS]);
void release_resources(int sockfd);

#endif