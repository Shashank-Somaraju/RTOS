#include<sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include "pthread.h"
#include <stddef.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define buffSIZE 1024
#define length 1024

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
int serv_socket, new_socket;
static const pa_sample_spec ss = {
  .format = PA_SAMPLE_S16LE,
  .rate = 44100,
  .channels = 2
};

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
    // Playing the recorded message
    if (pa_simple_read(s, buff, sizeof(buff), &error) < 0) {
      fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
    }
    int val = write(new_socket, buff, sizeof(buff));
  }


}


int main(int argc, char const *argv[])
{
  struct sockaddr_in address_serv, client_serv;
  int addrlen = sizeof(address_serv);

  if((serv_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    perror("Socket  creation Error");
    exit(1);
  }

  memset(&address_serv, '\0', sizeof(address_serv));
  address_serv.sin_family = AF_INET;
  address_serv.sin_addr.s_addr = INADDR_ANY;
  address_serv.sin_port = htons(9090);

  printf("\nBinding....\n");
  if((bind(serv_socket, (struct sockaddr *)&address_serv,sizeof(address_serv))) < 0){
    perror("Bind error");
    exit(1);
  }

  // Listening to clients
  printf("\nlistening....\n");
  if(listen(serv_socket, 50) < 0){
    perror("Listen error");
    exit(1);
  }

  printf("\nWaiting for connection....\n");
  new_socket = accept(serv_socket, (struct sockaddr *)&address_serv, (socklen_t*)&addrlen);
    if( new_socket< 0){
      return 1;
    }
    pthread_t id;
    pthread_create(&id,NULL,func1,NULL);



    pa_simple *s = NULL;
    int ret = 1;
    int error;

    if (!(s = pa_simple_new(NULL,argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
      fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
      return 0;
    }
    uint8_t buff[buffSIZE];
    while(1){
      ssize_t r;
      int val = read(new_socket, buff, sizeof(buff));
      if (pa_simple_write(s, buff, sizeof(buff), &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
      }
    }

}
