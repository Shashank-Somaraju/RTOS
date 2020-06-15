#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdlib.h>
#include "pthread.h"
#include <stddef.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#define BUFSIZE 1024
#define buffSIZE 1024

static const pa_sample_spec ss = {
	.format = PA_SAMPLE_S16LE,
	.rate = 44100,
	.channels = 2
};
int client_socket;


void* func1(void* input){
	pa_simple *s = NULL;
	int ret = 1;
	int error;

	if (!(s = pa_simple_new(NULL, "myapp", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		return NULL;
	}
	uint8_t buff[buffSIZE];
	while(1){
		if (pa_simple_read(s, buff, sizeof(buff), &error) < 0) {
			fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
		}
		int val = write(client_socket, buff, sizeof(buff));
		//printf("Writing %d\n", val);
	}
}


int main(int argc, char *argv[]){

	struct sockaddr_in address_serv;

	printf("\nCreating the socket....\n");
	if((client_socket= socket(AF_INET, SOCK_STREAM, 0)) < 0){
		perror("Socket error");
		exit(1);
	}

	memset(&address_serv, 0, sizeof(address_serv));
	address_serv.sin_family = AF_INET;
	address_serv.sin_port = htons(9090);

	if(inet_pton(AF_INET, argv[1], &address_serv.sin_addr)<=0){
		perror("pton error");
	}

	printf("\nWaiting for connection....\n");
	if((connect(client_socket, (struct sockaddr *)&address_serv, sizeof(address_serv))) < 0){
		perror("failed connection...");
		exit(1);
	}
	printf("\nConnection established....\n\n");


	pthread_t id;
	pthread_create(&id,NULL,func1,NULL);


	pa_simple *s = NULL;
	int ret = 1;
	int error;

	if (!(s = pa_simple_new(NULL,"myapp", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
		return 20;
	}

	// Sending recorded data
	uint8_t buff[BUFSIZE];
	while(1){
		if(read(client_socket, buff, sizeof(buff))<=0){
			printf("Fail to read");
			return 0;
		}

		/* Record some data ... */
		if (pa_simple_write(s, buff, sizeof(buff), &error) < 0) {
			fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
			return 20;
		}

	}


}
